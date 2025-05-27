#pragma once
#include "GameObject.h"

#define KOOPA_GRAVITY 0.002f
#define KOOPA_WALKING_SPEED 0.02f
#define KOOPA_SHELL_SPEED 0.25f
#define KOOPA_SHELL_DEFLECT_SPEED 0.25f

#define KOOPA_TEXTURE_WIDTH 16
#define KOOPA_TEXTURE_HEIGHT 26

#define KOOPA_BBOX_WIDTH 14
#define KOOPA_BBOX_HEIGHT 14

#define KOOPA_SHELL_TIMEOUT 7000
#define KOOPA_SHELL_ALERT_TIMEOUT 5000
#define KOOPA_DIE_TIMEOUT 700
#define KOOPA_TURNAROUND_TIMEOUT 200

#define KOOPA_STATE_WALKING_LEFT 100
#define KOOPA_STATE_WALKING_RIGHT 110
#define KOOPA_STATE_SHELL_STATIC 200
#define KOOPA_STATE_SHELL_DYNAMIC 300
#define KOOPA_STATE_BEING_HELD 400
#define KOOPA_STATE_DIE_ON_COLLIDE_WITH_ENEMY 500
#define KOOPA_STATE_DIE_ON_COLLIDE_WITH_TERRAIN 510
#define KOOPA_STATE_DIE_ON_COLLIDE_WITH_DYNAMIC_KOOPA 520
#define KOOPA_STATE_DIE_ON_COLLIDE_WITH_HELD_KOOPA 530

#define ID_ANI_KOOPA_WALKING_LEFT 150010
#define ID_ANI_KOOPA_WALKING_RIGHT 150020
#define ID_ANI_KOOPA_SHELL_STATIC_1 150100
#define ID_ANI_KOOPA_SHELL_STATIC_2 150101
#define ID_ANI_KOOPA_SHELL_DYNAMIC 150200
#define ID_ANI_KOOPA_SHELL_STATIC_REVERSE_1 150300
#define ID_ANI_KOOPA_SHELL_STATIC_REVERSE_2 150301
#define ID_ANI_KOOPA_SHELL_DYNAMIC_REVERSE 150400

class CKoopa : public CGameObject
{
protected:
	float ax;
	float ay;

	float x0;
	float y0;

	bool isReversed = false;
	bool isKicked = false;
	bool isFlying = false;

	ULONGLONG shellStart = -1;
	ULONGLONG dieStart = -1;
	ULONGLONG lastTurnAroundTime = -1;

	bool isHeld = 0;
	bool isDead = 0;
	bool isDefeated = 0;

	int originalChunkId;

	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();
	bool IsPlatformEdge(float checkDistance, vector<LPGAMEOBJECT>& possibleGrounds);

	virtual int IsCollidable() { return (isDead == 0 && isDefeated == 0); };
	virtual int IsBlocking() { return 0; }
	virtual void OnNoCollision(DWORD dt);

	virtual void OnCollisionWith(LPCOLLISIONEVENT e);
	void OnCollisionWithQuestionBlock(LPCOLLISIONEVENT e);
	void OnCollisionWithGoomba(LPCOLLISIONEVENT e);
	void OnCollisionWithWingedGoomba(LPCOLLISIONEVENT e);
	void OnCollisionWithPiranhaPlant(LPCOLLISIONEVENT e);
	void OnCollisionWithLifeBrick(LPCOLLISIONEVENT e);
	void OnCollisionWithCoinBrick(LPCOLLISIONEVENT e);
	void OnCollisionWithActivatorBrick(LPCOLLISIONEVENT e);
	void OnCollisionWithKoopa(LPCOLLISIONEVENT e);
	void OnCollisionWithWingedKoopa(LPCOLLISIONEVENT e);

	bool CheckCollisionWithTerrain(DWORD dt, vector<LPGAMEOBJECT>* coObjects);

public:
	CKoopa(int id, float x, float y, int z, int originalChunkId);
	virtual void SetState(int state);
	void StartShell();
	bool IsHeld() { return isHeld; }
	void SetIsHeld(int beingHeld) { this->isHeld = beingHeld; }
	void SetIsFlying(bool isFlying) { this->isFlying = isFlying; }
	void SetIsReversed(bool isReversed) { this->isReversed = isReversed; }
	void SetNx(int nx) { this->nx = nx; }
	void GetOriginalPosition(float& x0, float& y0) { x0 = this->x0; y0 = this->y0; }
	bool IsFlying() { return isFlying; }
	bool IsDefeated() { return isDefeated; }
	bool IsDead() { return isDead; }
	void SetIsDefeated(int isDefeated) { this->isDefeated = isDefeated; }
	int GetOriginalChunkId() { return originalChunkId; }
	int GetNx() { return nx; }
};

