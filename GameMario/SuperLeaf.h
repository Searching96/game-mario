#pragma once

#include "GameObject.h"
#include "Animation.h"
#include "Animations.h"

#define ID_ANI_SUPERLEAF_RIGHT 120000
#define ID_ANI_SUPERLEAF_LEFT 120010

#define SUPERLEAF_WIDTH 16
#define SUPERLEAF_BBOX_WIDTH 16
#define SUPERLEAF_BBOX_HEIGHT 16

#define SUPERLEAF_BOUNCE_UP_SPEED -0.2f
#define SUPERLEAF_FLOATING_X_SPEED 0.05f
#define SUPERLEAF_FLOATING_Y_SPEED 0.01f

#define SUPERLEAF_STATE_NOT_HIT 100
#define SUPERLEAF_STATE_BOUNCE_UP 200
#define SUPERLEAF_STATE_FLOATING_RIGHT 300
#define SUPERLEAF_STATE_FLOATING_LEFT 400

#define SUPERLEAF_BOUNCE_UP_TIME 200
#define SUPERLEAF_FLOATING_TIME 600

class CSuperLeaf : public CGameObject {
protected:
    float ax;
    float ay;

	int bounceUp = 0;
	int floatingRight = 0;
	int floatingLeft = 0;
	ULONGLONG bounceUpStart = -1;
	ULONGLONG floatingRightStart = -1;
	ULONGLONG floatingLeftStart = -1;

	int isVisible = 0;

public:
    CSuperLeaf(int id, float x, float y, int z);
    void Render();
    void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
    void GetBoundingBox(float& l, float& t, float& r, float& b);
    int IsBlocking() { return 0; }
    int IsCollidable() { return 1; };
    void OnCollisionWith(LPCOLLISIONEVENT e);
    void OnNoCollision(DWORD dt);
    void SetState(int state);
	void StartBounceUp() { bounceUp = 1; bounceUpStart = GetTickCount64(); }
	void StartFloatingRight() { floatingRight = 1; floatingRightStart = GetTickCount64(); }
	void StartFloatingLeft() { floatingLeft = 1; floatingLeftStart = GetTickCount64(); }
	void SetVisible(int isVisible) { this->isVisible = isVisible; }
};