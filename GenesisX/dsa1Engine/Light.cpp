#include "Light.h"



Light::Light()
{
}

Light::Light(char* name, float inten,glm::vec3 lightP, glm::vec3 lightD, Renderer* r )
{
	//creating a sphere object
	sphere = new GameEntity(name,*Engine::GetMesh("Sphere"),r);
	//setting tag to Light
	sphere->SetTag("Light");

	//loading in data
	myLight.lightPos = lightP;
	myLight.lightDir = lightD;
	myLight.color = glm::vec4(1.0f);
	myLight.lightIntensity = inten;

	//calculating a radius based on attenuation variables
	myLight.linear = 0.0;
	myLight.quadratic = 0.005f;
	myLight.lightRadius = CalcRadius();
	//printf("%f", myLight.lightRadius);

	//modifying sphere
	sphere->Translate(myLight.lightPos);
	sphere->Scale(myLight.lightRadius);
	lightID = r->AddToLights(*this);
}


Light::~Light()
{
	if (sphere != nullptr) { delete sphere; sphere = nullptr; }
}

void Light::Move(glm::vec3 move)
{
	myLight.lightPos += move;
	sphere->Translate(move);
}
void Light::Move(float x, float y, float z)
{
	myLight.lightPos.x += x;
	myLight.lightPos.y += y;
	myLight.lightPos.z += z;
	sphere->Translate(x,y,z);
}

GLfloat  Light::CalcRadius()
{
	GLfloat radius;
	GLfloat constant = 1.0;
	GLfloat lightMax = std::fmaxf(std::fmaxf(myLight.color.r, myLight.color.g), myLight.color.b);

	radius = (-myLight.linear + std::sqrtf(myLight.linear*myLight.linear - 4.0f * myLight.quadratic *(constant - (256.0f/ 5.0f) * lightMax))) / (2.0f * myLight.quadratic);
	return radius;
}
