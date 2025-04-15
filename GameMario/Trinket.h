#pragma once
#include "GameObject.h"
class CTrinket :
    public CGameObject
{
protected:
	//int width; default = 1				// Unit: cell 
	//int height; default = 1				// Unit: cell
	float cellWidth;
	float cellHeight;
	int spriteId;

public:
	CTrinket(float x, float y,
		float cell_width, float cell_height,
		int sprite_id) : CGameObject(x, y)
	{
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

typedef CTrinket* LPTRINKET;

