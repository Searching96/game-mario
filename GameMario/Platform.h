#pragma once

#include "GameObject.h"

#define ID_SPRITE_PLATFORM_TOP_LEFT 1070110
#define ID_SPRITE_PLATFORM_TOP_CENTER 1070120
#define ID_SPRITE_PLATFORM_TOP_RIGHT 1070130
#define ID_SPRITE_PLATFORM_BOTTOM_LEFT 1070210
#define ID_SPRITE_PLATFORM_BOTTOM_CENTER 1070220
#define ID_SPRITE_PLATFORM_BOTTOM_RIGHT 1070230

#define PLATFORM_CELL_WIDTH 16
#define PLATFORM_CELL_HEIGHT 16

// 
// The most popular type of object in Mario! 
// 
class CPlatform : public CGameObject
{
protected:
	int width;				// Unit: cell 
	int height;				// Unit: cell

public:
	CPlatform(float x, float y, int z, int width, int height) : CGameObject(x, y, z)
	{
		this->height = height;
		this->width = width;
	}

	void Render();
	void Update(DWORD dt) {}
	void GetBoundingBox(float& l, float& t, float& r, float& b);
	void RenderBoundingBox();
	virtual int IsBlocking() { return 1; }

	int IsDirectionColliable(float nx, float ny);
};

typedef CPlatform* LPPLATFORM;