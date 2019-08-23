#pragma once
#include "InputManager.h"
#include "Renderer.h"
#include <glm\glm.hpp>
#include <GLFW\glfw3.h>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/euler_angles.hpp>

class Player;

class alignas(16) Camera
{
public:
	Camera();
	Camera(GLFWwindow &window, InputManager &ip);
	~Camera();

	//camera variables
	glm::vec3 camPos;
	glm::vec3 camRot;
	glm::vec3 camCenter;
	glm::vec3 camDir;
	glm::vec3 camVel;
	glm::vec3 foward;
	glm::vec3 up;
	glm::vec3 right;
	glm::mat3 rotMat;

	//camera matrixs
	glm::mat4 worldPos;
	glm::mat4 viewMatrix;
	glm::mat4 ProjectMatrix;

	//window|input variables
	InputManager *input;
	GLFWwindow* window;

	bool currentMouseClick;
	bool previousMouseClick;

	int width;
	int height;

	//camera's sprint speed
	float sprintSpeed;

	//camera methods
	void SetView();
	void SetProjection();
	void ResetCamera();
	void UpdatePlayerCam(float dt);
	void UpdateCam(float dt);
	
	void SetPlayer(Player &p);

	Player* player = nullptr;

private:
	//don't think these do anything
	float t0;
	float t;
	float dt;
};

