#pragma once
#include "GameObject.h"

#include <vector>
#include <list>

#define QUESTIONBLOCK_BBOX_WIDTH  14
#define QUESTIONBLOCK_BBOX_HEIGHT 16

#define ID_ANI_QUESTIONBLOCK 1030000
#define ID_ANI_QUESTIONBLOCK_HIT 1031000

#define QUESTIONBLOCK_STATE_NOT_HIT 100
#define QUESTIONBLOCK_STATE_BOUNCE_UP 200
#define QUESTIONBLOCK_STATE_BOUNCE_DOWN 300
#define QUESTIONBLOCK_STATE_BOUNCE_COMPLETE 400

#define QUESTIONBLOCK_BOUNCE_OFFSET 16

#define QUESTIONBLOCK_BOUNCE_SPEED -0.1f

class CQuestionBlock : public CGameObject
{
protected:
	bool isHit = false;
	int bounceUp = 0;
	int bounceDown = 0;
	ULONGLONG bounceUpStart = -1;
	ULONGLONG bounceDownStart = -1;
	float y0 = -1;

public:
	CQuestionBlock(float x, float y, int z);

	void Render();
	void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	void GetBoundingBox(float& l, float& t, float& r, float& b);
	virtual void SetState(int state);
	virtual void StartBounceUp() { bounceUp = 1; bounceUpStart = GetTickCount64(); }
	virtual void StartBounceDown() { bounceDown = 1; bounceDownStart = GetTickCount64(); }
};

