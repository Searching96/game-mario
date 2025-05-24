#pragma once

#include "GameObject.h"
#include "Mario.h"

/*
	Object that triggers scene switching
*/
class CPortal : public CGameObject
{
	float targetX;
	float exitY;
	float yLevel;
	int enterDirection = 0; // 0 for down, 1 for up
	int exitDirection = 0; // 0 for down, 1 for up

	float width;
	float height;
public:
	CPortal(int id, float x, float y, float width, float height, int z, float targetX, float exitY, int enterDirection, int exitDirection, float yLevel);
	virtual void Render();
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
	void RenderBoundingBox(void);

	float GetTargetX() const { return targetX; }
	float GetExitY() const { return exitY; }
	float GetYLevel() const { return yLevel; }
	// 0 for down, 1 for up
	int GetEnterDirection() const { return enterDirection; }
	// 0 for down, 1 for up
	int GetExitDirection() const { return exitDirection; }

	void Teleport(CMario* mario);
	int IsBlocking() const { return 0; }
};