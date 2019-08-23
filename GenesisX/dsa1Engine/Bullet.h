#pragma once
#include "GameEntity.h"
#include "Light.h"
#include "Camera.h"
#include "Renderer.h"


class Bullet
{
public:
	Bullet();
	Bullet(float s, std::string file, Renderer* r);
	Bullet(float s, Mesh &myMesh, Renderer* r);
	~Bullet();

	void Update();

	unsigned int bulletID = 0;
	float speed;
	float bulletLifeSpan;
	float bulletTime;
	bool destroyBullet = false;
	GameEntity* bulletModel = nullptr;
	Light* bulletLight = nullptr;

};

