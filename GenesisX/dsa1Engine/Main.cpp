//#include <vld.h>
#include "Engine.h"
int main()
{
	Engine myEngine;
	if (myEngine.Init() == false) return -1;
	if (myEngine.bufferModel() == false) return -1;
	if (myEngine.useShader() == false) return -1;

	myEngine.gameLoop();
	return 0;
}
