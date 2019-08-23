#include "Game.h"

Game::Game(Renderer &r, InputManager &ip)
{
	rend = &r;
	input = &ip;
	bullets.reserve(20);
	gameobj.reserve(10);
	lights.reserve(20);
	CreateMeshes();
	lights.push_back(new Light("Light1", 1.0f, glm::vec3(0, 1, 0), glm::vec3(0, 0, 0), rend));
	lights.push_back(new Light("Light2", 1.0f, glm::vec3(0, 5, 0), glm::vec3(0, 0, 0), rend));
	lights.push_back(new Light("Light3", 1.0f, glm::vec3(10, 1, 0), glm::vec3(0, 0, 0), rend));
	lights.push_back(new Light("Light4", 1.0f, glm::vec3(10, 5, 0), glm::vec3(0, 0, 0), rend));
	
	lights[0]->myLight.color = glm::vec4(glm::normalize(glm::vec3(Engine::Random(), Engine::Random(), Engine::Random())), 1);
	lights[2]->myLight.color = glm::vec4(glm::normalize(glm::vec3(Engine::Random(), Engine::Random(), Engine::Random())), 1);
	lights[3]->myLight.color = glm::vec4(glm::normalize(glm::vec3(Engine::Random(), Engine::Random(), Engine::Random())), 1);
	lights[1]->myLight.color = glm::vec4(glm::normalize(glm::vec3(Engine::Random(), Engine::Random(), Engine::Random())), 1);
	
	UnrenderLights();
	CreatePlayer();

	//rend->RemoveFromRenderer(light1->sphere.rendID);
}


Game::~Game()
{
	for(unsigned int i = 0; i < gameobj.size();i++)
	{
		if (gameobj[i] != nullptr) { delete gameobj[i]; gameobj[i] = nullptr; }
	}
	for (unsigned int i = 0; i < lights.size(); i++)
	{
		if (lights[i] != nullptr) { delete lights[i]; lights[i] = nullptr; }
	}
	for (unsigned int i = 0; i < bullets.size(); i++)
	{
		if (bullets[i]->bulletModel != nullptr) { delete bullets[i]->bulletModel; bullets[i]->bulletModel = nullptr; }
		if (bullets[i]->bulletLight != nullptr) { delete bullets[i]->bulletLight; bullets[i]->bulletLight = nullptr; }
		if (bullets[i] != nullptr) { delete bullets[i]; bullets[i] = nullptr; }
	}
	if (player != nullptr) { delete player; player = nullptr; }
	if (rayGun != nullptr) { delete rayGun; rayGun = nullptr; }
}

void Game::CreateMeshes()
{
	// making a game obj
	//gameobj.push_back(new GameEntity("too much pizza", "models/DestinyIsland/level/di00_01.obj", Mesh::MultipleMesh, rend));
	//gameobj.push_back(new GameEntity("too much pizza", "models/TraverseTown/level/tw00_01.obj", Mesh::MultipleMesh, rend));
	gameobj.push_back(new GameEntity("The floor", *Engine::GetMesh("Plane"), rend));
	gameobj.push_back(new GameEntity("My box", *Engine::GetMesh("Box"), rend));
	//gameobj.push_back(new GameEntity("Big ass wall", *Engine::GetMesh("Plane"), rend));
	rayGun = new GameEntity("RayGun", *Engine::GetMesh("RayGun"), rend);

	//gameobj[0]->SetTag("MultiMesh");
	//gameobj[0]->Scale(0.05f);
	gameobj[0]->Rotate(3.038f, 0, 0);
	gameobj[0]->Scale(100.0f);
	gameobj[0]->LoadTexture("models/textures/brick.jpg");
	//gameobj[0]->SetTag("Floor");
	
	
	gameobj[1]->LoadTexture("models/textures/raygunUVTest.tga");
	gameobj[1]->Rotate(90, 0, 0);
	gameobj[1]->Translate(0, 3.5f, 0);
	
	//gameobj[2]->Rotate(90, 0, 0);
	//gameobj[2]->Scale(2.0f);
	//gameobj[2]->Translate(4, 3, -4);
	
	rayGun->LoadTexture("models/textures/raygunUVTest.tga");
	rayGunOffset = { 2,-1.5,-3 };
	rayGun->SetTag("Gun");


	GenrateWall(10, 10, 0);
}

void Game::Update()
{
	float speed = 0.25f;
	if (input->IsKeyDown(GLFW_KEY_LEFT))
	{
		//gameobj[1]->ApplyForce(glm::vec3(-speed, 0.0f, 0.0f));
	}
	if (input->IsKeyDown(GLFW_KEY_RIGHT))
	{
		//gameobj[1]->ApplyForce(glm::vec3(speed, 0.0f, 0.0f));
	}
	if (input->IsKeyDown(GLFW_KEY_UP))
	{
		//gameobj[1]->ApplyForce(glm::vec3(0.0f, 0.0f, -speed));
	}
	if (input->IsKeyDown(GLFW_KEY_DOWN))
	{
		//gameobj[1]->ApplyForce(glm::vec3(0.0f, 0.0f, speed));
	}
	if (input->IsKeyDown(GLFW_KEY_LEFT_BRACKET))
	{
		//gameobj[5]->Rotate(0.0f,-0.1f,0.0f);
	}
	if (input->IsKeyDown(GLFW_KEY_RIGHT_BRACKET))
	{
		//gameobj[5]->Rotate(0.0f, 0.1f, 0.0f);
	}
	if (input->IsKeyDown(GLFW_KEY_O))
	{
		lights[0]->myLight.lightRadius -= 0.1f;
	}
	if (input->IsKeyDown(GLFW_KEY_P))
	{
		lights[0]->myLight.lightRadius += 0.1f;
	}
	if (input->IsKeyPressed(GLFW_KEY_TAB))
	{
		rend->ChangeSkyBox();
	}
	if (input->IsKeyDown(GLFW_KEY_0))
	{
		lights[0]->myLight.color = { 1.0f,1.0f,1.0f,1.0f };

		lights[1]->myLight.color = { 1.0f,0.0f,0.0f,1.0f };
		lights[2]->myLight.color = { 0.0f,1.0f,0.0f,1.0f };
		lights[3]->myLight.color = { 0.0f,0.0f,1.0f,1.0f };
	}
	if (input->IsKeyDown(GLFW_KEY_1))
	{
		lights[0]->myLight.color = { 1.0f,0.0f,0.0f,1.0f };

		lights[2]->myLight.color = { 1.0f,0.0f,0.0f,1.0f };
		lights[3]->myLight.color = { 0.0f,1.0f,0.0f,1.0f };
		lights[1]->myLight.color = { 0.0f,0.0f,1.0f,1.0f };
	}
	if (input->IsKeyDown(GLFW_KEY_2))
	{
		lights[0]->myLight.color = { 0.0f,1.0f,0.0f,1.0f };

		lights[3]->myLight.color = { 1.0f,0.0f,0.0f,1.0f };
		lights[1]->myLight.color = { 0.0f,1.0f,0.0f,1.0f };
		lights[2]->myLight.color = { 0.0f,0.0f,1.0f,1.0f };
	}
	if (input->IsKeyDown(GLFW_KEY_3))
	{
		lights[0]->myLight.color = { 0.0f,0.0f,1.0f,1.0f };

		lights[1]->myLight.color = { 1.0f,0.0f,0.0f,1.0f };
		lights[2]->myLight.color = { 0.0f,1.0f,0.0f,1.0f };
		lights[3]->myLight.color = { 1.0f,1.0f,1.0f,1.0f };
	}
	if (input->IsKeyPressed(GLFW_KEY_G))
	{
		for (unsigned int i = 0; i < gameobj.size(); i++)
		{
			if ((gameobj)[i]->applyGrav)
			{
				(gameobj)[i]->applyGrav = false;
			}
			else
			{
				(gameobj)[i]->applyGrav = true;
			}
		}
	}
	if (input->IsMouseClick(GLFW_MOUSE_BUTTON_2))
	{
		currentMouseClick = true;
	}
	//if (currentMouseClick == true)// && previousMouseClick == false)
	//{
	//	Shoot();
	//}
	if (currentMouseClick == true && previousMouseClick == false)
	{
		Shoot();
	}

	for(unsigned int i = 0; i < gameobj.size(); i++)
	{
		(gameobj)[i]->Update();
		rayGun->transform.position = rend->cam->camPos + (rend->cam->rotMat *(glm::vec3(-1,0,0) + rayGunOffset));
		rayGun->transform.rotation = glm::vec3(-rend->cam->camRot.x, rend->cam->camRot.y, rend->cam->camRot.z);
		rayGun->Rotate(0, 180, 0);
		rayGun->SetWorldPos();
		
	}
	for (unsigned int i = 0; i < 4; i++)
	{
		//(lights)[i]->Move(sin(Engine::time.t) / 10.0f, cos(Engine::time.t) / 10.0f,0);
		//(lights)[i]->myLight.color += glm::vec4(sin(Engine::time.t * 3) / 20.0f, cos(Engine::time.t * 3) / 20.0f, sin(Engine::time.t * 3) / 20.0f,1.0);
	}
	for (unsigned int i = 0; i < bullets.size(); i++)
	{
		bullets[i]->Update();
		for (unsigned int j = 0; j < gameobj.size(); j++)
		{
			if(gameobj[j]->GetTag() == "GameObject")
			{
				if(BoxCollision(bullets[i]->bulletModel,gameobj[j]))
				{
					bullets[i]->destroyBullet = true;
				}
			}
		}
	}
	for (unsigned int i = 0; i < bullets.size(); i++)
	{
		if (bullets[i]->destroyBullet)
		{			
			//DestroyBullet(i);
			StickBullet(i);
			//if (bulletHolder != nullptr) { delete bulletHolder;  bulletHolder = nullptr; }
		}
	}
	int collisions = 0;
	for (unsigned int i = 0; i < gameobj.size(); i++)
	{
		if (BoxCollision(player->myPlayer, gameobj[i]))
		{
			if (gameobj[i]->GetTag() != "Gun")
			{
				collisions++;
				player->grounded = true;
			}
			//player->myPlayer->transform.position.y = gameobj[i]->transform.position.y + .5f;
		}
	}
	if (collisions == 0) 
	{
		player->grounded = false;
	}
	//(gameobj)[0]->Translate(sin(Engine::time.t)/20.0f,0.0f,0.0f);
	//gameobj[0]->Scale(abs(sin(Engine::time.t))/5.0f);
	//gameobj[0]->Rotate(1.0f*Engine::time.dt, 2.0f*Engine::time.dt, 0.0f);
	//gameobj[1]->Rotate(0.0f, 2.0f*Engine::time.dt, 0.0f);

	previousMouseClick = currentMouseClick;
	currentMouseClick = false;
}

void Game::RenderLights()
{
	for (unsigned int i = 0; i < lights.size(); i++) {
		rend->AddToRenderer(*lights[i]->sphere);
	}
}

void Game::UnrenderLights()
{
	for (unsigned int i = 0; i < lights.size(); i++) {
		rend->RemoveFromRenderer(lights[i]->sphere->rendID);
	}
}

void Game::DeleteBullet(unsigned int j)
{
	
	unsigned int id = bullets[j]->bulletID;
	delete bullets[j];
	bullets.erase(bullets.begin() + id);
	for (unsigned int i = 0; i < bullets.size(); i++)
	{
		bullets[i]->bulletID = i;
	}
}

void Game::DestroyBullet(unsigned int i)
{
	rend->RemoveFromRenderer(bullets[i]->bulletModel->rendID);
	rend->RemoveFromLights(bullets[i]->bulletLight->lightID);
	DeleteBullet(i);
}

void Game::StickBullet(unsigned int i)
{
	bullets[i]->bulletModel->SetTag("GameObject");
	bullets[i]->bulletModel->ridgidBody.velocity = glm::vec3(0);
	bullets[i]->bulletModel->applyGrav = false;
	//rend->RemoveFromLights(bullets[i]->bulletLight->lightID);
	gameobj.push_back(bullets[i]->bulletModel);
	lights.push_back(bullets[i]->bulletLight);
	DeleteBullet(i);
}

void Game::BulletLightsOn()
{
	for (unsigned int i = 0; i < bullets.size(); i++)
	{
		bullets[i]->bulletLight->lightID = rend->AddToLights(*bullets[i]->bulletLight);
	}
}

void Game::BulletLightsOff()
{
	for (unsigned int i = 0; i < bullets.size(); i++)
	{
		rend->RemoveFromLights(bullets[i]->bulletLight->lightID);
	}
}

void Game::GenrateWall(int width, int height, float spacing)
{
	for(float i = 0; i <= width; i++)
	{
		for (float j = 0; j <= height; j++)
		{
			GameEntity* block = new GameEntity("Wall Bock", *Engine::GetMesh("Box"), rend);
			block->Translate(0, 1.0f, 0);
			block->Scale(6,6,0.5f);
			block->Translate(spacing + ((i) * block->transform.scale.x), spacing + ((j) * block->transform.scale.y), 0 );
			gameobj.push_back(block);
		}
	}
}

bool Game::SphereCollision(GameEntity *obj1, GameEntity *obj2)
{
	float radius1 = obj1->collider.radius;
	float radius2 = obj2->collider.radius;

	glm::vec3 center1 = obj1->transform.position;
	glm::vec3 center2 = obj2->transform.position;
	glm::vec3 distance = center1 - center2;

	float dist = glm::pow(distance.z, 2) + glm::pow(distance.y, 2) + glm::pow(distance.x, 2);

	if(dist <= glm::pow((radius1 + radius2), 2))
	{
		obj1->collider.Colliding = true;
		obj2->collider.Colliding = true;
		return true;
	}
	obj1->collider.Colliding = false;
	obj2->collider.Colliding = false;
	return false;
}

bool Game::BoxCollision(GameEntity *obj1, GameEntity *obj2)
{

	glm::vec3 obj1Min = obj1->collider.bbMin - obj1->collider.skin;
	glm::vec3 obj1Max = obj1->collider.bbMax + obj1->collider.skin;

	glm::vec3 obj2Min = obj2->collider.bbMin - obj2->collider.skin;
	glm::vec3 obj2Max = obj2->collider.bbMax + obj2->collider.skin;

	if (!(obj1Min.x < obj2Max.x))
	{
		obj1->collider.Colliding = false;
		obj2->collider.Colliding = false;
		return false;
	}
	if (!((obj1Max.x > obj2Min.x)))
	{
		obj1->collider.Colliding = false;
		obj2->collider.Colliding = false;
		return false;
	}
	if (!((obj1Min.y < obj2Max.y)))
	{
		obj1->collider.Colliding = false;
		obj2->collider.Colliding = false;
		return false;
	}
	if (!((obj1Max.y > obj2Min.y)))
	{
		obj1->collider.Colliding = false;
		obj2->collider.Colliding = false;
		return false;
	}
	if (!((obj1Min.z < obj2Max.z)))
	{
		obj1->collider.Colliding = false;
		obj2->collider.Colliding = false;
		return false;
	}
	if (!((obj1Max.z > obj2Min.z)))
	{
		obj1->collider.Colliding = false;
		obj2->collider.Colliding = false;
		return false;
	}

	obj1->collider.Colliding = true;
	obj2->collider.Colliding = true;
	return true; 
}

bool Game::SphereBoxCollision(GameEntity * sphere, GameEntity * box)
{
	float dist2;
	float rad2 = glm::pow(sphere->collider.radius, 2);

	glm::vec3 distance = box->transform.position - sphere->transform.position;

	glm::vec3 half = (box->collider.bbMin - box->collider.bbMax)/2.0f;
	glm::vec3 p = box->transform.position + glm::clamp(distance, -half, half);
	dist2 = glm::pow((p.x - sphere->transform.position.x),2) + glm::pow((p.y - sphere->transform.position.y),2) + glm::pow((p.z - sphere->transform.position.z),2);

	if (dist2 <= rad2)
	{
		box->collider.Colliding = true;
		sphere->collider.Colliding = true;
		return true;
	}
	box->collider.Colliding = false;
	sphere->collider.Colliding = false;
	return false;
}

void Game::Shoot()
{
	glm::vec3 bulletModelOffset =  rayGunOffset + glm::vec3(0,0,-5);
	glm::vec3 moveBullet = rend->cam->camPos + (rend->cam->rotMat *(glm::vec3(-1, 0, 0) + bulletModelOffset));

	glm::vec3 lightOffset = bulletModelOffset + glm::vec3(0, 0, 1);
	glm::vec3 moveLight = rend->cam->camPos + (rend->cam->rotMat *(glm::vec3(-1, 0, 0) + lightOffset));

	unsigned int pos = bullets.size();
	bullets.push_back( new Bullet(30, *Engine::GetMesh("Box"), rend));
	bullets[pos]->bulletID = pos;
	bullets[pos]->bulletModel->transform.position = moveBullet;
	bullets[pos]->bulletLight->sphere->transform.position = moveLight;
	bullets[pos]->bulletLight->myLight.lightPos = moveLight;
}

void Game::CreatePlayer()
{
	player = new Player("Mike", *Engine::GetMesh("Box"), rend);
	player->myPlayer->applyGrav = true;
	player->myPlayer->applyFric = true;
	player->myPlayer->fricStrength = 0.4f;
	player->myPlayer->Translate(0, 50, 10);
	player->myPlayer->collider.skin = 3.5f;
	rend->RemoveFromRenderer(player->myPlayer->rendID);
}

