#include "Player.h"



Player::Player()
{
}

Player::Player(std::string nm, Mesh &oM, Renderer * r)
{
	grounded = false;
	myPlayer = new GameEntity(nm, oM, r);
}


Player::~Player()
{
	if (myPlayer != nullptr) { delete myPlayer; myPlayer = nullptr; }
}

void Player::PlayerUpdate()
{
	if(grounded)
	{
		myPlayer->applyGrav = false;
		myPlayer->ridgidBody.velocity = glm::vec3(myPlayer->ridgidBody.velocity.x, 0 , myPlayer->ridgidBody.velocity.z);
	}
	else
	{
		myPlayer->applyGrav = true;
	}
}
