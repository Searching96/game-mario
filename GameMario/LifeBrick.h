#pragma once
#include "QuestionBlock.h"
#include "LifeMushroom.h"

#include <vector>
#include <list>

#pragma once


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

class CLifeBrick :
    public CQuestionBlock
{
protected:
	CLifeMushroom* mushroom;
	int toSpawn; // 0: mushroom, 1: superleaf
	bool isHit = false;
	int bounceUp = 0;
	int bounceDown = 0;
	ULONGLONG bounceUpStart = -1;
	ULONGLONG bounceDownStart = -1;
	float y0 = -1;

public:
	CLifeBrick(float x, float y, CLifeMushroom* mushroom);

	void Render();
	void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	void GetBoundingBox(float& l, float& t, float& r, float& b);
	void SetState(int state);
	void StartBounceUp() { bounceUp = 1; bounceUpStart = GetTickCount64(); }
	void StartBounceDown() { bounceDown = 1; bounceDownStart = GetTickCount64(); }
	void SetToSpawn(int toSpawn) { this->toSpawn = toSpawn; }
	CLifeMushroom* GetLifeMushroom() { return mushroom; }
};



