#include "InputManager.h"

std::map<int, bool> keyIsDown;
std::map<int, bool> keyWasDown;
void MouseClick(GLFWwindow* winPtr, int button, int action, int mods)
{
	keyIsDown[button] = action;
}
void KeyCallback(GLFWwindow* win, int key, int scancode, int action, int mods)
{
	keyIsDown[key] = action;
}

InputManager::InputManager()
{
	keyWasDown = keyIsDown;
}

InputManager::InputManager(GLFWwindow &w)
{
	window = &w;
	glfwSetMouseButtonCallback(window, MouseClick);
	glfwSetKeyCallback(window, KeyCallback);
}


InputManager::~InputManager()
{
}

bool InputManager::IsKeyPressed(unsigned int key)
{
	if (keyIsDown[key])
	{
		if (!keyWasDown[key])
		{
			keyWasDown = keyIsDown;
			return true;			
		}
	}
	keyWasDown = keyIsDown;
	return false;
}

bool InputManager::IsKeyDown(unsigned int key)
{
	if (keyIsDown[key])
	{
		return true;
	}
	return false;
}

bool InputManager::IsKeyUp(unsigned int key)
{
	if (keyWasDown[key])
	{
		return true;
	}
	return false;
}

bool InputManager::IsMouseClick(unsigned int key)
{
	if (keyIsDown[key])
	{
		return true;
	}
	return false;
}

void InputManager::Update()
{
	
}
