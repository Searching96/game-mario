#pragma once
#include "GameObject.h"

#define WINGED_GOOMBA_GRAVITY					0.002f
#define WINGED_GOOMBA_WALKING_SPEED				0.05f
#define WINGED_GOOMBA_BOUNCE_SPEED				0.135f

#define WINGED_GOOMBA_BBOX_WIDTH				16
#define WINGED_GOOMBA_BBOX_HEIGHT				14
#define WINGED_GOOMBA_BBOX_HEIGHT_DIE			7

#define WINGED_GOOMBA_DIE_TIMEOUT				500

#define WINGED_GOOMBA_STATE_WALKING				100
#define WINGED_GOOMBA_STATE_DIE_ON_STOMP		200
#define WINGED_GOOMBA_STATE_DIE_ON_TAIL_WHIP	300
#define WINGED_GOOMBA_STATE_TRACKING			400
#define WINGED_GOOMBA_STATE_BOUNCING			500
#define WINGED_GOOMBA_STATE_FLYING				600

#define ID_ANI_WINGED_GOOMBA_WALKING			170000
#define ID_ANI_WINGED_GOOMBA_DIE_ON_STOMP		171000
#define ID_ANI_WINGED_GOOMBA_DIE_ON_TAIL_WHIP	171100

#define ID_ANI_WING_FLAP_LEFT		160000
#define ID_ANI_WING_CLOSE_LEFT		160100
#define ID_ANI_WING_FLAP_RIGHT		160010
#define ID_ANI_WING_CLOSE_RIGHT		160110

#define WINGED_GOOMBA_TRACKING_TIME				1000
#define WINGED_GOOMBA_BOUNCING_TIME				1000
#define WINGED_GOOMBA_FLYING_TIME				1000

#define WINGED_GOOMBA_MAX_BOUNCE_COUNT			4

class CWingedGoomba : public CGameObject
{
protected:
	float ax;
	float ay;

	int wingState = 0; // 0: flap, 1: fold

	ULONGLONG dieOnStompStart = -1;
	ULONGLONG dieOnWhipStart = -1;
	ULONGLONG flapStart = -1;

	int isWinged = 1;
	int isTracking = 0;
	int isBouncing = 0;
	int isFlying = 0;
	ULONGLONG trackingStart = -1;
	ULONGLONG flyingStart = -1;

	int bounceCount = 0;

	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();

	virtual int IsCollidable() { return (state != WINGED_GOOMBA_STATE_DIE_ON_TAIL_WHIP) ? 1 : 0; };
	virtual int IsBlocking() { return 0; }
	virtual void OnNoCollision(DWORD dt);

	virtual void OnCollisionWith(LPCOLLISIONEVENT e);

public:
	CWingedGoomba(float x, float y, int z);
	virtual void SetState(int state);
	void StartTracking() { isTracking = 1; trackingStart = GetTickCount64(); }
	void StartBouncing() { isBouncing = 1; bounceCount++; }
	void StartFlying() { isFlying = 1; flyingStart = GetTickCount64(); }
	void SetWinged(int winged) { isWinged = winged; }
	int GetWinged() { return isWinged; }
};