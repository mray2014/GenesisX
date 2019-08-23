#pragma once
#include <ctime>
#include "Object.h"
class Timer
	:Object
{
public:
	Timer();
	~Timer() override;

	void Update() override;

	float t;
	float dt;
	float fps;

private:
	clock_t cur;
	clock_t prev;
};

