#pragma once
#include "GameObject.h"

#define FLYING_KOOPA_STATE_FLYING_DOWN	1000
#define FLYING_KOOPA_STATE_FLYING_UP	1100

#define FLYING_KOOPA_MOVE_OFFSET		108.0f

class CFlyingKoopa : public CGameObject
{
protected:
	float ax;
	float ay;

	float x0;
	float y0;

	float y00 = -1;

	bool isReversed = false;
	bool isKicked = false;
	bool isFlying = false;
	
	bool isWinged = true;
	int wingState = 0;

	ULONGLONG shellStart = -1;
	ULONGLONG dieStart = -1;
	ULONGLONG flapStart = -1;

	bool isHeld = false;
	bool isDead = false;
	bool isDefeated = false;

	int originalChunkId;

	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();

	virtual int IsCollidable() { return (!isDead && !isDefeated); };
	virtual int IsBlocking() { return 0; }
	
	virtual void OnNoCollision(DWORD dt);
	virtual void OnCollisionWith(LPCOLLISIONEVENT e);
	void OnCollisionWithKoopa(LPCOLLISIONEVENT e);

public:
	CFlyingKoopa(int id, float x, float y, int z, int originalChunkId);
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
	bool IsWinged() { return isWinged; }
	void SetIsWinged(bool isWinged) { this->isWinged = isWinged; }
};

