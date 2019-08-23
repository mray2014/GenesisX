#include "GameEntity.h"

GameEntity::GameEntity(Renderer *r)
	:Object()
{
	name = "";
	tag = "GameObject";
	objMesh = new Mesh();
	rend = r;
	rendID = ((Renderer*)rend)->AddToRenderer(*this);
	color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	specular = glm::vec4(2.0f, 2.0f, 2.0f, 2.0f);
	collider.skin = 0.1f;
	ResetGameEntity();
}
GameEntity::GameEntity(std::string nm, Mesh &oM, Renderer *r)
	:Object()
{
	name = nm;
	tag = "GameObject";
	objMesh = &oM;
	rend = r;
	rendID = ((Renderer*)rend)->AddToRenderer(*this);
	color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	specular = glm::vec4(2.0f, 2.0f, 2.0f, 2.0f);
	collider.skin = 0.1f;
	ResetGameEntity();
}
GameEntity::GameEntity(std::string nm, std::vector<Vertex> &v, std::vector<unsigned int> &i, Renderer *r)
	:Object()
{
	name = nm;
	tag = "GameObject";
	objMesh = new Mesh(v, i);
	rend = r;
	rendID = ((Renderer*)rend)->AddToRenderer(*this);
	color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	specular = glm::vec4(2.0f, 2.0f, 2.0f, 2.0f);
	collider.skin = 0.1f;
	ResetGameEntity();
}

GameEntity::GameEntity(std::string nm, std::string filename, Mesh::MeshType t, std::string file,Renderer * r)
{
	name = nm;
	tag = "GameObject";
	objMesh = new Mesh(filename, t, file);
	rend = r;
	rendID = rend->AddToRenderer(*this);
	color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	specular = glm::vec4(2.0f, 2.0f, 2.0f, 2.0f);
	collider.skin = 0.1f;
	ResetGameEntity();
}

GameEntity::~GameEntity()
{
	//rend->RemoveFromRenderer(rendID);
}
void GameEntity::Update()
{
	//printf("updating at time %f\n", Engine::time.t);
	//printf("Rendering %s", &tag[0]);
	if(applyGrav)
	{
		ApplyGravity();
	}
	if(applyFric)
	{
		CalculateFric();
		//printf("Fric- x: %f, y: %f, z %f\n", ridgidBody.friction.x, ridgidBody.friction.y, ridgidBody.friction.z);
		//printf("Foward- x: %f, y: %f, z %f\n", transform.forward.x, transform.forward.y, transform.forward.z);
		//printf("Right- x: %f, y: %f, z %f\n", transform.right.x, transform.right.y, transform.right.z);
		//printf("Up- x: %f, y: %f, z %f\n", transform.up.x, transform.up.y, transform.up.z);
	}
	UpdateVelocity();
	//SetDirection();
	//printf("x: %.3f, y: %.3f, z: %.3f\n ", transform.forward.x, transform.forward.y, transform.forward.z);
}
void GameEntity::SetWorldPos()
{
	glm::mat4 myTrans = glm::translate(transform.position);
	glm::mat4 myScale = glm::scale(transform.scale);
	glm::mat4 myRot = glm::yawPitchRoll(transform.rotation.y, transform.rotation.x, transform.rotation.z);

	worldPos = myTrans * myRot * myScale;

	if (parent != nullptr) {
		worldPos = parent->worldPos * worldPos;
	}

	collider.bbMin = glm::mat3(myScale) * collider.boundingBox;
	collider.bbMax = glm::mat3(myScale) * collider.boundingBox;
	collider.bbMin = transform.position - collider.bbMin;
	collider.bbMax = transform.position + collider.bbMax;

	float r = collider.bbMax.x;
	if (r < collider.bbMax.y)
	{
		r = collider.bbMax.y;
	}
	if (r < collider.bbMax.z)
	{
		r = collider.bbMax.z;
	}
	collider.radius = r;
}
void GameEntity::ResetGameEntity()
{
	worldPos = glm::mat4(1.0f);
	applyGrav = false;
	maxSpeed = 0.6f;
	fricStrength = 10.0f;
	gravity = {0,-1.0f,0};

	transform.position = {0.0f,0.0f,0.0f};
	transform.scale = { 1.0f,1.0f,1.0f};
	transform.rotation = { 0.0f,0.0f,0.0f};

	ridgidBody.velocity = { 0.0f,0.0f,0.0f };
	ridgidBody.acceleration = { 0.0f,0.0f,0.0f };
	ridgidBody.friction = { 0.0f,0.0f,0.0f };
	ridgidBody.mass = 1.0f;

	FindRadius();
	SetWorldPos();
}
GLuint GameEntity::GetVertArr()
{
	return objMesh->vertArr;
}

unsigned int GameEntity::GetCount()
{
	return objMesh->count;
}

void GameEntity::SetTag(std::string tagName)
{
	tag = tagName;
}

std::string GameEntity::GetTag()
{
	return tag;
}
void GameEntity::FindRadius()
{
	float largestX = 0;
	float largestY = 0;
	float largestZ = 0;
	for(unsigned int i = 0; i< objMesh->verts.size();i++)
	{
		glm::vec3 pos = objMesh->verts[i].pos;
		float x = glm::abs(pos.x);
		float y = glm::abs(pos.y);
		float z = glm::abs(pos.z);
		if(x > largestX)
		{
			largestX = x;
		}
		if (y > largestY)
		{
			largestY = y;
		}
		if (z > largestZ)
		{
			largestZ = z;
		}
	}
	/*if(largestX == 0)
	{
		largestX = 0.5;
	}
	if (largestY == 0)
	{
		largestY = 0.5;
	}
	if (largestZ == 0)
	{
		largestZ = 0.5;
	}*/
	collider.bbMin = {0, 0, 0};
	collider.bbMax = {0, 0, 0};
	collider.boundingBox = glm::abs(glm::vec3(largestX, largestY, largestZ));
}

void GameEntity::SetDirection()
{
	glm::mat3 myRot = (glm::mat3) glm::yawPitchRoll(transform.rotation.y, transform.rotation.x, transform.rotation.z);

	transform.up = myRot * glm::vec3(0, 1, 0);
	transform.right = myRot * glm::vec3(1, 0, 0);
	transform.forward = glm::cross(transform.up, transform.right);
}

#pragma region Tranform Methods

void GameEntity::Translate(glm::vec3 transVec)
{
	transform.position.x += transVec.x;
	transform.position.y += transVec.y;
	transform.position.z += transVec.z;

	SetWorldPos();
}

void GameEntity::Translate(float x, float y, float z)
{
	transform.position.x += x;
	transform.position.y += y;
	transform.position.z += z;

	SetWorldPos();
}

void GameEntity::Scale(glm::vec3 scaleVec)
{
	transform.scale.x = scaleVec.x;
	transform.scale.y = scaleVec.y;
	transform.scale.z = scaleVec.z;

	SetWorldPos();
}

void GameEntity::Scale(float x, float y, float z)
{
	transform.scale.x = x;
	transform.scale.y = y;
	transform.scale.z = z;

	SetWorldPos();
}

void GameEntity::Scale(float c)
{
	transform.scale.x = c;
	transform.scale.y = c;
	transform.scale.z = c;

	SetWorldPos();
}

void GameEntity::Rotate(glm::vec3 RotVec)
{
	glm::vec3 newRot = {RotVec.x *(3.14/180), RotVec.y *(3.14 / 180) ,RotVec.z *(3.14 / 180) };

	transform.rotation.x += newRot.x;
	transform.rotation.y += newRot.y;
	transform.rotation.z += newRot.z;

	SetWorldPos();
}

void GameEntity::Rotate(float x, float y, float z)
{
	glm::vec3 newRot = { x *(3.14 / 180), y *(3.14 / 180) ,z *(3.14 / 180) };

	transform.rotation.x += newRot.x;
	transform.rotation.y += newRot.y;
	transform.rotation.z += newRot.z;

	SetWorldPos();
}
#pragma endregion

#pragma region Ridgid Body Methods

void GameEntity::ApplyForce(glm::vec3 force)
{
	ridgidBody.acceleration += force / ridgidBody.mass;
}

void GameEntity::ApplyGravity()
{
	ridgidBody.acceleration += gravity * ridgidBody.mass;
}

void GameEntity::CalculateFric()
{
	if (glm::abs(glm::length(ridgidBody.velocity)) <= 0.01f)
	{
		ridgidBody.velocity = { 0.0f,0.0f,0.0f };
		//ridgidBody.friction = { 0.0f,0.0f,0.0f };
		return;
	}

	ridgidBody.friction = ridgidBody.velocity * -1.0f;
	ridgidBody.friction = glm::normalize(ridgidBody.friction);
	ridgidBody.friction *= fricStrength;
	ApplyForce(ridgidBody.friction);
}

void GameEntity::UpdateVelocity()
{
	
	ridgidBody.velocity += ridgidBody.acceleration * Engine::time.dt;
	ridgidBody.velocity = glm::clamp(ridgidBody.velocity, -maxSpeed, maxSpeed);
	ridgidBody.acceleration = { 0.0f,0.0f,0.0f };

	if (glm::length(ridgidBody.velocity) <= 0.001f && glm::length(ridgidBody.velocity) >= -0.001f)
	{
		ridgidBody.velocity = { 0.0f,0.0f,0.0f };
	}
	Translate(ridgidBody.velocity);
}
#pragma endregion
void GameEntity::LoadTexture(char * filename)
{
	//FREE_IMAGE_FORMAT type = FreeImage_GetFileType("models/textures/raygunUVTest.tga",0);
	//FIBITMAP* image = FreeImage_Load(type, "models/textures/raygunUVTest.tga");
	FREE_IMAGE_FORMAT type = FreeImage_GetFileType(filename, 0);
	FIBITMAP* image = FreeImage_Load(type, filename);
	if (image == nullptr) return;

	FIBITMAP* image32Bit = FreeImage_ConvertTo32Bits(image);
	FreeImage_Unload(image);

	texID;
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);

	int texWidth = FreeImage_GetWidth(image32Bit);
	int texHeight = FreeImage_GetHeight(image32Bit);
	BYTE* texAddress = FreeImage_GetBits(image32Bit);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB_ALPHA, texWidth, texHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, (void*)texAddress);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	FreeImage_Unload(image32Bit);
	glBindTexture(GL_TEXTURE_2D, 0);
	hasTex = true;
}

GLuint GameEntity::GetTexId()
{
	return texID;
}
