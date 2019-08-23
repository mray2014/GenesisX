#pragma once

#include "Mesh.h"
#include "Object.h"
#include "Transform.h"
#include "RidgidBody.h"
#include "Collider.h"
#include "Engine.h"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include "Renderer.h"


class alignas(16) GameEntity
	:Object
{
public:
	GameEntity* parent = nullptr;
	GameEntity() = default;
	GameEntity(Renderer *r);
	GameEntity(std::string nm, std::vector<Vertex> &v, std::vector<unsigned int> &i, Renderer *r);
	GameEntity(std::string nm, std::string filename, Mesh::MeshType t, std::string file, Renderer* r);
	GameEntity(std::string nm, Mesh &oM, Renderer *r);
	~GameEntity() override;

	void Update() override;

	void SetWorldPos();
	void ResetGameEntity();

	GLuint GetVertArr();
	unsigned int GetCount();

	void SetTag(std::string tagName);
	std::string GetTag();

	glm::mat4 worldPos;
	
	std::string name;
	unsigned int rendID;
	Mesh* objMesh;

	void Translate(glm::vec3 transVec);
	void Translate(float x, float y, float z);
	void Scale(glm::vec3 scaleVec);
	void Scale(float x, float y, float z);
	void Scale(float c);
	void Rotate(glm::vec3 RotVec);
	void Rotate(float x, float y, float z);

	void ApplyForce(glm::vec3 force);
	void ApplyGravity();
	void CalculateFric();
	void UpdateVelocity();
	void FindRadius();

	void SetDirection();

	bool applyGrav = false;
	bool applyFric = false;
	float maxSpeed;
	float fricStrength;
	glm::vec3 gravity;

	glm::vec4 color;
	glm::vec4 specular;

	Transform transform;
	RidgidBody ridgidBody;
	Collider collider;


	void LoadTexture(char* filename);
	GLuint GetTexId();

	bool hasTex = false;
	unsigned int texID = 0;


private:
	glm::vec3 PREV_FORWARD= { 0,0,1 };
	Renderer* rend;
};

