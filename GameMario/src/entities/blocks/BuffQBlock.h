#pragma once
#include "core/GameObject.h"
#include "entities/blocks/QuestionBlock.h"
#include "entities/items/Mushroom.h"
#include "entities/items/SuperLeaf.h"

class CBuffQBlock : public CGameObject
{
protected:
	CMushroom* mushroom;
	CSuperLeaf* superleaf;
	int toSpawn; // 0: mushroom, 1: superleaf
	bool isHit = false;
	int bounceUp = 0;
	int bounceDown = 0;
	ULONGLONG bounceUpStart = -1;
	ULONGLONG bounceDownStart = -1;
	float y0 = -1;
	int originalChunkId;

public:
	CBuffQBlock(int id, float x, float y, int z, int originalChunkId, CMushroom* mushroom, CSuperLeaf* superleaf);

	void Render();
	void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	void GetBoundingBox(float& l, float& t, float& r, float& b);
	void SetState(int state);
	void StartBounceUp() { bounceUp = 1; bounceUpStart = GetTickCount64(); }
	void StartBounceDown() { bounceDown = 1; bounceDownStart = GetTickCount64(); }
	void SetToSpawn(int toSpawn) { this->toSpawn = toSpawn; }
	bool IsHit() { return isHit; }
	void SetIsHit(bool isHit) { this->isHit = isHit; }
	CMushroom* GetMushroom() { return mushroom; }
};

