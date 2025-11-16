#pragma once
#include "entities/blocks/QuestionBlock.h"
#include "entities/blocks/Brick.h"
#include "entities/blocks/Activator.h"

#define ACTIVATOR_BRICK_STATE_NOT_HIT 0
#define ACTIVATOR_BRICK_STATE_BOUNCE_UP 100
#define ACTIVATOR_BRICK_STATE_BOUNCE_DOWN 200
#define ACTIVATOR_BRICK_STATE_BOUNCE_COMPLETE 300

class CActivatorBrick : public CGameObject
{
protected:

	int originalChunkId;
	CActivator* activator;
	bool isHit = false;
	bool bounceUp = false;
	bool bounceDown = false;
	ULONGLONG bounceUpStart = -1;
	ULONGLONG bounceDownStart = -1;
	float y0 = -1;

public:
	CActivatorBrick(int id, float x, float y, int z, int originalChunkId, CActivator* activator);

	void Render();
	void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	void GetBoundingBox(float& l, float& t, float& r, float& b);
	void StartBounceUp() { bounceUp = true; bounceUpStart = GetTickCount64(); }
	void StartBounceDown() { bounceDown = true; bounceDownStart = GetTickCount64(); }
	void SetState(int state);
	void SetIsHit(bool isHit) { this->isHit = isHit; }
	int IsBlocking() { return 1; }

	bool IsHit() { return isHit; }
};