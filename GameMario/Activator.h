#pragma once
#include "GameObject.h"

#define ID_ANI_ACTIVATOR_UNACTIVATED 1200000
#define ID_ANI_ACTIVATOR_ACTIVATED 1200100

#define ACTIVATOR_CELL_WIDTH 16
#define ACTIVATOR_CELL_HEIGHT 16

#define ACTIVATOR_STATE_UNACTIVATED 0
#define ACTIVATOR_STATE_ACTIVATED 100
#define ACTIVATOR_STATE_ACTIVATED_BY_CHUNK_RELOAD 110
#define ACTIVATOR_STATE_REVEALED 200

class CActivator : public CGameObject
{
protected:
	int originalChunkId;
	bool isActivated = false;
	bool isRevealed = false;
	float y0;

public:
	CActivator(int id, float x, float y, int z, int originalChunkId);

	void Render();
	void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	void GetBoundingBox(float& l, float& t, float& r, float& b);
	int IsBlocking() { return (!isActivated && isRevealed); }
	int IsCollidable() { return (!isActivated && isRevealed); };
	void SetState(int state);
	void SetIsRevealed(bool isRevealed) { this->isRevealed = isRevealed; }
	bool IsActivated() { return isActivated; }
	bool IsRevealed() { return isRevealed; }
};

