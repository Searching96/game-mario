#pragma once
#include "GameObject.h"
class CParticle :
    public CGameObject
{
protected:
	int isEmerging = 0;
	int point;
	ULONGLONG emergingStart = -1;
public:
	CParticle(float x, float y, int z, int point);
	virtual ~CParticle() {}
	void Render();
	void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects = nullptr);
	void GetBoundingBox(float& l, float& t, float& r, float& b);
	int IsCollidable() { return 0; };
	int IsBlocking() { return 0; }
	void SetState(int state);
	void StartEmerging() { isEmerging = 1; emergingStart = GetTickCount64(); }
};

