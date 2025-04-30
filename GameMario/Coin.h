#pragma once

#include "GameObject.h"
#include "Animation.h"
#include "Animations.h"

#define ID_ANI_COIN_STATIC 1010000
#define ID_ANI_COIN_DYNAMIC 1011000

#define COIN_BBOX_WIDTH 14
#define COIN_BBOX_HEIGHT 14

#define COIN_STATE_STATIC 0
#define COIN_STATE_DYNAMIC 100
#define COIN_STATE_BOUNCE_UP 200
#define COIN_STATE_BOUNCE_DOWN 300
#define COIN_STATE_BOUNCE_COMPLETE 400

#define COIN_BOUNCE_OFFSET 64

#define COIN_BOUNCE_SPEED -0.3f

class CCoin : public CGameObject {
protected:
	int bounceUp = 0;
	int bounceDown = 0;
	int type; // 0: static, 1: dynamic
	int isVisible;
	float y0 = -1;
public:
	CCoin(float x, float y, int z, int type);
	void Render();
	void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	void GetBoundingBox(float& l, float& t, float& r, float& b);
	int IsBlocking() { return 0; }
	void SetState(int state);
	void StartBounceUp() { bounceUp = 1; }
	void StartBounceDown() { bounceDown = 1; }
	void SetVisible(int isVisible) { this->isVisible = isVisible; }
};