#pragma once
#include "QuestionBlock.h"
#include "Brick.h"

#define ID_ANI_QUESTIONBLOCK_HIT 1031000
#define ID_ANI_STATIC_HIDDEN_COIN 1012000

#define COIN_BRICK_STATE_NOT_REVEALED 0
#define COIN_BRICK_STATE_BREAK 100
#define COIN_BRICK_STATE_REVEALED 200
#define COIN_BRICK_STATE_CONSUMED 300

#define COIN_BRICK_REVEAL_TIMEOUT 1500000

class CCoinBrick : public CGameObject
{
protected:
	int originalChunkId;
	bool isRevealed = false;

	ULONGLONG revealStart = -1;
public:
	CCoinBrick(int id, float x, float y, int z, int originalChunkId);

	void Render();
	void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	void GetBoundingBox(float& l, float& t, float& r, float& b);
	void SetState(int state);
	int IsBlocking() { return (state != COIN_BRICK_STATE_REVEALED); }
	void StartReveal() { isRevealed = true; revealStart = GetTickCount64(); }

	bool IsRevealed() { return isRevealed; }
};