#pragma once
#include "GameObject.h"

//---------------- BLANK SPRITE ------------------
#define SPRITE_ID_BLANK						1064300
//---------------- SHADOW ------------------
#define SPRITE_ID_SHADOW_TOP_RIGHT                1060130
#define SPRITE_ID_SHADOW_MIDDLE_RIGHT            1060230
#define SPRITE_ID_SHADOW_BOTTOM_LEFT          1060310
#define SPRITE_ID_SHADOW_BOTTOM_CENTER        1060320
#define SPRITE_ID_SHADOW_BOTTOM_RIGHT         1060330

//---------------- ORANGE BOX ------------------
#define SPRITE_ID_ORANGE_TOP_LEFT           1061110
#define SPRITE_ID_ORANGE_TOP_CENTER         1061120
#define SPRITE_ID_ORANGE_TOP_RIGHT          1061130
#define SPRITE_ID_ORANGE_MIDDLE_LEFT        1061210
#define SPRITE_ID_ORANGE_MIDDLE_CENTER      1061220
#define SPRITE_ID_ORANGE_MIDDLE_RIGHT       1061230
#define SPRITE_ID_ORANGE_BOTTOM_LEFT        1061310
#define SPRITE_ID_ORANGE_BOTTOM_CENTER      1061320
#define SPRITE_ID_ORANGE_BOTTOM_RIGHT       1061330

//---------------- BLUE BOX --------------------
#define SPRITE_ID_BLUE_TOP_LEFT             1062110
#define SPRITE_ID_BLUE_TOP_CENTER           1062120
#define SPRITE_ID_BLUE_TOP_RIGHT            1062130
#define SPRITE_ID_BLUE_MIDDLE_LEFT          1062210
#define SPRITE_ID_BLUE_MIDDLE_CENTER        1062220
#define SPRITE_ID_BLUE_MIDDLE_RIGHT         1062230
#define SPRITE_ID_BLUE_BOTTOM_LEFT          1062310
#define SPRITE_ID_BLUE_BOTTOM_CENTER        1062320
#define SPRITE_ID_BLUE_BOTTOM_RIGHT         1062330

//---------------- GREEN BOX -------------------
#define SPRITE_ID_GREEN_TOP_LEFT            1063110
#define SPRITE_ID_GREEN_TOP_CENTER          1063120
#define SPRITE_ID_GREEN_TOP_RIGHT           1063130
#define SPRITE_ID_GREEN_MIDDLE_LEFT         1063210
#define SPRITE_ID_GREEN_MIDDLE_CENTER       1063220
#define SPRITE_ID_GREEN_MIDDLE_RIGHT        1063230
#define SPRITE_ID_GREEN_BOTTOM_LEFT         1063310
#define SPRITE_ID_GREEN_BOTTOM_CENTER       1063320
#define SPRITE_ID_GREEN_BOTTOM_RIGHT        1063330

//---------------- WHITE BOX -------------------
#define SPRITE_ID_WHITE_TOP_LEFT            1064110
#define SPRITE_ID_WHITE_TOP_CENTER          1064120
#define SPRITE_ID_WHITE_TOP_RIGHT           1064130
#define SPRITE_ID_WHITE_MIDDLE_LEFT         1064210
#define SPRITE_ID_WHITE_MIDDLE_CENTER       1064220
#define SPRITE_ID_WHITE_MIDDLE_RIGHT        1064230
// Note: The sprite list provided does not include bottom row sprites for WHITE color.

//---------------- BLACK BOX -------------------
#define SPRITE_ID_BLACK_TOP_LEFT            1065110
#define SPRITE_ID_BLACK_TOP_CENTER          1065120
#define SPRITE_ID_BLACK_TOP_RIGHT           1065130
#define SPRITE_ID_BLACK_MIDDLE_LEFT         1065210
#define SPRITE_ID_BLACK_MIDDLE_CENTER       1065220
#define SPRITE_ID_BLACK_MIDDLE_RIGHT        1065230
#define SPRITE_ID_BLACK_BOTTOM_LEFT         1065310
#define SPRITE_ID_BLACK_BOTTOM_CENTER       1065320
#define SPRITE_ID_BLACK_BOTTOM_RIGHT        1065330

////----------------  HILL BOX -------------------
#define SPRITE_ID_HILL_TOP_LEFT            1066110
#define SPRITE_ID_HILL_TOP_CENTER          1066120
#define SPRITE_ID_HILL_TOP_RIGHT           1066130
#define SPRITE_ID_HILL_MIDDLE_LEFT         1066210
#define SPRITE_ID_HILL_MIDDLE_CENTER       1066220
#define SPRITE_ID_HILL_MIDDLE_RIGHT        1066230
#define SPRITE_ID_HILL_BOTTOM_LEFT         1066310
#define SPRITE_ID_HILL_BOTTOM_CENTER       1066320
#define SPRITE_ID_HILL_BOTTOM_RIGHT        1066330


//---------------- ENDING BACKGROUND -------------------
#define SPRITE_ID_ENDING_BORDER			    1060001
#define SPRITE_ID_ENDING_BACKGROUND		    1060002

#define BOX_CELL_WIDTH 16
#define BOX_CELL_HEIGHT 16

class CBox : public CGameObject
{
protected:
	int width;				// Unit: cell 
	int height;				// Unit: cell
	int color; // 0: orange, 1: blue, 2: green, 3: white, 4: hill, 5: black, 6: ending
	int bottomShadow;
public:
	CBox(int id, float x, float y, int z, int width, int height, int color, int bottomShadow)  : CGameObject(id, x, y, z)
	{
		this->height = height;
		this->width = width;
		this->color = color;
		this->bottomShadow = bottomShadow;
	}

	void Render();
	void RenderBox(CSprites* s);
	void RenderShadow(CSprites* s);
	void Update(DWORD dt) {}
	void GetBoundingBox(float& l, float& t, float& r, float& b);
	void RenderBoundingBox();
	int IsBlocking() { return color >= 5 ? 0 : 1; }

	int IsDirectionColliable(float nx, float ny);
};

typedef CBox* LPBOX;
