#pragma once

#include "GameObject.h"
#include "Animation.h"
#include "Animations.h"

#define ID_ANI_TAIL_WHIP 20000

#define TAIL_WHIP_BBOX_WIDTH 14
#define TAIL_WHIP_BBOX_HEIGHT 8

#define COIN_STATE_DYNAMIC 100

#define COIN_BOUNCE_UP_TIME 200
#define COIN_BOUNCE_DOWN_TIME 200

class CTailWhip : public CGameObject 
{
protected:
	int bounceUp = 0;
	int bounceDown = 0;
	ULONGLONG bounceUpStart = -1;
	ULONGLONG bounceDownStart = -1;
	int type; // 0: static, 1: dynamic
public:
	CTailWhip(float x, float y);
	void Render();
	void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	void GetBoundingBox(float& l, float& t, float& r, float& b);
	int IsBlocking() { return 0; }
	void SetState(int state);
	void StartBounceUp() { bounceUp = 1; bounceUpStart = GetTickCount64(); }
	void StartBounceDown() { bounceDown = 1; bounceDownStart = GetTickCount64(); }
};