#include "Renderer.h"
#include "GameEntity.h"
#include "Camera.h"
#include "Light.h"
#include <src\SOIL.h>


Renderer::Renderer(Camera *c, ShaderManager& man, int *w, int *h)
{
	cam = c;
	shaderM = &man;
	wid = w;
	hei = h;

	lights.reserve(20);
	gameObjs.reserve(20);

	//glClearColor(0.392f, 0.584f, 0.929f, 1.0f);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	currentSky = 1;

	SkyBoxModel = new GameEntity("skybox", "models/box.obj", Mesh::SingleMesh,"",this);
	SkyBoxModel->SetTag("SkyBox");
	RemoveFromRenderer(SkyBoxModel->rendID);
	LoadAllSkyBoxes();


	rendType = Deferred;

	Skybox = loadedSkyBoxes[currentSky];
	g_Buffer = CreateGeometryBuffer();
	CreateQuad();
}	


Renderer::~Renderer()
{
	if (SkyBoxModel->objMesh != nullptr) { delete SkyBoxModel->objMesh; SkyBoxModel->objMesh = nullptr; }
	if (SkyBoxModel != nullptr) { delete SkyBoxModel; SkyBoxModel = nullptr; }
}
void Renderer::Update() 
{
	//binding default framebuffer and clearing info
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//binding g_Buffer framebuffer and clearing info
	glBindFramebuffer(GL_FRAMEBUFFER, g_Buffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		if (rendType == Deferred)
		{
			//Geometry Pass
			SetUpGeometry();
			for (unsigned int i = 0; i < gameObjs.size(); i++)
			{
				GeometryPass(*gameObjs[i]);
			}
			ResetGeometry();

			//Setting drawing to this render target
			glDrawBuffer(GL_COLOR_ATTACHMENT4);
			glClear(GL_COLOR_BUFFER_BIT);

			//Ambient Pass
			SetUpAmbient();
			AmbientPass();
			ResetAmbient();
			
			//glClear(GL_COLOR_BUFFER_BIT);
			//Lighting Pass
			SetUpLighting();
			LightingPass();
			ResetLighting();

		}
		if (rendType == Forward)
		{
			FowardLightingSetUp();
			for (unsigned int i = 0; i < gameObjs.size(); i++)
			{
				FowardPass(gameObjs[i]);
			}
		}
		//drawing sky box
		DrawSkyBox();

		glDrawBuffer(GL_COLOR_ATTACHMENT5);
		SetUpBrightness();
		BrightnessPass();
		ResetBrightness();

		SetUpBlur();
		BlurPass();
		ResetBlur();
		
		glDrawBuffer(GL_COLOR_ATTACHMENT3);
		//glClear(GL_COLOR_BUFFER_BIT);
		SetUpHDR();
		HDRPass();
		ResetHDR();
		//
		//DrawSkyBox();



		//copying info from g_Buffer to default buffer
		BlitInfo();

		//binding default framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);


	}


unsigned int Renderer::AddToRenderer(GameEntity &obj)
{
	unsigned int pos = gameObjs.size();
	gameObjs.push_back(&obj);

	return pos;
}

void Renderer::RemoveFromRenderer(unsigned int pos)
{
	gameObjs.erase(gameObjs.begin() + pos);
	for(unsigned int i = 0; i < gameObjs.size(); i++)
	{
		(gameObjs)[i]->rendID = i;
	}
}

unsigned int Renderer::AddToLights(Light &lig)
{
	unsigned int pos = lights.size();
	lights.push_back(&lig);

	return pos;
}

void Renderer::RemoveFromLights(unsigned int pos)
{
	lights.erase(lights.begin() + pos);
	for (unsigned int i = 0; i < lights.size(); i++)
	{
		(lights)[i]->lightID = i;
	}
}
void Renderer::FowardPass(GameEntity *obj)
{
	// rendering game object
	float time = Engine::time.t;

	glUniformMatrix4fv(0, 1, GL_FALSE, &obj->worldPos[0][0]);
	glUniformMatrix4fv(1, 1, GL_FALSE, &cam->viewMatrix[0][0]);
	glUniformMatrix4fv(2, 1, GL_FALSE, &cam->ProjectMatrix[0][0]);
	glUniform1f(7, time);
	glUniform3f(8, cam->camPos.x, cam->camPos.y, cam->camPos.z);
	glUniform1i(10, obj->hasTex);
	//glUniform4f(13, gameObjs[i]->objMesh.specular.x, gameObjs[i]->objMesh.specular.y, gameObjs[i]->objMesh.specular.z, gameObjs[i]->objMesh.specular.w);
	if (obj->hasTex)
	{
		glBindTexture(GL_TEXTURE_2D, obj->GetTexId());
	}
	if (obj->GetTag() == "Light")
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	glBindVertexArray(obj->GetVertArr());
	glDrawArrays(GL_TRIANGLES, 0, obj->GetCount());
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	if (obj->GetTag() == "Light")
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}
#pragma region SkyBox Methods

void Renderer::ChangeSkyBox()
{
	currentSky++;
	if (currentSky >= loadedSkyBoxes.size()) 
	{
		currentSky = 0;
	}
	Skybox = loadedSkyBoxes[currentSky];

}

GLuint Renderer::loadCubeMap(std::vector<const GLchar*> faces)
{
	GLuint textureID;
	
	glGenTextures(1, &textureID);
	glActiveTexture(GL_TEXTURE0);
	

	int width, height;
	unsigned char* image;

	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	for (GLuint i = 0; i < faces.size(); i++) {
		image = SOIL_load_image(faces[i],&width,&height,0,SOIL_LOAD_RGB);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
		GL_RGB, width, height, 0 , GL_RGB, GL_UNSIGNED_BYTE, image);
		SOIL_free_image_data(image);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	return textureID;
}

void Renderer::LoadAllSkyBoxes()
{
	std::vector<const GLchar*> faces;
	faces.push_back("SkyBox/sunset/right.jpg");
	faces.push_back("SkyBox/sunset/left.jpg");
	faces.push_back("SkyBox/sunset/top.jpg");
	faces.push_back("SkyBox/sunset/bottom.jpg");
	faces.push_back("SkyBox/sunset/back.jpg");
	faces.push_back("SkyBox/sunset/front.jpg");
	loadedSkyBoxes.push_back(loadCubeMap(faces));
	faces.clear();

	faces.push_back("SkyBox/space1/right.jpg");
	faces.push_back("SkyBox/space1/left.jpg");
	faces.push_back("SkyBox/space1/top.jpg");
	faces.push_back("SkyBox/space1/bottom.jpg");
	faces.push_back("SkyBox/space1/back.jpg");
	faces.push_back("SkyBox/space1/front.jpg");
	loadedSkyBoxes.push_back(loadCubeMap(faces));
	faces.clear();

	faces.push_back("SkyBox/lake/right.jpg");
	faces.push_back("SkyBox/lake/left.jpg");
	faces.push_back("SkyBox/lake/top.jpg");
	faces.push_back("SkyBox/lake/bottom.jpg");
	faces.push_back("SkyBox/lake/back.jpg");
	faces.push_back("SkyBox/lake/front.jpg");
	loadedSkyBoxes.push_back(loadCubeMap(faces));
	faces.clear();

	faces.push_back("SkyBox/winter/right.jpg");
	faces.push_back("SkyBox/winter/left.jpg");
	faces.push_back("SkyBox/winter/top.jpg");
	faces.push_back("SkyBox/winter/bottom.jpg");
	faces.push_back("SkyBox/winter/back.jpg");
	faces.push_back("SkyBox/winter/front.jpg");
	loadedSkyBoxes.push_back(loadCubeMap(faces));
	faces.clear();

	faces.push_back("SkyBox/lava/right.jpg");
	faces.push_back("SkyBox/lava/left.jpg");
	faces.push_back("SkyBox/lava/top.jpg");
	faces.push_back("SkyBox/lava/bottom.jpg");
	faces.push_back("SkyBox/lava/back.jpg");
	faces.push_back("SkyBox/lava/front.jpg");
	loadedSkyBoxes.push_back(loadCubeMap(faces));
	faces.clear();

	faces.push_back("SkyBox/default/right.jpg");
	faces.push_back("SkyBox/default/left.jpg");
	faces.push_back("SkyBox/default/top.jpg");
	faces.push_back("SkyBox/default/bottom.jpg");
	faces.push_back("SkyBox/default/back.jpg");
	faces.push_back("SkyBox/default/front.jpg");
	loadedSkyBoxes.push_back(loadCubeMap(faces));
	faces.clear();
}

void Renderer::DrawSkyBox()
{
	glUseProgram(shaderM->skyProgram);
	glDepthMask(GL_FALSE);
	glDisable(GL_CULL_FACE);
	glm::mat4 view = glm::mat4(glm::mat3(cam->viewMatrix));
	glUniformMatrix4fv(1, 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(2, 1, GL_FALSE, &cam->ProjectMatrix[0][0]);
	glBindVertexArray(SkyBoxModel->GetVertArr());
	glBindTexture(GL_TEXTURE_CUBE_MAP, Skybox);
	glDepthFunc(GL_LEQUAL);

	glDrawArrays(GL_TRIANGLES, 0, SkyBoxModel->GetCount());
	glBindVertexArray(0);
	glDepthMask(GL_TRUE);
	glUseProgram(shaderM->GetProgram());
	glEnable(GL_CULL_FACE);
	glDepthFunc(GL_LESS);
}

#pragma endregion

#pragma region Deferred Rendering Methods

GLuint Renderer::CreateGeometryBuffer()
{
	// getting the windows width and height
	int width = *wid;
	int height = *hei;
	GLuint piz = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	GLuint za = GL_FRAMEBUFFER_COMPLETE;

	// create a new render target to render to
	GLuint newFrameBuffer;
	glGenFramebuffers(1, &newFrameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, newFrameBuffer);


	// create a new texture to render my color to
	glGenTextures(1, &colText);
	glBindTexture(GL_TEXTURE_2D, colText);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	// create a new texture to render my normals to
	glGenTextures(1, &normText);
	glBindTexture(GL_TEXTURE_2D, normText);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	// create a new texture to render my world positions to
	glGenTextures(1, &worldText);
	glBindTexture(GL_TEXTURE_2D, worldText);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	// create a depth buffer
	glGenRenderbuffers(1, &depthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);

	// create a stencil buffer
	glGenTextures(1, &stenText);
	glBindTexture(GL_TEXTURE_2D, stenText);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH32F_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_FLOAT_32_UNSIGNED_INT_24_8_REV, NULL);

	//// create a new texture to render my final texture
	glGenTextures(1, &finText);
	glBindTexture(GL_TEXTURE_2D, finText);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGB, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	//// create a new texture to render my final texture
	glGenTextures(1, &HDRText);
	glBindTexture(GL_TEXTURE_2D, HDRText);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glGenTextures(1, &BrightnessText);
	glBindTexture(GL_TEXTURE_2D, BrightnessText);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glGenFramebuffers(2, BloomFBO);
	glGenTextures(2, BloomText);
	for (int i = 0; i < 2; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, BloomFBO[i]);
		glBindTexture(GL_TEXTURE_2D, BloomText[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, BloomText[i], 0);

		piz = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (piz != za) { return false; }
	}
	glBindFramebuffer(GL_FRAMEBUFFER, newFrameBuffer);
	// attaching buffers to render target
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, colText, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, normText, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, worldText, 0);
	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, stenText, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, finText, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, HDRText, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT5, BrightnessText, 0);

	GLenum DrawBuffer[3] = { GL_COLOR_ATTACHMENT0,GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(3, DrawBuffer);
	piz = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (piz != za) { return false; }

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	return newFrameBuffer;
}
void Renderer::CreateQuad()
{

	GLfloat quadVertices[] = {
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		1.0f, -1.0f,  1.0f, 0.0f,
		1.0f,  1.0f,  1.0f, 1.0f
	};

	GLuint vboQuad;
	glGenBuffers(1, &vboQuad);

	glGenVertexArrays(1, &vaoQuad);

	glBindBuffer(GL_ARRAY_BUFFER, vboQuad);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

	glBindVertexArray(vaoQuad);
	glBindBuffer(GL_ARRAY_BUFFER, vboQuad);

	//GLint posAttrib = glGetAttribLocation(shaderM->lightProgram, "position");
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);

	//GLint texAttrib = glGetAttribLocation(shaderM->lightProgram, "texcoord");
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));

	glBindVertexArray(0);
}
void Renderer::SetUpGeometry()
{
	glUseProgram(shaderM->geoProgram);
	//drawing to 3 textures
	GLenum DrawBuffer[3] = { GL_COLOR_ATTACHMENT0,GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(3, DrawBuffer);

	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glDepthMask(GL_TRUE);
}
void Renderer::GeometryPass(GameEntity &obj)
{
	float time = Engine::time.t;

	glUniformMatrix4fv(7, 1, GL_FALSE, &(&obj)->worldPos[0][0]);
	glUniformMatrix4fv(8, 1, GL_FALSE, &cam->viewMatrix[0][0]);
	glUniformMatrix4fv(9, 1, GL_FALSE, &cam->ProjectMatrix[0][0]);
	glUniform1f(10, time);
	// rendering game object
	if ((obj).GetTag() == "MultiMesh") 
	{
		//glDisable(GL_CULL_FACE);
		//glDepthFunc(GL_ALWAYS);
		//glFrontFace(GL_CCW);

		for(unsigned int i = 0; i < (&obj)->objMesh->myMeshes.size(); i++)
		{
			glUniform4f(6, (&obj)->color.x, (&obj)->color.y, (&obj)->color.z, (&obj)->color.w);
			glUniform1i(11, (&obj)->objMesh->myMeshes[i].hasTex);
			if ((&obj)->objMesh->myMeshes[i].hasTex)
			{
				glBindTexture(GL_TEXTURE_2D, (&obj)->objMesh->myMeshes[i].texID);
			}
			glBindVertexArray((&obj)->objMesh->myMeshes[i].newVertArr);
			glDrawArrays(GL_TRIANGLES, 0, ((&obj)->objMesh->myMeshes[i].count));
			glBindVertexArray(0);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
		//glFrontFace(GL_CW);
		//glEnable(GL_CULL_FACE);
		//glDepthFunc(GL_LESS);
	}
	else {
		glUniform4f(6, (&obj)->color.x, (&obj)->color.y, (&obj)->color.z, (&obj)->color.w);
		glUniform1i(11, (&obj)->hasTex);
		//glUniform4f(13, gameObjs[i]->objMesh.specular.x, gameObjs[i]->objMesh.specular.y, gameObjs[i]->objMesh.specular.z, gameObjs[i]->objMesh.specular.w);
		if ((&obj)->hasTex)
		{
			glBindTexture(GL_TEXTURE_2D, (&obj)->GetTexId());
		}
		if ((&obj)->GetTag() == "Light")
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glDisable(GL_CULL_FACE);
			//glCullFace(GL_FRONT_AND_BACK);
		}
		glBindVertexArray((&obj)->GetVertArr());
		glDrawArrays(GL_TRIANGLES, 0, (&obj)->GetCount());
		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);
		if ((&obj)->GetTag() == "Light")
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glEnable(GL_CULL_FACE);
			//glCullFace(GL_BACK);
		}
	}

}
void Renderer::ResetGeometry()
{
	glDepthMask(GL_FALSE);
	glDisable(GL_DEPTH_TEST);
}
void Renderer::SetUpAmbient()
{
	glUseProgram(shaderM->ambProgram);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, colText);
	glUniform1i(2, 0);
}
void Renderer::AmbientPass()
{
	//glUniform4f(0, mainCam->camPos.x, mainCam->camPos.y, mainCam->camPos.z, 1.0f);
	glBindVertexArray(vaoQuad);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
	//rend.LightingPass();	
}
void Renderer::ResetAmbient()
{
	glEnable(GL_DEPTH_TEST);
}
void Renderer::SetUpStencil()
{
	glUseProgram(shaderM->nulProgram);
	glDrawBuffer(GL_NONE);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glClear(GL_STENCIL_BUFFER_BIT);
	glStencilFunc(GL_ALWAYS, 0, 0);
	glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
	glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);
}
void Renderer::StencilePass()
{

}
void Renderer::ResetStencil()
{
}
void Renderer::SetUpLighting()
{
	glUseProgram(shaderM->ligProgram);
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_ONE, GL_ONE);
	glDepthMask(GL_FALSE);
	glDepthFunc(GL_ALWAYS);
}
void Renderer::LightingPass()
{
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, normText);
	glUniform1i(16, 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, worldText);
	glUniform1i(17, 2);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, colText);
	glUniform1i(15, 0);

	DeferredLightingSetUp();
}
void Renderer::ResetLighting()
{
	glDepthMask(GL_TRUE);
	glEnable(GL_CULL_FACE);
	glDisable(GL_BLEND);
	glCullFace(GL_BACK);
}
void Renderer::SetUpHDR()
{
	glDepthMask(GL_FALSE);
	glDisable(GL_DEPTH_TEST);

	glUseProgram(shaderM->HDRProgram);

	glActiveTexture(GL_TEXTURE1);
	////glBindTexture(GL_TEXTURE_2D, HDRText);
	glBindTexture(GL_TEXTURE_2D, BloomText[0]);
	glUniform1i(4, 1);

	glActiveTexture(GL_TEXTURE0);
	////glBindTexture(GL_TEXTURE_2D, HDRText);
	glBindTexture(GL_TEXTURE_2D, HDRText);
	glUniform1i(3, 0);

	glUniform1f(2, 0.01f);
}
void Renderer::HDRPass()
{
	glBindVertexArray(vaoQuad);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}
void Renderer::ResetHDR()
{
	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
}
void Renderer::SetUpBrightness()
{
	glDepthMask(GL_FALSE);
	glDisable(GL_DEPTH_TEST);

	glUseProgram(shaderM->BriProgram);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, HDRText);
	glUniform1i(2, 0);
}
void Renderer::BrightnessPass()
{
	glBindVertexArray(vaoQuad);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}
void Renderer::ResetBrightness()
{
	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
}
void Renderer::SetUpBlur()
{
	glDepthMask(GL_FALSE);
	glDisable(GL_DEPTH_TEST);

	glUseProgram(shaderM->BluProgram);
}
void Renderer::BlurPass()
{
	bool horizontal = true;
	bool first_Iteration = true;
	int amount = 20;
	//glDrawBuffer(GL_COLOR_ATTACHMENT0);
	for (int i = 0; i < amount; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, BloomFBO[horizontal]);
		glUniform1f(3, horizontal);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, first_Iteration ? BrightnessText : BloomText[!horizontal]);

		glBindVertexArray(vaoQuad);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);

		horizontal = !horizontal;
		//horizontal ? printf("TRUE\n") : printf("FALSE\n");
		if (first_Iteration) {
			first_Iteration = false;
		}
	}
	glBindFramebuffer(GL_FRAMEBUFFER, g_Buffer);
}
void Renderer::ResetBlur()
{
	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
}

void Renderer::BlitInfo()
{
	int width = *wid;
	int height = *hei;

	glBindFramebuffer(GL_READ_FRAMEBUFFER, g_Buffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	glReadBuffer(GL_COLOR_ATTACHMENT3);
	glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_LINEAR);
}

#pragma endregion

#pragma region Lighting Setup Methods

void Renderer::DeferredLightingSetUp()
{
	int width = *wid;
	int height = *hei;
	for (unsigned int i = 0; i < lights.size(); i++) {
		glUniformMatrix4fv(0, 1, GL_FALSE, &lights[i]->sphere->worldPos[0][0]);
		glUniformMatrix4fv(1, 1, GL_FALSE, &cam->ProjectMatrix[0][0]);
		glUniformMatrix4fv(2, 1, GL_FALSE, &cam->viewMatrix[0][0]);

		glUniform1f(7, (float)(width));
		glUniform1f(8, (float)(height));
		glUniform1f(9, lights[i]->myLight.lightRadius);
		glUniform1f(10, lights[i]->myLight.linear);
		glUniform1f(11, lights[i]->myLight.quadratic);
		glUniform3f(12, lights[i]->myLight.lightPos.x, lights[i]->myLight.lightPos.y, lights[i]->myLight.lightPos.z);
		glUniform3f(13, lights[i]->myLight.color.x, lights[i]->myLight.color.y, lights[i]->myLight.color.z);
		glUniform3f(14, cam->camPos.x, cam->camPos.y, cam->camPos.z);

		glBindVertexArray(lights[i]->sphere->GetVertArr());
		glm::vec3 vecDist = lights[i]->myLight.lightPos - cam->camPos;
		float dist = glm::length(vecDist);
		//printf("\n%f", dist);
		if (dist <= lights[i]->myLight.lightRadius)
		{
			//glPolygonMode(GL_FRONT, GL_FILL);
			glCullFace(GL_FRONT);
		}
		glDrawArrays(GL_TRIANGLES, 0, lights[i]->sphere->GetCount());
		glBindVertexArray(0);
		//glEnable(GL_DEPTH_TEST);
	}
}

void Renderer::FowardLightingSetUp()
{
	glUniform3f(9, lights[0]->myLight.lightPos.x, lights[0]->myLight.lightPos.y, lights[0]->myLight.lightPos.z);
}

#pragma endregion