#pragma once
#include "GameObject.h"

#define FIREBALL_BBOX_WIDTH 8
#define FIREBALL_BBOX_HEIGHT 8

#define FIREBALL_RANGE_WIDTH 160
#define FIREBALL_RANGE_HEIGHT 160

#define FIREBALL_STATE_STATIC 0
#define FIREBALL_STATE_SHOOT_TOP_RIGHT 110
#define FIREBALL_STATE_SHOOT_TOP_LEFT 120
#define FIREBALL_STATE_SHOOT_UPPER_CENTER_LEFT 210
#define FIREBALL_STATE_SHOOT_UPPER_CENTER_RIGHT 220
#define FIREBALL_STATE_SHOOT_LOWER_CENTER_LEFT 310
#define FIREBALL_STATE_SHOOT_LOWER_CENTER_RIGHT 320
#define FIREBALL_STATE_SHOOT_BOTTOM_RIGHT 410
#define FIREBALL_STATE_SHOOT_BOTTOM_LEFT 420


#define SQUARE_ROOT_25_29 0.92847669089f
#define SQUARE_ROOT_4_29 0.37139067635f

#define FIREBALL_SPEED 0.1f

#define ID_ANI_FIREBALL_STATIC 140000
#define ID_ANI_FIREBALL_DYNAMIC 140001

class CFireball :
	public CGameObject
{
protected:
	int x0, y0;
	int isEnabled = 0;
public:
	CFireball(int id, float x, float y, int z) : CGameObject(id, x, y, z)
	{
		this->x0 = x;
		this->y0 = y;
		SetState(FIREBALL_STATE_STATIC);
	}

	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();

	virtual void SetState(int state);
	virtual int IsCollidable() { return isEnabled; };
	virtual int IsBlocking() { return 0; }
	void SetEnable(int enable) { isEnabled = enable; }
};

