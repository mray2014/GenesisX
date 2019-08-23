#include "Camera.h"
#include "Player.h"


Camera::Camera()
{
	t0 = 0;
	t = 0;
	dt = 0;
	currentMouseClick = false;
}

Camera::Camera(GLFWwindow &w, InputManager &ip)
{
	window = &w;
	input = &ip;

	t0 = 0;
	t = 0;
	dt = 0;

	sprintSpeed = 25.0f;

	camPos = {0,4,15};
	camRot = {0,0,0};
	foward = {0,0,-1};

	glfwGetWindowSize(window, &width, &height);
	glfwSetCursorPos(window, width / 2, height / 2);

	currentMouseClick = false;

	SetView();
	SetProjection();
}


Camera::~Camera()
{
}

void Camera::SetView()
{
	rotMat = (glm::mat3)glm::yawPitchRoll(camRot.y, camRot.x, camRot.z);
	camCenter = camPos + (rotMat * glm::vec3(0, 0, -1));
	right = rotMat * glm::vec3(1, 0, 0);
	up = rotMat * glm::vec3(0, 1, 0);
	//foward = rotMat * glm::vec3(0, 0, -1);
	foward = glm::cross(up, right);
	//foward = glm::cross(up, glm::vec3(1, 0, 0));

	glm::mat4 myTrans = glm::translate(camPos);
	glm::mat4 myRot = glm::yawPitchRoll(camRot.y, camRot.x, camRot.z);

	worldPos = myTrans * myRot;

	viewMatrix = glm::lookAt(camPos, camCenter, up);
	//foward = glm::normalize(glm::vec3(viewMatrix * glm::vec4(0,0,1,0)));
	//foward = glm::vec3(foward.x, foward.y, foward.z*-1);
	//printf("x: %.3f, y: %.3f, z: %.3f\n", foward.x, foward.y, foward.z);
}

void Camera::SetProjection()
{
	float zoom = 1.0f;
	glfwGetWindowSize(window, &width, &height);

	float fovy = 3.14159f * 0.4f / zoom;
	float aspect = (float)width / (float)height;
	float zNear = 0.01f;
	float zFar = 1000.0f;

	ProjectMatrix = glm::perspective(fovy, aspect, zNear, zFar);
}
void Camera::ResetCamera()
{
	camPos = { 0,0,2 };
	camRot = { 0,0,0 };
	SetView();
	SetProjection();
}

void Camera::UpdatePlayerCam(float dt)
{
	if (input->IsMouseClick(GLFW_MOUSE_BUTTON_1))
	{
		//printf("Mouse CLICKUUUUUU");
		currentMouseClick = true;
	}
	if (currentMouseClick == true && previousMouseClick == false)
	{
		int width, height;
		glfwGetWindowSize(window, &width, &height);
		glfwSetCursorPos(window, width * 0.5f, height * 0.5f);
	}

	if (currentMouseClick)
	{
		float sens = 0.005f;
		double x, y;
		glfwGetCursorPos(window, &x, &y);
		camRot.y -= sens * ((float)x - width * 0.5f);
		camRot.x -= sens * ((float)y - height * 0.5f);
		camRot.x -= glm::clamp(camRot.z, -0.5f*3.145f, 0.5f*3.145f);
		glfwSetCursorPos(window, width * 0.5f, height * 0.5f);
	}
	if (input->IsKeyDown(GLFW_KEY_LEFT_CONTROL) && input->IsKeyDown(GLFW_KEY_C))
	{
		ResetCamera();
	}

	camVel = {};
	glm::mat3 R = (glm::mat3)glm::yawPitchRoll(camRot.y, camRot.x, camRot.z);
	if (input->IsKeyDown(GLFW_KEY_A))
	{
		player->myPlayer->ApplyForce(glm::vec3(right.x * -1.0f, 0, right.z * -1.0f));
		printf("Velocity- x: %f, y: %f, z %f\n", player->myPlayer->ridgidBody.velocity.x, player->myPlayer->ridgidBody.velocity.y, player->myPlayer->ridgidBody.velocity.z);
	}
	if (input->IsKeyDown(GLFW_KEY_D))
	{
		player->myPlayer->ApplyForce(glm::vec3(right.x * 1.0f,0, right.z * 1.0f));
		printf("Velocity- x: %f, y: %f, z %f\n", player->myPlayer->ridgidBody.velocity.x, player->myPlayer->ridgidBody.velocity.y, player->myPlayer->ridgidBody.velocity.z);
	}
	if (input->IsKeyDown(GLFW_KEY_W))
	{
		player->myPlayer->ApplyForce(glm::vec3(foward.x * 1.0f, 0, foward.z * 1.0f));
		printf("Velocity- x: %f, y: %f, z %f\n", player->myPlayer->ridgidBody.velocity.x, player->myPlayer->ridgidBody.velocity.y, player->myPlayer->ridgidBody.velocity.z);
	}
	if (input->IsKeyDown(GLFW_KEY_S))
	{
		player->myPlayer->ApplyForce(glm::vec3(foward.x * -1.0f, 0, foward.z * -1.0f));
		printf("Velocity- x: %f, y: %f, z %f\n", player->myPlayer->ridgidBody.velocity.x, player->myPlayer->ridgidBody.velocity.y, player->myPlayer->ridgidBody.velocity.z);
	}
	if (input->IsKeyDown(GLFW_KEY_SPACE))
	{
		player->grounded = false;
		player->myPlayer->ApplyForce(glm::vec3(0.0f, 10.0f, 0.0f));
	}



	/*float speed = 20.0f;
	if (camVel != glm::vec3())
	{
		if (input->IsKeyDown(GLFW_KEY_LEFT_SHIFT))
		{
			camVel = glm::normalize(camVel) * sprintSpeed;
		}
		else
		{
			camVel = glm::normalize(camVel) * speed;
		}
	}*/

	camPos = player->myPlayer->transform.position;
	previousMouseClick = currentMouseClick;
	currentMouseClick = false;
	player->myPlayer->transform.forward = glm::vec3(foward.x, 0, foward.z );
	player->myPlayer->transform.right = glm::vec3(right.x, 0, right.z);
	player->myPlayer->Update();
	player->PlayerUpdate();
}

void Camera::UpdateCam(float dt)
{
	if (input->IsMouseClick(GLFW_MOUSE_BUTTON_1))
	{
		//printf("Mouse CLICKUUUUUU");
		currentMouseClick = true;
	}
	if (currentMouseClick == true && previousMouseClick == false)
	{
		int width, height;
		glfwGetWindowSize(window, &width, &height);
		glfwSetCursorPos(window, width * 0.5f, height * 0.5f);
	}

	if(currentMouseClick)
	{
		float sens = 0.005f;
		double x, y;
		glfwGetCursorPos(window, &x, &y);
		camRot.y -= sens * ((float)x - width * 0.5f);
		camRot.x -= sens * ((float)y - height * 0.5f);
		camRot.x -= glm::clamp(camRot.z, -0.5f*3.145f, 0.5f*3.145f);
		glfwSetCursorPos(window, width * 0.5f, height * 0.5f);
	}
	if (input->IsKeyDown(GLFW_KEY_LEFT_CONTROL) && input->IsKeyDown(GLFW_KEY_C))
	{
		ResetCamera();
	}

	camVel = {};
	glm::mat3 R = (glm::mat3)glm::yawPitchRoll(camRot.y, camRot.x, camRot.z);
	if (input->IsKeyDown(GLFW_KEY_A))
	{
		camVel += R*glm::vec3(-0.001, 0, 0);
	}
	if (input->IsKeyDown(GLFW_KEY_D))
	{
		camVel += R*glm::vec3(0.001, 0, 0);
	}
	if (input->IsKeyDown(GLFW_KEY_W))
	{
		camVel += R*glm::vec3(0, 0, -0.001);
	}
	if (input->IsKeyDown(GLFW_KEY_S))
	{
		camVel += R*glm::vec3(0, 0, 0.001);
	}
	if (input->IsKeyDown(GLFW_KEY_SPACE))
	{
		camVel += R*glm::vec3(0, 0.001, 0);
	}
	if (input->IsKeyDown(GLFW_KEY_X))
	{
		camVel += R*glm::vec3(0, -0.001, 0);
	}



	float speed = 20.0f;
	if(camVel != glm::vec3())
	{
		if (input->IsKeyDown(GLFW_KEY_LEFT_SHIFT))
		{
			camVel = glm::normalize(camVel) * sprintSpeed;
		}
		else
		{
			camVel = glm::normalize(camVel) * speed;
		}
	}

	camPos += camVel * dt;
	previousMouseClick = currentMouseClick;
	currentMouseClick = false;
}

void Camera::SetPlayer(Player &p)
{
	player = &p;
}

