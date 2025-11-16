#pragma once
#include "core/GameObject.h"
#include "entities/blocks/QuestionBlock.h"
#include "entities/items/Coin.h"

#include <vector>

#define HIDDEN_COIN_BRICK_NUM_COINS 10

class CHiddenCoinBrick : public CGameObject
{
protected:
	vector<CCoin*> coinList;
	bool isHit = false;
	int bounceUp = 0;
	int bounceDown = 0;
	ULONGLONG bounceUpStart = -1;
	ULONGLONG bounceDownStart = -1;
	float y0 = -1;
	int originalChunkId;
	int remainingCoins;

public:
	CHiddenCoinBrick(int id, float x, float y, int z, int originalChunkId, vector<CCoin*> coinList);

	void Render();
	void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	void GetBoundingBox(float& l, float& t, float& r, float& b);
	void SetState(int state);
	void StartBounceUp() { bounceUp = 1; bounceUpStart = GetTickCount64(); }
	void StartBounceDown() { bounceDown = 1; bounceDownStart = GetTickCount64(); }
	bool IsHit() { return isHit; }
	void SetIsHit(bool isHit) { this->isHit = isHit; }
	int GetBounceUp() { return bounceUp; }
};

