#pragma once
#include "core/GameObject.h"
#include "entities/player/Mario.h"
#include "entities/player/Fireball.h"
#include "entities/enemies/Koopa.h"

#include "core/Game.h"
#include "core/PlayScene.h"


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
#define ID_ANI_PIRANHA_PLANT_UPRIGHT 130020
#define ID_ANI_PIRANHA_PLANT_TOP_LEFT_SHOOT 130110
#define ID_ANI_PIRANHA_PLANT_BOTTOM_LEFT_SHOOT 130210
#define ID_ANI_PIRANHA_PLANT_TOP_RIGHT_SHOOT 130120
#define ID_ANI_PIRANHA_PLANT_BOTTOM_RIGHT_SHOOT 130220


#define ID_ANI_GREEN_PLANT_LEFT_MOVE 131000
#define ID_ANI_GREEN_PLANT_RIGHT_MOVE 131010
#define ID_ANI_GREEN_PLANT_UPRIGHT 131020
#define ID_ANI_GREEN_PLANT_TOP_LEFT_SHOOT 131110
#define ID_ANI_GREEN_PLANT_BOTTOM_LEFT_SHOOT 131210
#define ID_ANI_GREEN_PLANT_TOP_RIGHT_SHOOT 131120
#define ID_ANI_GREEN_PLANT_BOTTOM_RIGHT_SHOOT 131220

class CPiranhaPlant : public CGameObject
{
protected:
	int moveUp = 0;
	int moveDown = 0;
	ULONGLONG moveUpStart = -1;
	ULONGLONG hoverStart = -1;
	ULONGLONG moveDownStart = -1;
	ULONGLONG lastMove = -1;
	int type; // 0: red, 1: green, 2: green no shoot
	float y00 = -1.0f;
	bool hasShot = false;

	float x0;
	float y0;

	bool isDefeated = false;

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
	CPiranhaPlant(int id, float x, float y, int z, int type, int originalChunkId);

	void Render();
	void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	void GetBoundingBox(float& l, float& t, float& r, float& b);
	void SetState(int state);

	int IsBlocking() { return 0; }
	int IsCollidable() { return (state != PIRANHA_PLANT_STATE_DIE && !isDefeated); }
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

	int GetType() {return type;}
	void GetOriginalPosition(float& x0, float& y0) { x0 = this->x0; y0 = this->y0; }
	int IsDefeated() { return isDefeated; }
	void SetIsDefeated(int isDefeated) { this->isDefeated = isDefeated; }
	int GetOriginalChunkId() { return originalChunkId; }
};

