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

#define ID_TEX_SWIRL_CLOUD_SINGLE 1020300
#define ID_TEX_SWIRL_CLOUD_LEFT 1020310
#define ID_TEX_SWIRL_CLOUD_RIGHT 1020320

class CCloud :
	public CGameObject
{
protected:
	int width;				// Unit: cell 
	int height; // default: 2 for normal cloud, 1 for swirl				// Unit: cell
	int type; 			// 0: normal, 1: swirl

public:
	CCloud(int id, float x, float y, int z, int width, int type) : CGameObject(id, x, y, z)
	{
		this->width = width;
		this->type = type;
		this->height = type == 0 ? 2 : 1; // 2 for normal cloud, 1 for swirl
	}

	void Render();
	void Update(DWORD dt) {}
	void GetBoundingBox(float& l, float& t, float& r, float& b);
	void RenderBoundingBox();
	int IsCollidable() { return 0; }

	int IsDirectionColliable(float nx, float ny);
};

typedef CCloud* LPCLOUD;

