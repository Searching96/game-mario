#pragma once
#include "GameObject.h"
#include "Mario.h"

#define PIRANHA_PLANT_BBOX_WIDTH 16
#define PIRANHA_PLANT_BBOX_HEIGHT 24
#define PIRANHA_PLANT_BBOX_OFFSET 8

#define PIRANHA_PLANT_MOVE_OFFSET 24

#define PIRANHA_PLANT_DIE_TIMEOUT 500
#define PIRANHA_PLANT_SHOOT_TIMEOUT 2000
#define PIRANHA_PLANT_HIDE_TIMEOUT 2000

#define PIRANHA_PLANT_STATE_HIDDEN	100
#define PIRANHA_PLANT_STATE_ASCEND 200
#define PIRANHA_PLANT_STATE_HOVER 300
#define PIRANHA_PLANT_STATE_DESCEND 400
#define PIRANHA_PLANT_STATE_DIED 500

#define PIRANHA_PLANT_BOUNCE_SPEED -0.1f

#define ID_ANI_PIRANHA_PLANT_LEFT_MOVE 130000
#define ID_ANI_PIRANHA_PLANT_RIGHT_MOVE 130010
#define ID_ANI_PIRANHA_PLANT_TOP_LEFT_SHOOT 130110
#define ID_ANI_PIRANHA_PLANT_BOTTOM_LEFT_SHOOT 130210
#define ID_ANI_PIRANHA_PLANT_TOP_RIGHT_SHOOT 130120
#define ID_ANI_PIRANHA_PLANT_BOTTOM_RIGHT_SHOOT 130220
#define ID_ANI_PIRANHA_PLANT_DIED 130300


class CPiranhaPlant : public CGameObject
{
protected:
	int moveUp = 0;
	int moveDown = 0;
	ULONGLONG moveUpStart = -1;
	ULONGLONG hoverStart = -1;
	ULONGLONG moveDownStart = -1;
	ULONGLONG lastMove = -1;
	float y0 = -1;
	//int aiming = 0; // 0: right up, 1: left up, 2: left down, 3: right down

	ULONGLONG die_start;

public:
	CPiranhaPlant(float x, float y);

	void Render();
	void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	void GetBoundingBox(float& l, float& t, float& r, float& b);
	void SetState(int state);
	int GetAiming();
	void StartMoveUp() { moveUp = 1; moveUpStart = GetTickCount64(); }
	void StartMoveDown() { moveDown = 1; moveDownStart = GetTickCount64(); }
};

