#pragma once
#include "GameObject.h"
#define CLOUD_WIDTH 16
#define CLOUD_HEIGHT 16

#define ID_TEX_CLOUD_TOP_LEFT 1020110
#define ID_TEX_CLOUD_TOP_CENTER 1020120
#define ID_TEX_CLOUD_TOP_RIGHT 1020130
#define ID_TEX_CLOUD_BOTTOM_LEFT 1020210
#define ID_TEX_CLOUD_BOTTOM_CENTER 1020220
#define ID_TEX_CLOUD_BOTTOM_RIGHT 1020230

class CCloud :
	public CGameObject
{
protected:
	int width;				// Unit: cell 
	//int height; default = 2				// Unit: cell

public:
	CCloud(float x, float y, int z, int width) : CGameObject(x, y, z)
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

typedef CCloud* LPCLOUD;

