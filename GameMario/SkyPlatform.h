#pragma once
#include "Platform.h"

#define ID_SPRITE_SKY_PLATFORM 1071000

class CSkyPlatform :
	public CPlatform
{
public:
	CSkyPlatform(float x, float y, int z, int width, int height)
		: CPlatform(x, y, z, width, height)
	{

	};
	void Render();
	int IsBlocking() { return 1; }
	int IsDirectionColliable(float nx, float ny);
};

