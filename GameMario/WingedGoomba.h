#pragma once
#include "GameObject.h"

#define WINGED_GOOMBA_GRAVITY					0.002f
#define WINGED_GOOMBA_WALKING_SPEED				0.05f

#define WINGED_GOOMBA_BBOX_WIDTH				16
#define WINGED_GOOMBA_BBOX_HEIGHT				14
#define WINGED_GOOMBA_BBOX_HEIGHT_DIE			7

#define WINGED_GOOMBA_DIE_TIMEOUT				500

#define WINGED_GOOMBA_STATE_WALKING				100
#define WINGED_GOOMBA_STATE_DIE_ON_STOMP		200
#define WINGED_GOOMBA_STATE_DIE_ON_TAIL_WHIP	300

#define ID_ANI_WINGED_GOOMBA_WALKING			170000
#define ID_ANI_WINGED_GOOMBA_DIE_ON_STOMP		171000
#define ID_ANI_WINGED_GOOMBA_DIE_ON_TAIL_WHIP	171100

class CWingedGoomba : public CGameObject
{
protected:
	float ax;
	float ay;

	int isWinged = 0;

	ULONGLONG dieOnStompStart = -1;
	ULONGLONG dieOnWhipStart = -1;

	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();

	virtual int IsCollidable() { return (state != WINGED_GOOMBA_STATE_DIE_ON_TAIL_WHIP) ? 1 : 0; };
	virtual int IsBlocking() { return 0; }
	virtual void OnNoCollision(DWORD dt);

	virtual void OnCollisionWith(LPCOLLISIONEVENT e);

public:
	CWingedGoomba(float x, float y);
	virtual void SetState(int state);
};