#include "Engine.h"
#include <GL/GLU.h>
#include "Game.h"

Engine::Engine()
{
	shaderM = new ShaderManager();
}


Engine::~Engine()
{
	if (mainCam != nullptr) { delete mainCam; mainCam = nullptr; }
	//if (input != nullptr) { delete bulletModel;  bulletModel = nullptr; }
	if (shaderM != nullptr) { delete shaderM;  shaderM = nullptr; }
	if (newRend!= nullptr) { delete newRend;  newRend = nullptr; }
	if (myGame != nullptr) { delete myGame;  myGame = nullptr; }
}
bool Engine::Init()
{
	if (glfwInit() == GL_FALSE) return false;

	GLFWwindowPtr = glfwCreateWindow(800, 600, "Michael Ray DSA1 Engine", NULL, NULL);
	if (GLFWwindowPtr != nullptr)glfwMakeContextCurrent(GLFWwindowPtr);
	else { glfwTerminate(); return false; }

	if (glewInit() != GLEW_OK) { glfwTerminate(); return false; }

	input = InputManager(*GLFWwindowPtr);
	
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	//Engine::LoadMesh("DestinyIsland", "models/DestinyIsland/level/di00_01.obj", Mesh::MultipleMesh, "models/DestinyIsland/level/");
	//Engine::LoadMesh("TraverseTown", "models/TraverseTown/level/tw00_01.obj", Mesh::MultipleMesh, "models/TraverseTown/level/");
	Engine::LoadMesh("RayGun", "models/raygun.obj", Mesh::SingleMesh, "");
	//Engine::LoadMesh("HaloSword", "models/HaloSword.obj", Mesh::SingleMesh, "");
	//Engine::LoadMesh("Teapot", "models/teapot.obj", Mesh::SingleMesh, "");
	Engine::LoadMesh("Sphere", "models/sphere.obj", Mesh::SingleMesh, "");
	Engine::LoadMesh("Box", "models/box.obj", Mesh::SingleMesh, "");
	Engine::LoadMesh("Plane", "models/plane.obj", Mesh::SingleMesh, "");

	mainCam = new Camera(*GLFWwindowPtr, input);

	return true;
}
bool Engine::bufferModel()
{

	return true;
}
bool Engine::gameLoop()
{
	glfwGetWindowSize(GLFWwindowPtr, &width, &height);
	newRend = new Renderer(mainCam, *shaderM, &width, &height);
	myGame = new Game(*newRend, input);
	mainCam->SetPlayer(*myGame->player);
	//myGame->SetPlayer();

	while (!glfwWindowShouldClose(GLFWwindowPtr))
	{
		if (input.IsKeyDown(GLFW_KEY_ESCAPE))
		{
			glfwSetWindowShouldClose(GLFWwindowPtr, GL_TRUE);
		}
		//updating camera
		mainCam->SetProjection();
		mainCam->SetView();
		//mainCam->UpdateCam(time.dt);
		mainCam->UpdatePlayerCam(time.dt);

		//updating models
		myGame->Update();

		//rendering models
		newRend->Update();

		//updating title
		std::string titleName = "Michael Ray DSA1 Engine Fps: ";
		Engine::Update(GLFWwindowPtr, titleName);

		// swap chain
		glfwSwapBuffers(GLFWwindowPtr);

		// Checking Input	
		glfwPollEvents();
	}
	glfwTerminate();
	return true;
}
bool Engine::useShader()
{
	if(!shaderM->LoadShaders("shaders/RegularShading/vShader.glsl","shaders/RegularShading/fShader.glsl", shaderM->regProgram))
	{
		return false;
	}
	if (!shaderM->LoadShaders("shaders/SkyBoxShading/skyBoxVShader.glsl", "shaders/SkyBoxShading/skyBoxPShader.glsl", shaderM->skyProgram))
	{
		return false;
	}
	if (!shaderM->LoadShaders("shaders/DeferredShading/geometryVShader.glsl", "shaders/DeferredShading/geometryPShader.glsl", shaderM->geoProgram))
	{
		return false;
	}
	if (!shaderM->LoadShaders("shaders/DeferredShading/nullV.glsl", "shaders/DeferredShading/nullP.glsl", shaderM->nulProgram))
	{
		return false;
	}
	if (!shaderM->LoadShaders("shaders/DeferredShading/ambientVShader.glsl", "shaders/DeferredShading/ambientPShader.glsl", shaderM->ambProgram))
	{
		return false;
	}
	if (!shaderM->LoadShaders("shaders/DeferredShading/lightVShader.glsl", "shaders/DeferredShading/lightPShader.glsl", shaderM->ligProgram))
	{
		return false;
	}
	if (!shaderM->LoadShaders("shaders/HDRShading/HDRVShader.glsl", "shaders/HDRShading/HDRPShader.glsl", shaderM->HDRProgram))
	{
		return false;
	}
	if (!shaderM->LoadShaders("shaders/BloomShading/BrightnessVShader.glsl", "shaders/BloomShading/BrightnessPShader.glsl", shaderM->BriProgram))
	{
		return false;
	}
	if (!shaderM->LoadShaders("shaders/BloomShading/BlurVShader.glsl", "shaders/BloomShading/BlurPShader.glsl", shaderM->BluProgram))
	{
		return false;
	}
	glUseProgram(shaderM->GetProgram());
	return true;
}
void Engine::Start()
{
	std::srand(std::time(0));
}

void Engine::Stop()
{
	
}

void Engine::Update(GLFWwindow* window, std::string title)
{
	time.Update();
}
float Engine::Random()
{
	return (float)rand();
}
bool Engine::LoadMesh(std::string meshName, std::string fileName, Mesh::MeshType mT, std::string filePath)
{
	MeshDictionary.insert(std::pair<std::string, std::shared_ptr<Mesh>>(meshName, std::make_shared<Mesh>(fileName, mT, filePath)));
	return true;
}
Mesh* Engine::GetMesh(std::string name)
{
	std::shared_ptr<Mesh> newMesh = MeshDictionary.at(name);
	return newMesh.get();
}
Timer Engine::time;
std::map <std::string, std::shared_ptr<Mesh>> Engine::MeshDictionary;

