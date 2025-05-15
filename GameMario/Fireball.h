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


#define INVERSE_SQUARE_ROOT_10 0.31622776601

#define FIREBALL_SPEED 0.1f

#define ID_ANI_FIREBALL_STATIC 140000
#define ID_ANI_FIREBALL_DYNAMIC 140001

#define DEPENDENT_ID 9999

class CFireball :
	public CGameObject
{
public:
	CFireball(int id, float x, float y, int z, int direction) : CGameObject(id, x, y, z)
	{
		switch (direction)
		{
		case 0:
			SetState(FIREBALL_STATE_SHOOT_BOTTOM_LEFT);
			//DebugOut(L"[INFO] Shoot left down sharp\n");
			break;
		case 1:
			SetState(FIREBALL_STATE_SHOOT_LOWER_CENTER_LEFT);
			//DebugOut(L"[INFO] Shoot left down shallow\n");
			break;
		case 2:
			SetState(FIREBALL_STATE_SHOOT_UPPER_CENTER_LEFT);
			//DebugOut(L"[INFO] Shoot left up shallow\n");
			break;
		case 3:
			SetState(FIREBALL_STATE_SHOOT_TOP_LEFT);
			//DebugOut(L"[INFO] Shoot left up sharp\n");
			break;
		case 4:
			SetState(FIREBALL_STATE_SHOOT_TOP_RIGHT);
			//DebugOut(L"[INFO] Shoot right up sharp\n");
			break;
		case 5:
			SetState(FIREBALL_STATE_SHOOT_UPPER_CENTER_RIGHT);
			//DebugOut(L"[INFO] Shoot right up shallow\n");
			break;
		case 6:
			SetState(FIREBALL_STATE_SHOOT_LOWER_CENTER_RIGHT);
			//DebugOut(L"[INFO] Shoot right down shallow\n");
			break;
		case 7:
			SetState(FIREBALL_STATE_SHOOT_BOTTOM_RIGHT);
			//DebugOut(L"[INFO] Shoot right down sharp\n");
			break;
		default:
			SetState(FIREBALL_STATE_STATIC);
			break;
		}
	}

	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();

	virtual void SetState(int state);
	virtual int IsBlocking() { return 0; }

	static void Shoot(LPGAMEOBJECT obj, int direction);
};

