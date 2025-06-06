#pragma once
#include "GameObject.h"
class CBorder :
	public CGameObject
{
private:
	float width;
	float height;
public:
	CBorder(int id, float x, float y, float width, float height);
	virtual void Render();
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
	void RenderBoundingBox();

	int IsBlocking() const { return 1; }
};

typedef CBorder* LPBORDER;

