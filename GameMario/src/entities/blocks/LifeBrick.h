#pragma once
#include "entities/blocks/QuestionBlock.h"
#include "entities/items/LifeMushroom.h"
#include "entities/blocks/Brick.h"

class CLifeBrick : public CGameObject
{
protected:
	CLifeMushroom* mushroom;
	bool isHit = false;
	int bounceUp = 0;
	int bounceDown = 0;
	ULONGLONG bounceUpStart = -1;
	ULONGLONG bounceDownStart = -1;
	float y0 = -1;
	int originalChunkId;

public:
	CLifeBrick(int id, float x, float y, int z, int originalChunkId, CLifeMushroom* mushroom);

	void Render();
	void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	void GetBoundingBox(float& l, float& t, float& r, float& b);
	void SetState(int state);
	void StartBounceUp() { bounceUp = 1; bounceUpStart = GetTickCount64(); }
	void StartBounceDown() { bounceDown = 1; bounceDownStart = GetTickCount64(); }
	bool IsHit() { return isHit; }
	void SetIsHit(bool isHit) { this->isHit = isHit; }
	CLifeMushroom* GetLifeMushroom() { return mushroom; }
};