#pragma once

#include "GameObject.h"
#include "Animation.h"
#include "Animations.h"

#define ID_ANI_MUSHROOM 110000

#define MUSHROOM_WIDTH 10
#define MUSHROOM_BBOX_WIDTH 14
#define MUSHROOM_BBOX_HEIGHT 16

#define MUSHROOM_GRAVITY 0.0003f
#define MUSHROOM_WALKING_SPEED 0.05f

#define MUSHROOM_STATE_NOT_HIT 100
#define MUSHROOM_STATE_BOUNCE_UP 200
#define MUSHROOM_STATE_BOUNCE_DOWN 300
#define MUSHROOM_STATE_RISE 400
#define MUSHROOM_STATE_MOVING 500

#define MUSHROOM_BOUNCE_OFFSET 16

#define MUSHROOM_BOUNCE_SPEED -0.1f
#define MUSHROOM_RISE_SPEED -0.01f


#define MUSHROOM_RISE_OFFSET 16

class CMushroom : public CGameObject {
protected:
    float ax;
    float ay;
	int bounceUp = 0;
	int bounceDown = 0;
    ULONGLONG bounceUpStart = -1;
    ULONGLONG bounceDownStart = -1;

    int rise = 0;
    float y0 = - 1;
    int collisionNx;
    int isVisible = 0;

public:
    CMushroom(float x, float y);
    void Render();
    void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
    void GetBoundingBox(float& l, float& t, float& r, float& b);
    int IsBlocking() { return 0; }
    int IsCollidable() { return 1; };
    void OnCollisionWith(LPCOLLISIONEVENT e);
    void OnNoCollision(DWORD dt);
    void SetState(int state);
	void StartBounceUp() { bounceUp = 1; bounceUpStart = GetTickCount64(); }
	void StartBounceDown() { bounceDown = 1; bounceDownStart = GetTickCount64(); }
	void StartRise() { rise = 1; }
	void SetCollisionNx(int collisionNx) { this->collisionNx = collisionNx; }
	void SetVisible(int isVisible) { this->isVisible = isVisible; }
};