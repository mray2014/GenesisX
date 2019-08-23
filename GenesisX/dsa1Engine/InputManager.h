#pragma once
#include <map>
#include <GL\glew.h>
#include <GLFW\glfw3.h>

class InputManager
{
public:
	InputManager();
	InputManager(GLFWwindow &w);
	~InputManager();

	bool IsKeyPressed(unsigned int key);
	bool IsKeyDown(unsigned int key);
	bool IsKeyUp(unsigned int key);
	bool IsMouseClick(unsigned int key);

	void Update();

	GLFWwindow* window;


private:

};

