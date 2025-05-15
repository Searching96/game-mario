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

	float width;
	float height;
public:
	CPortal(int id, float x, float y, float width, float height, int z, float targetX, float exitY, float yLevel);
	virtual void Render();
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
	void RenderBoundingBox(void);

	void Teleport(CMario* mario);
	int IsBlocking() const { return 0; }
};