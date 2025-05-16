#pragma once
#include "GameObject.h"
#include "Mario.h"
#include "Fireball.h"
#include "Koopa.h"

#include "Game.h"
#include "PlayScene.h"


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
#define PIRANHA_PLANT_STATE_DIE 500

#define PIRANHA_PLANT_MOVE_SPEED -0.1f

#define ID_ANI_PIRANHA_PLANT_LEFT_MOVE 130000
#define ID_ANI_PIRANHA_PLANT_RIGHT_MOVE 130010
#define ID_ANI_PIRANHA_PLANT_TOP_LEFT_SHOOT 130110
#define ID_ANI_PIRANHA_PLANT_BOTTOM_LEFT_SHOOT 130210
#define ID_ANI_PIRANHA_PLANT_TOP_RIGHT_SHOOT 130120
#define ID_ANI_PIRANHA_PLANT_BOTTOM_RIGHT_SHOOT 130220

class CPiranhaPlant : public CGameObject
{
protected:
	int moveUp = 0;
	int moveDown = 0;
	ULONGLONG moveUpStart = -1;
	ULONGLONG hoverStart = -1;
	ULONGLONG moveDownStart = -1;
	ULONGLONG lastMove = -1;
	float y00 = -1;
	bool hasShot = false;

	float x0;
	float y0;

	int isDefeated = 0;

	int originalChunkId;

	// 0 bottom left, 1 lower center left, 2 upper center left, 3 top left
	//4 top right, 5 upper center right, 6 lower center right, 7 bottom right

	ULONGLONG deathStart;
	int GetAiming();
	void Shoot(int direction);
	bool IsMarioInRange();
	bool IsMarioOnTop();

	void OnNoCollision(DWORD dt);
	void OnCollisionWith(LPCOLLISIONEVENT e);
	void OnCollisionWithKoopa(LPCOLLISIONEVENT e);

public:
	CPiranhaPlant(int id, float x, float y, int z, int originalChunkId);

	void Render();
	void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	void GetBoundingBox(float& l, float& t, float& r, float& b);
	void SetState(int state);

	int IsBlocking() { return 0; }
	int IsCollidable() { return (state != PIRANHA_PLANT_STATE_DIE && isDefeated == 0); }
	void StartDeath()
	{
		deathStart = GetTickCount64();
	}
	void StartMoveUp() {
		moveUp = 1;
		moveUpStart = GetTickCount64();
	}
	void StartMoveDown() {
		moveDown = 1;
		moveDownStart = GetTickCount64();
	}
	void GetOriginalPosition(float& x0, float& y0) { x0 = this->x0; y0 = this->y0; }
	int IsDefeated() { return isDefeated; }
	void SetIsDefeated(int isDefeated) { this->isDefeated = isDefeated; }
	int GetOriginalChunkId() { return originalChunkId; }
};

