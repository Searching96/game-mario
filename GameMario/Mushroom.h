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
#define MUSHROOM_STATE_HIT 200

#define MUSHROOM_SPAWN_TIME 2000

class CMushroom : public CGameObject {
protected:
    float ax;
    float ay;
	ULONGLONG spawn_start;
    int spawned;
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
    void SpawnStart() { spawned = 1; spawn_start = GetTickCount64(); }
};