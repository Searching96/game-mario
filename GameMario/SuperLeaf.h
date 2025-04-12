#pragma once

#include "GameObject.h"
#include "Animation.h"
#include "Animations.h"

#define ID_ANI_SUPERLEAF 120000

#define SUPERLEAF_WIDTH 16
#define SUPERLEAF_BBOX_WIDTH 16
#define SUPERLEAF_BBOX_HEIGHT 16

#define SUPERLEAF_GRAVITY 0.0002f
#define SUPERLEAF_FLOATING_SPEED 0.05f

class CSuperLeaf : public CGameObject {
protected:
    float ax;
    float ay;
public:
    CSuperLeaf(float x, float y);
    void Render();
    void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
    void GetBoundingBox(float& l, float& t, float& r, float& b);
    int IsBlocking() { return 0; }
    int IsCollidable() { return 1; };
    void OnCollisionWith(LPCOLLISIONEVENT e);
    void OnNoCollision(DWORD dt);
};