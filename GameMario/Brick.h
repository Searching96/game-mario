#pragma once

#include "GameObject.h"
#include "Animation.h"
#include "Animations.h"

#define ID_SPRITE_BRICK 1000001
#define ID_SPRITE_STRIPED_BRICK 1000002

class CBrick : public CGameObject {
protected:
	//int width; default = 1				// Unit: cell 
	//int height; default = 1				// Unit: cell
	float cellWidth;
	float cellHeight;
	int type; // 0: brick, 1: striped

public:
	CBrick(float x, float y,
		float cell_width, float cell_height, int type) : CGameObject(x, y)
	{
		this->cellWidth = cell_width;
		this->cellHeight = cell_height;
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