#pragma once
#include "GameEntity.h"

class Player
{
public:
	Player();
	Player(std::string nm, Mesh &oM, Renderer *r);
	~Player();

	bool grounded;
	GameEntity* myPlayer = nullptr;
	void PlayerUpdate();
};

