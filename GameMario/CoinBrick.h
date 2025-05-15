#pragma once
#include "QuestionBlock.h"
#include "Brick.h"

#include <vector>
#include <list>

#define ID_ANI_QUESTIONBLOCK_HIT 1031000

#define COIN_BRICK_STATE_BREAK 100

class CCoinBrick : public CGameObject
{
protected:
	int originalChunkId;

public:
	CCoinBrick(int id, float x, float y, int z, int originalChunkId);

	void Render();
	void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	void GetBoundingBox(float& l, float& t, float& r, float& b);
	void SetState(int state);
};