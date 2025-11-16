#pragma once
#include "core/GameObject.h"
class CPipe : public CGameObject
{
protected:
	//int width; default = 2				// Unit: cell 
	int height;				// Unit: cell
	float cellWidth;
	float cellHeight;
	int spriteIdTopLeft, spriteIdTopRight,
		spriteIdBottomLeft, spriteIdBottomRight;

public:
	CPipe(int id, float x, float y, int z,
		float cell_width, float cell_height, int height,
		int sprite_id_top_left, int sprite_id_top_right,
		int sprite_id_bottom_left, int sprite_id_bottom_right) : CGameObject(id, x, y, z)
	{
		this->height = height;
		this->cellWidth = cell_width;
		this->cellHeight = cell_height;
		this->spriteIdTopLeft = sprite_id_top_left;
		this->spriteIdTopRight = sprite_id_top_right;
		this->spriteIdBottomLeft = sprite_id_bottom_left;
		this->spriteIdBottomRight = sprite_id_bottom_right;
	}

	void Render();
	void Update(DWORD dt) {}
	void GetBoundingBox(float& l, float& t, float& r, float& b);
	void RenderBoundingBox();
	int IsBlocking() { return 1; }

	int IsDirectionColliable(float nx, float ny);
};

typedef CPipe* LPPIPE;
