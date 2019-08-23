#pragma once
#include "Vertex.h"
#include "VertInd.h"
#include <GL\glew.h>
#include <FreeImage.h>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
class Mesh
{
public:
	enum MeshType {
		SingleMesh,
		MultipleMesh,
	}meshType = SingleMesh;

	struct MatLib {
		unsigned int illum;
		glm::vec3 diffuse;
		glm::vec3 ambience;
		glm::vec3 Tf;
		float Ni;
		std::string kdFileName;
		std::string trFileName;
	};
	struct MiniMesh {
		GLuint newVertArr;
		GLuint newVertBuf;
		bool hasTex = false;
		unsigned int texID = 0;
		unsigned int count;
	};

	Mesh();
	Mesh(std::vector<Vertex> &v);
	Mesh(std::vector<Vertex> &v, std::vector<unsigned int> &i);
	Mesh(std::string filename, MeshType m, std::string file);
	~Mesh();

	//void CreateZZZFile();
	//void LoadZZZFile();
	//void SaveZZZFile();

	void CreateBuffer();
	void CreateModelLoadingBuffer(std::vector<glm::vec3> pos, std::vector<glm::vec2> uv, std::vector<glm::vec3> nor);

	GLuint vertArr;
	GLuint vertBuf;

	std::string kindomHeartsLevel;

	std::vector<Vertex> verts;
	std::vector<unsigned int> ind;
	std::vector<VertInd> moreInd;

	std::vector<MiniMesh> myMeshes;
	std::map<std::string, MatLib> matLibs;

	unsigned int count;
private:

};

