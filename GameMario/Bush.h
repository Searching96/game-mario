#pragma once
#include "GameObject.h"
class CBush : public CGameObject
{
protected:
	int width;				// Unit: cell 
	//int height; default = 1				// Unit: cell
	float cellWidth;
	float cellHeight;
	int spriteId;

public:
	CBush(float x, float y,
		float cell_width, float cell_height, int width,
		int sprite_id) : CGameObject(x, y)
	{
		this->width = width;
		this->cellWidth = cell_width;
		this->cellHeight = cell_height;
		this->spriteId = sprite_id;
	}

	void Render();
	void Update(DWORD dt) {}
	void GetBoundingBox(float& l, float& t, float& r, float& b);
	void RenderBoundingBox();

	int IsDirectionColliable(float nx, float ny);
};

typedef CBush* LPBUSH;