#pragma once

#include "GameObject.h"
#include "Animation.h"
#include "Animations.h"

#define WING_BBOX_WIDTH			0
#define WING_BBOX_HEIGHT		0

#define WING_STATE_FLAP			0
#define WING_STATE_CLOSE		100

#define ID_ANI_WING_FLAP_LEFT		160000
#define ID_ANI_WING_CLOSE_LEFT		160100
#define ID_ANI_WING_FLAP_RIGHT		160010
#define ID_ANI_WING_CLOSE_RIGHT		160110

class CWing : public CGameObject
{
protected:
	int side; // 0: left, 1: right
	int killed = 0;
public:
	CWing(float x, float y, int side);
	void Render();
	void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	void GetBoundingBox(float& l, float& t, float& r, float& b);
	int IsCollidable() { return 0; };
	int IsBlocking() { return 0; }
	void SetState(int state);
	void SetKilled(int killed) { this->killed = killed; }
	int GetKilled() { return killed; }
};