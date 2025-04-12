#pragma once

#include "GameObject.h"
#include "Animation.h"
#include "Animations.h"

#define ID_ANI_MUSHROOM 110000

#define MUSHROOM_WIDTH 10
#define MUSHROOM_BBOX_WIDTH 10
#define MUSHROOM_BBOX_HEIGHT 10

#define MUSHROOM_GRAVITY 0.0003f
#define MUSHROOM_WALKING_SPEED 0.05f

#define MUSHROOM_STATE_NOT_HIT 100
#define MUSHROOM_STATE_BOUNCE_UP 200
#define MUSHROOM_STATE_BOUNCE_DOWN 300
#define MUSHROOM_STATE_BOUNCE_COMPLETE 400

#define MUSHROOM_BOUNCE_UP_TIME 200
#define MUSHROOM_BOUNCE_DOWN_TIME 200

class CMushroom : public CGameObject {
protected:
    float ax;
    float ay;
    int spawned = 0;
	int bounceUp = 0;
	int bounceDown = 0;
    ULONGLONG bounceUpStart = -1;
    ULONGLONG bounceDownStart = -1;
    ULONGLONG spawnStart = -1;
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
    void StartSpawn() { spawned = 1; spawnStart = GetTickCount64(); }
	void StartBounceUp() { bounceUp = 1; bounceUpStart = GetTickCount64(); }
	void StartBounceDown() { bounceDown = 1; bounceDownStart = GetTickCount64(); }
};