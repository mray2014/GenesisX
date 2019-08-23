#pragma once
#include <glm\glm.hpp>
struct Transform
{
	glm::vec3 position;
	glm::vec3 scale;
	glm::vec3 rotation;
	glm::vec3 forward = { 0,0,-1 };
	glm::vec3 up = { 0,1,0 };
	glm::vec3 right = { 1,0,0 };
};