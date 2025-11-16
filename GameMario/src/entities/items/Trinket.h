#pragma once
#include "core/GameObject.h"

#define TRINKET_WIDTH 16
#define TRINKET_HEIGHT 16

#define ID_TEX_FLOWER 1110001
#define ID_TEX_STAR 1110002
#define ID_TEX_MUSHROOM 1110003
class CTrinket :
    public CGameObject
{
protected:
	int type;

public:
	CTrinket(int id, float x, float y, int z, int type) : CGameObject(id, x, y, z)
	{
		this->type = type;
	}

	void Render();
	void Update(DWORD dt) {}
	void GetBoundingBox(float& l, float& t, float& r, float& b);
	void RenderBoundingBox();

	int IsCollidable() { return 0; }

	int IsDirectionColliable(float nx, float ny);
};

typedef CTrinket* LPTRINKET;

