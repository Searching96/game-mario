#pragma once
#include "GameObject.h"

#define GOOMBA_GRAVITY 0.002f
#define GOOMBA_WALKING_SPEED 0.05f


#define GOOMBA_BBOX_WIDTH 16
#define GOOMBA_BBOX_HEIGHT 14
#define GOOMBA_BBOX_HEIGHT_DIE 7

#define GOOMBA_DIE_TIMEOUT 500

#define GOOMBA_STATE_WALKING			100
#define GOOMBA_STATE_DIE_ON_STOMP		200
#define GOOMBA_STATE_DIE_ON_TAIL_WHIP	300
#define GOOMBA_STATE_DIE_ON_HELD_KOOPA	400

#define ID_ANI_GOOMBA_WALKING			100000
#define ID_ANI_GOOMBA_DIE_ON_STOMP		101000
#define ID_ANI_GOOMBA_DIE_ON_TAIL_WHIP	101100


class CGoomba : public CGameObject
{
protected:
	float ax;				
	float ay; 

	ULONGLONG dieStart = -1;

	ULONGLONG dieOnStompStart = -1;
	ULONGLONG dieOnWhipStart = -1;

	int isDead = 0;

	virtual void GetBoundingBox(float &left, float &top, float &right, float &bottom);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT> *coObjects);
	virtual void Render();

	virtual int IsCollidable() { return (isDead == 0); }
	virtual int IsBlocking() { return 0; }

	virtual void OnNoCollision(DWORD dt);
	virtual void OnCollisionWith(LPCOLLISIONEVENT e);
	void OnCollisionWithKoopa(LPCOLLISIONEVENT e);

public: 	
	CGoomba(int id, float x, float y, int z);
	virtual void SetState(int state);
	int GetIsDead() { return isDead; }
};