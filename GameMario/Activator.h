#pragma once
#include "GameObject.h"

#define ID_ANI_ACTIVATOR_UNACTIVATED 1120000
#define ID_ANI_ACTIVATOR_ACTIVATED 1120100

#define ACTIVATOR_CELL_WIDTH 16
#define ACTIVATOR_CELL_HEIGHT 16

#define ACTIVATOR_STATE_UNACTIVATED 0
#define ACTIVATOR_STATE_ACTIVATED 100

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
	int IsBlocking() { return (state != ACTIVATOR_STATE_ACTIVATED); }
	int IsCollidable() { return (state != ACTIVATOR_STATE_ACTIVATED); };
	void SetState(int state);
	bool IsActivated() { return isActivated; }
};

