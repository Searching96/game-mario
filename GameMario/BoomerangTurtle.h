#pragma once
#include "GameObject.h"

#define BOOMERANG_TURTLE_GRAVITY 0.002f
#define BOOMERANG_TURTLE_WALKING_SPEED 0.02f

#define BOOMERANG_TURTLE_BBOX_WIDTH 20
#define BOOMERANG_TURTLE_BBOX_HEIGHT 14

#define BOOMERANG_TURTLE_DIE_TIMEOUT 700

#define BOOMERANG_TURTLE_WALKING_LEFT		100
#define BOOMERANG_TURTLE_WALKING_RIGHT		200
#define BOOMERANG_TURTLE_DIE_ON_STOMP		300
#define BOOMERANG_TURTLE_DIE_ON_TAIL_WHIP	400

#define ID_ANI_GOOMBA_WALKING			100000
#define ID_ANI_GOOMBA_DIE_ON_STOMP		101000
#define ID_ANI_GOOMBA_DIE_ON_TAIL_WHIP	101100


class CGoomba : public CGameObject
{
protected:
	float ax;
	float ay;

	float x0;
	float y0;

	ULONGLONG dieStart = -1;
	ULONGLONG dieOnStompStart = -1;
	ULONGLONG dieOnWhipStart = -1;

	bool isDead = false;
	bool isDefeated = false;

	int originalChunkId;

	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();

	virtual int IsCollidable() { return (!isDead && !isDefeated); }
	virtual int IsBlocking() { return 0; }

	virtual void OnNoCollision(DWORD dt);
	virtual void OnCollisionWith(LPCOLLISIONEVENT e);
	void OnCollisionWithKoopa(LPCOLLISIONEVENT e);
	void OnCollisionWithWingedGoomba(LPCOLLISIONEVENT e);

public:
	CGoomba(int id, float x, float y, int z, int originalChunkId);
	virtual void SetState(int state);
	int IsDead() { return isDead; }
	int IsDefeated() { return isDefeated; }
	void SetIsDefeated(int defeated) { isDefeated = defeated; }
	void GetOriginalPosition(float& x0, float& y0) { x0 = this->x0; y0 = this->y0; }
	int GetOriginalChunkId() { return originalChunkId; }
};
