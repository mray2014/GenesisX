#pragma once
#include <glm\glm.hpp>
struct RidgidBody
{
	glm::vec3 velocity;
	glm::vec3 acceleration;
	glm::vec3 friction;
	float mass;
};