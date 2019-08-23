#pragma once
#include "InputManager.h"
#include "Renderer.h"
#include"GameEntity.h"
#include "Player.h"
#include "Light.h"
#include "Bullet.h"
#include <map>
class Game
{
public:

	Game(Renderer &r, InputManager &ip);
	~Game();

	//game methods
	void CreateMeshes();
	void Update();
	void RenderLights();
	void UnrenderLights();
	void DeleteBullet(unsigned int i);
	void DestroyBullet(unsigned int i);
	void StickBullet(unsigned int i);
	void BulletLightsOn();
	void BulletLightsOff();
	void GenrateWall(int width, int height, float spacing);
	bool SphereCollision(GameEntity *obj1, GameEntity *obj2);
	bool BoxCollision(GameEntity *obj1, GameEntity *obj2);
	bool SphereBoxCollision(GameEntity *obj1, GameEntity *obj2);

	void CreatePlayer();

	void Shoot();

	InputManager *input;
	Renderer* rend;
	Player* player;
	GameEntity* rayGun;

	bool currentMouseClick;
	bool previousMouseClick;

	glm::vec3 rayGunOffset;

	std::map <std::string, Mesh> MeshDictionary;

	std::vector<GameEntity*> gameobj;
	std::vector<Bullet*> bullets;
	std::vector<Light*> lights;
private:

};

