#include "Mesh.h"



Mesh::Mesh()
{
}
Mesh::Mesh(std::vector<Vertex> &v, std::vector<unsigned int> &i)
{
	verts = v;
	ind = i;
	count = ind.size();
	CreateBuffer();
}
Mesh::Mesh(std::vector<Vertex> &v)
{
	verts = v;
	count = v.size();

	std::vector<Vertex> vertBufData;
	vertBufData = v;

	glGenVertexArrays(1, &vertArr);
	glGenBuffers(1, &vertBuf);

	// placing our buffer on the machine
	glBindVertexArray(vertArr);
	glBindBuffer(GL_ARRAY_BUFFER, vertBuf);

	// binding our data to the buffer on the machine, know they wont change in size not matter what
	glBufferData(GL_ARRAY_BUFFER,
		sizeof(Vertex)*count,
		&vertBufData[0],
		GL_STATIC_DRAW);

	// letting the computer know how the buffer is structured
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, // the index
		3, // number of components to expect(x,y,z)
		GL_FLOAT, // Type of data
		GL_FALSE, // should we normalize the data
		sizeof(glm::vec3), // stride
		0); // the offset 
	glBindVertexArray(0);
}
Mesh::Mesh(std::string filename, MeshType m, std::string file)
{
	using namespace std;
	ifstream inFile;
	string info;
	meshType = m;
	kindomHeartsLevel = file;

	vector<glm::vec3> pos;
	vector<glm::vec2> uv;
	vector<glm::vec3> nor;
	vector<VertInd> in;

	string matName;

	inFile.open(filename);
	if(inFile.is_open())
	{
		while(getline(inFile, info))
		{
			istringstream ss(info);
			string type;

			ss >> type;
			if (meshType == MultipleMesh) {
				if (type == "g")
				{
					if(pos.size() != 0)
					{
						MiniMesh newMesh;
						newMesh.count = in.size();

						std::vector<Vertex> vertBufData;

						for (unsigned int i = 0; i < newMesh.count; i++)
						{
							Vertex newVert;
							newVert.pos = pos[in[i].posInd - 1];
							newVert.uv = uv[in[i].uvInd - 1];
							newVert.normal = { 0,1,0 };

							vertBufData.push_back(newVert);
						}
						// generating buffer and storing their addresses to out init variables
						glGenVertexArrays(1, &newMesh.newVertArr);
						glGenBuffers(1, &newMesh.newVertBuf);

						// placing our buffer on the machine
						glBindVertexArray(newMesh.newVertArr);
						glBindBuffer(GL_ARRAY_BUFFER, newMesh.newVertBuf);

						// binding our data to the buffer on the machine, know they wont change in size not matter what
						glBufferData(GL_ARRAY_BUFFER,
							sizeof(Vertex)*newMesh.count,
							&vertBufData[0],
							GL_STATIC_DRAW);

						// letting the computer know how the buffer is structured
						glEnableVertexAttribArray(3);
						glEnableVertexAttribArray(4);
						glEnableVertexAttribArray(5);
						glVertexAttribPointer(3, // the index
							3, // number of components to expect(x,y,z)
							GL_FLOAT, // Type of data
							GL_FALSE, // should we normalize the data
							sizeof(Vertex), // stride
							0); // the offset 

						glVertexAttribPointer(4,
							2, // number of components to expect(u,v)
							GL_FLOAT,
							GL_FALSE,
							sizeof(Vertex),
							reinterpret_cast<void *>(offsetof(Vertex, uv)));

						glVertexAttribPointer(5,
							3,// number of components to expect(x,y,z)
							GL_FLOAT,
							GL_FALSE,
							sizeof(Vertex),
							reinterpret_cast<void *>(offsetof(Vertex, normal)));
						glBindVertexArray(0);

						FREE_IMAGE_FORMAT type = FreeImage_GetFileType(&matLibs.at(matName).kdFileName[0], 0);
						FIBITMAP* image = FreeImage_Load(type, &matLibs.at(matName).kdFileName[0]);
						if (image == nullptr) return;

						FIBITMAP* image32Bit = FreeImage_ConvertTo32Bits(image);
						FreeImage_Unload(image);

						glGenTextures(1, &newMesh.texID);
						glBindTexture(GL_TEXTURE_2D, newMesh.texID);

						int texWidth = FreeImage_GetWidth(image32Bit);
						int texHeight = FreeImage_GetHeight(image32Bit);
						BYTE* texAddress = FreeImage_GetBits(image32Bit);

						glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB_ALPHA, texWidth, texHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, (void*)texAddress);

						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

						FreeImage_Unload(image32Bit);
						glBindTexture(GL_TEXTURE_2D, 0);
						newMesh.hasTex = true;


						myMeshes.push_back(newMesh);

						in.clear();
					}
				}
				else if (type == "usemtl")
				{
					ss >> matName;
				}
				else if (type == "mtllib")
				{
					ifstream newInFile;
					string newInfo;

					string matFileName;
					ss >> matFileName;

					newInFile.open(kindomHeartsLevel + matFileName);
					if (newInFile.is_open())
					{
						string name;
						while (getline(newInFile, newInfo))
						{
							istringstream dd(newInfo);
							string newType;
							dd >> newType;

							if(newType == "newmtl")
							{
								dd >> name;

								MatLib newMat;
								matLibs.insert(pair<string, MatLib>(name, newMat));
							}
							else if (newType == "illum")
							{
								unsigned int ill;
								dd >> ill;

								matLibs.at(name).illum = ill;
							}
							else if (newType == "Kd")
							{
								float x, y, z;
								dd >> x >> y >> z;

								matLibs.at(name).diffuse = glm::vec3(x,y,z);
							}
							else if (newType == "Ka")
							{
								float x, y, z;
								dd >> x >> y >> z;

								matLibs.at(name).ambience = glm::vec3(x, y, z);
							}
							else if (newType == "Tf")
							{
								float x, y, z;
								dd >> x >> y >> z;

								matLibs.at(name).Tf = glm::vec3(x, y, z);
							}
							else if (newType == "Ni")
							{
								float x;
								dd >> x;

								matLibs.at(name).Ni = x;
							}
							else if (newType == "map_Kd")
							{
								string newFileName;
								dd >> newFileName;

								matLibs.at(name).kdFileName = kindomHeartsLevel + newFileName;
							}
							else if (newType == "map_tr")
							{
								string newFileName;
								dd >> newFileName;

								matLibs.at(name).trFileName = kindomHeartsLevel + newFileName;
							}
						}
					}
					newInFile.close();
				}
			}
			if(type == "v")
			{
				glm::vec3 ps;
				ss >> ps.x;
				ss >> ps.y;
				ss >> ps.z;
				pos.push_back(ps);
			}
			else if (type == "vt")
			{
				glm::vec2 ps;
				ss >> ps.x;
				ss >> ps.y;
				uv.push_back(ps);
			}
			else if (type == "vn")
			{
				glm::vec3 ps;
				ss >> ps.x;
				ss >> ps.y;
				ss >> ps.z;
				nor.push_back(ps);
			}
			else if (type == "f")
			{
				if (meshType == MultipleMesh) {
					for (int i = 0; i < 3; i++)
					{
						char slash;
						VertInd indices;
						ss >> indices.posInd;
						ss >> slash;
						ss >> indices.uvInd;
	
						in.push_back(indices);
						}
				}
				else{
					for (int i = 0; i < 3; i++)
					{
						char slash;
						VertInd indices;
						ss >> indices.posInd;
						ss >> slash;
						ss >> indices.uvInd;

						ss >> slash;
						ss >> indices.normInd;

						in.push_back(indices);
					}
				}
			}
		}
		inFile.close();
	}

	if (meshType == SingleMesh) {
		moreInd = in;
		count = moreInd.size();
		CreateModelLoadingBuffer(pos, uv, nor);
	}
}

Mesh::~Mesh()
{
	glDeleteVertexArrays(1, &vertArr);
	glDeleteBuffers(1, &vertBuf);
}
void Mesh::CreateModelLoadingBuffer(std::vector<glm::vec3> pos, std::vector<glm::vec2> uv, std::vector<glm::vec3> nor)
{
	std::vector<Vertex> vertBufData;

	for (unsigned int i = 0; i < count; i++)
	{
			Vertex newVert;
			newVert.pos = pos[moreInd[i].posInd - 1];
			newVert.uv = uv[moreInd[i].uvInd - 1];

			if (moreInd.size() < 90000) {
				newVert.normal = nor[moreInd[i].normInd - 1];
			}
			else {
				newVert.normal = { 0,1,0 };
			}					
			vertBufData.push_back(newVert);
			/*if ((i + 1) % 3 == 0) {
				glm::vec3 totNorm = { 0,0,0 };
				for (int j = 2; j >= 0; j--) {
					totNorm += vertBufData[i - j].normal;
				}
				for (int j = 2; j >= 0; j--) {
					vertBufData[i - j].normal = glm::normalize(totNorm);
				}
			}*/
	}
	verts = vertBufData;
	// generating buffer and storing their addresses to out init variables
	glGenVertexArrays(1, &vertArr);
	glGenBuffers(1, &vertBuf);

	// placing our buffer on the machine
	glBindVertexArray(vertArr);
	glBindBuffer(GL_ARRAY_BUFFER, vertBuf);

	// binding our data to the buffer on the machine, know they wont change in size not matter what
	glBufferData(GL_ARRAY_BUFFER,
		sizeof(Vertex)*count,
		&vertBufData[0],
		GL_STATIC_DRAW);

	// letting the computer know how the buffer is structured
	glEnableVertexAttribArray(3);
	glEnableVertexAttribArray(4);
	glEnableVertexAttribArray(5);
	glVertexAttribPointer(3, // the index
		3, // number of components to expect(x,y,z)
		GL_FLOAT, // Type of data
		GL_FALSE, // should we normalize the data
		sizeof(Vertex), // stride
		0); // the offset 

	glVertexAttribPointer(4,
		2, // number of components to expect(u,v)
		GL_FLOAT,
		GL_FALSE,
		sizeof(Vertex),
		reinterpret_cast<void *>(offsetof(Vertex, uv)));

	glVertexAttribPointer(5,
		3,// number of components to expect(x,y,z)
		GL_FLOAT,
		GL_FALSE,
		sizeof(Vertex),
		reinterpret_cast<void *>(offsetof(Vertex, normal)));
	glBindVertexArray(0);
}

void Mesh::CreateBuffer()
{
	std::vector<Vertex> vertBufData(count);

	for (unsigned int i = 0; i < count; i++)
	{
		vertBufData[i] = verts[ind[i]];
	}

	// generating buffer and storing their addresses to out init variables
	glGenVertexArrays(1, &vertArr);
	glGenBuffers(1, &vertBuf);

	// placing our buffer on the machine
	glBindVertexArray(vertArr);
	glBindBuffer(GL_ARRAY_BUFFER, vertBuf);

	// binding our data to the buffer on the machine, know they wont change in size not matter what
	glBufferData(GL_ARRAY_BUFFER,
		sizeof(Vertex)*count,
		&vertBufData[0],
		GL_STATIC_DRAW);


	// letting the computer know how the buffer is structured
	glEnableVertexAttribArray(3);
	glEnableVertexAttribArray(4);
	glEnableVertexAttribArray(5);
	glVertexAttribPointer(3, // the index
		3, // number of components to expect(x,y,z)
		GL_FLOAT, // Type of data
		GL_FALSE, // should we normalize the data
		sizeof(Vertex), // stride
		0); // the offset 
	
		glVertexAttribPointer(4,
			2, // number of components to expect(u,v)
			GL_FLOAT,
			GL_FALSE,
			sizeof(Vertex),
			reinterpret_cast<void *>(offsetof(Vertex, uv)));

		glVertexAttribPointer(5,
			3,// number of components to expect(x,y,z)
			GL_FLOAT, 
			GL_FALSE, 
			sizeof(Vertex), 
			reinterpret_cast<void *>(offsetof(Vertex, normal)));
	glBindVertexArray(0);
}
