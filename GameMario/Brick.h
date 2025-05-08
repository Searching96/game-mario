#pragma once

#include "GameObject.h"
#include "Animation.h"
#include "Animations.h"

#define ID_SPRITE_BRICK 1000001
#define ID_SPRITE_STRIPED_BRICK 1000002

#define BRICK_CELL_WIDTH 16
#define BRICK_CELL_HEIGHT 16

class CBrick : public CGameObject {
protected:
	int type; // 0: brick, 1: striped

public:
	CBrick(int id, float x, float y, int z, int type) : CGameObject(id, x, y, z)
	{
		this->type = type;
	}

	void Render();
	void Update(DWORD dt) {}
	void GetBoundingBox(float& l, float& t, float& r, float& b);
	void RenderBoundingBox();
	int IsBlocking() { return 1; }

	int IsDirectionColliable(float nx, float ny);
};

typedef CBrick* LPBRICK;