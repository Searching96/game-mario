#pragma once
#include "GameObject.h"

class CFallPitch : public CGameObject
{
	float width;
	float height; 
public:
	CFallPitch(float l, float t, float r, float b);
	virtual void Render();
	virtual void GetBoundingBox(float &l, float &t, float &r, float &b);
	void RenderBoundingBox(void);

	int IsBlocking() { return 0; }
};

