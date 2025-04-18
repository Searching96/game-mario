#pragma once
#include "GameObject.h"
class CCloud :
	public CGameObject
{
protected:
	int width;				// Unit: cell 
	//int height; default = 2				// Unit: cell
	float cellWidth;
	float cellHeight;
	int spriteIdTopLeft, spriteIdTopCenter, spriteIdTopRight,
		spriteIdBottomLeft, spriteIdBottomCenter, spriteIdBottomRight;

public:
	CCloud(float x, float y,
		float cell_width, float cell_height, int width,
		int sprite_id_top_left, int sprite_id_top_center, int sprite_id_top_right,
		int sprite_id_bottom_left, int sprite_id_bottom_center, int sprite_id_bottom_right) : CGameObject(x, y)
	{
		this->width = width;
		this->cellWidth = cell_width;
		this->cellHeight = cell_height;
		this->spriteIdTopLeft = sprite_id_top_left;
		this->spriteIdTopCenter = sprite_id_top_center;
		this->spriteIdTopRight = sprite_id_top_right;
		this->spriteIdBottomLeft = sprite_id_bottom_left;
		this->spriteIdBottomCenter = sprite_id_bottom_center;
		this->spriteIdBottomRight = sprite_id_bottom_right;
	}

	void Render();
	void Update(DWORD dt) {}
	void GetBoundingBox(float& l, float& t, float& r, float& b);
	void RenderBoundingBox();

	int IsDirectionColliable(float nx, float ny);
};

typedef CCloud* LPCLOUD;

