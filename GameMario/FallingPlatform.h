#pragma once
#include "GameObject.h"

#define ID_SPRITE_FALLING_PLATFORM_LEFT 160010
#define ID_SPRITE_FALLING_PLATFORM_CENTER 160020
#define ID_SPRITE_FALLING_PLATFORM_RIGHT 160030

#define FALLING_PLATFORM_CELL_HEIGHT 16
#define FALLING_PLATFORM_CELL_WIDTH 16

#define FALLING_PLATFORM_TIMEOUT 1000

#define FALLING_PLATFORM_STATE_ACTIVE 100
#define FALLING_PLATFORM_STATE_INACTIVE 200
#define FALLING_PLATFORM_STATE_COMPLETE 300

class CFallingPlatform :
	public CGameObject
{
private:
	float ay;
	int width;
	float x0, y0; // initial position
	ULONGLONG fallStart = -1;
	bool isDefeated = false;
	int originalChunkId = -1;

public:
	CFallingPlatform(int id, float x, float y, int z, int originalChunkId, int width);
	void Render();
	void Update(DWORD dt,vector<LPGAMEOBJECT> *coObjects);


	void GetBoundingBox(float& l, float& t, float& r, float& b);
	void RenderBoundingBox();
	void GetOriginalPosition(float& x0, float& y0) { x0 = this->x0; y0 = this->y0; }
	int IsBlocking() const { return 1; }
	bool IsDefeated() const { return isDefeated; }
	void SetIsDefeated(bool isDefeated) { this->isDefeated = isDefeated; }
	int GetOriginalChunkId() const { return originalChunkId; }
	int GetWidth() const { return width; }

	int IsDirectionCollidable(float nx, float ny) const { return 1; }

	void SetState(int state);
};

