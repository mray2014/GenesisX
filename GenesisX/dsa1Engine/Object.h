#pragma once
#include <iostream>
#include <string>
class Object
{
public:
	Object();
	virtual ~Object();

	virtual void Update() = 0;

	std::string tag;
private:

};

