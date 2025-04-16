#pragma once
#include "GameObject.h"

#define FIREBALL_BBOX_WIDTH 8
#define FIREBALL_BBOX_HEIGHT 8

#define FIREBALL_RANGE_WIDTH 160
#define FIREBALL_RANGE_HEIGHT 160

#define FIREBALL_STATE_SHOOT_TOP_RIGHT 100
#define FIREBALL_STATE_SHOOT_TOP_LEFT 200
#define FIREBALL_STATE_SHOOT_BOTTOM_LEFT 300
#define FIREBALL_STATE_SHOOT_BOTTOM_RIGHT 400
#define FIREBALL_STATE_SHOOT_COMPLETE 500
#define FIREBALL_STATE_ASCEND 600
#define FIREBALL_STATE_DESCEND 700
#define FIREBALL_STATE_STATIC 0

#define FIREBALL_SPEED 0.1f

#define ID_ANI_FIREBALL_STATIC 140000
#define ID_ANI_FIREBALL_DYNAMIC 140001

class CFireball :
	public CGameObject
{
protected:
	int x0, y0;

public:
	CFireball(float x, float y) :CGameObject(x, y)
	{
		this->x0 = x;
		this->y0 = y;
		SetState(FIREBALL_STATE_STATIC);
	}

	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();

	virtual void SetState(int state);
	virtual int IsCollidable() { return 1; };
	virtual int IsBlocking() { return 0; }
	//virtual void OnNoCollision(DWORD dt);


	//virtual void OnCollisionWith(LPCOLLISIONEVENT e);
};

