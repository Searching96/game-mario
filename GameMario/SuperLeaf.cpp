#include "SuperLeaf.h"

CSuperLeaf::CSuperLeaf(float x, float y) : CGameObject(x, y)
{
    this->ax = 0;
    this->ay = SUPERLEAF_GRAVITY;
}

void CSuperLeaf::Render()
{
    CAnimations* animations = CAnimations::GetInstance();
    animations->Get(ID_ANI_SUPERLEAF)->Render(x, y);

    RenderBoundingBox();
}

void CSuperLeaf::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
    vy += ay * dt;
    vx += ax * dt;

    // just test
    /*vx = vy = 0;*/
    vx = SUPERLEAF_FLOATING_SPEED;

    CGameObject::Update(dt, coObjects);
    CCollision::GetInstance()->Process(this, dt, coObjects);
}

void CSuperLeaf::OnNoCollision(DWORD dt)
{
    x += vx * dt;
    y += vy * dt;
}

void CSuperLeaf::OnCollisionWith(LPCOLLISIONEVENT e)
{
    if (!e->obj->IsBlocking()) return;
}

void CSuperLeaf::GetBoundingBox(float& l, float& t, float& r, float& b)
{
    l = x - SUPERLEAF_BBOX_WIDTH / 2;
    t = y - SUPERLEAF_BBOX_HEIGHT / 2;
    r = l + SUPERLEAF_BBOX_WIDTH;
    b = t + SUPERLEAF_BBOX_HEIGHT;
}