#pragma once
#include "GameObject.h"

#define ID_TEX_BUSH 1100000

#define BUSH_WIDTH 16
#define BUSH_HEIGHT 16

class CBush : public CGameObject
{
protected:
	int width;				// Unit: cell 
	//int height; default = 1				// Unit: cell

public:
	CBush(int id, float x, float y, int z, int width) : CGameObject(id, x, y, z)
	{
		this->width = width;
	}

	void Render();
	void Update(DWORD dt) {}
	void GetBoundingBox(float& l, float& t, float& r, float& b);
	void RenderBoundingBox();

	int IsCollidable() { return 0; }


	int IsDirectionColliable(float nx, float ny);
};

typedef CBush* LPBUSH;