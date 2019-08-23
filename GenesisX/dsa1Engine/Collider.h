#pragma once
#include <glm\glm.hpp>
struct Collider
{
public:
	enum ColliderType {
		BoxCollider,
		SphereCollider
	}colliderType = BoxCollider;
	glm::vec3 boundingBox;
	glm::vec3 boundingBoxOffset;
	glm::vec3 bbMin;
	glm::vec3 bbMax;
	float radius;
	float skin;
	bool Colliding = false;
};