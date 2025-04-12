#pragma once
#include "GameObject.h"

#include <vector>
#include <list>

#define QUESTIONBLOCK_BBOX_WIDTH  16
#define QUESTIONBLOCK_BBOX_HEIGHT 16

#define ID_ANI_QUESTIONBLOCK 1030000
#define ID_ANI_QUESTIONBLOCK_HIT 1031000

#define QUESTIONBLOCK_STATE_NOT_HIT 100
#define QUESTIONBLOCK_STATE_BOUNCE_UP 200
#define QUESTIONBLOCK_STATE_BOUNCE_DOWN 300
#define QUESTIONBLOCK_STATE_BOUNCE_COMPLETE 400

#define QUESTIONBLOCK_BOUNCE_UP_TIME 200
#define QUESTIONBLOCK_BOUNCE_DOWN_TIME 200

extern list<LPGAMEOBJECT> objects;

class CQuestionBlock : public CGameObject
{
protected:
	bool isHit = false;
	int bounceUp = 0;
	int bounceDown = 0;
	int bounceUpStart = -1;
	int bounceDownStart = -1;

public:
	CQuestionBlock(float x, float y);

	void Render();
	void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	void GetBoundingBox(float& l, float& t, float& r, float& b);
	void SetState(int state);
	void StartBounceUp() { bounceUp = 1; bounceUpStart = GetTickCount64(); }
	void StartBounceDown() { bounceDown = 1; bounceDownStart = GetTickCount64(); }
	//void OnCollisionWith(LPCOLLISIONEVENT e);
};

