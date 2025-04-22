#pragma once
#include "GameObject.h"
#include "Mario.h"
#include "Fireball.h"

#define PIRANHA_PLANT_BBOX_WIDTH 16
#define PIRANHA_PLANT_BBOX_HEIGHT 32
#define PIRANHA_PLANT_BBOX_OFFSET 8

#define PIRANHA_PLANT_MOVE_OFFSET 32

#define PIRANHA_PLANT_DIE_TIMEOUT 400
#define PIRANHA_PLANT_SHOOT_TIMEOUT 2500
#define PIRANHA_PLANT_HOVER_TIMEOUT 4000
#define PIRANHA_PLANT_HIDE_TIMEOUT 3000

#define PIRANHA_PLANT_STATE_HIDDEN	100
#define PIRANHA_PLANT_STATE_ASCEND 200
#define PIRANHA_PLANT_STATE_HOVER 300
#define PIRANHA_PLANT_STATE_DESCEND 400
#define PIRANHA_PLANT_STATE_DIED 500

#define PIRANHA_PLANT_MOVE_SPEED -0.1f

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
	bool hasShot = false;

	CFireball* fireball = NULL;

	// 0 bottom left, 1 lower center left, 2 upper center left, 3 top left
	//4 top right, 5 upper center right, 6 lower center right, 7 bottom right

	ULONGLONG death_start;
	int GetAiming();
	void Shoot(int direction);
	bool IsMarioInRange();
	bool IsMarioOnTop();

public:
	CPiranhaPlant(float x, float y, CFireball* fireball);

	void Render();
	void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	void GetBoundingBox(float& l, float& t, float& r, float& b);
	void SetState(int state);

	int IsBlocking() { return 0; }
	int IsCollidable() { return 1; }
	void StartDeath()
	{
		death_start = GetTickCount64();
		SetState(PIRANHA_PLANT_STATE_DIED);
	}
	void StartMoveUp() {
		moveUp = 1;
		moveUpStart = GetTickCount64();
	}
	void StartMoveDown() {
		moveDown = 1;
		moveDownStart = GetTickCount64();
	}
};

