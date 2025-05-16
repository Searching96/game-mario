#pragma once
#include "GameObject.h"

#define ID_ANI_ACTIVATOR_UNACTIVATED 1120000
#define ID_ANI_ACTIVATOR_ACTIVATED 1120100

#define COIN_BRICK_STATE_BREAK 100

#define ACTIVATOR_CELL_WIDTH 16
#define ACTIVATOR_CELL_HEIGHT 16

class CActivator : public CGameObject
{
protected:
	int originalChunkId;
	bool isActivated = false;

public:
	CActivator(int id, float x, float y, int z, int originalChunkId);

	void Render();
	void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	void GetBoundingBox(float& l, float& t, float& r, float& b);
	void SetState(int state);
	bool IsActivated() { return isActivated; }
};

