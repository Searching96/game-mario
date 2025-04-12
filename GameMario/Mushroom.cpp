#include "Mushroom.h"
#include "Mario.h"
#include "QuestionBlock.h"

CMushroom::CMushroom(float x, float y) : CGameObject(x, y)
{
    this->ax = 0;
    this->ay = MUSHROOM_GRAVITY;
	this->SetState(MUSHROOM_STATE_NOT_HIT);

	spawned = 0;
	spawn_start = -1;
}

void CMushroom::Render()
{
    CAnimations* animations = CAnimations::GetInstance();
    animations->Get(ID_ANI_MUSHROOM)->Render(x, y);

    RenderBoundingBox();
}

void CMushroom::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
    if (GetTickCount64() - spawn_start > MUSHROOM_SPAWN_TIME)
    {
		ay = MUSHROOM_GRAVITY;
		vx = MUSHROOM_WALKING_SPEED;
		spawn_start = -1;
		spawned = 1;
        return;
    }

    vy += ay * dt;
    vx += ax * dt;

    CGameObject::Update(dt, coObjects);
    CCollision::GetInstance()->Process(this, dt, coObjects);
}

void CMushroom::OnNoCollision(DWORD dt)
{
    x += vx * dt;
    y += vy * dt;
}

void CMushroom::OnCollisionWith(LPCOLLISIONEVENT e)
{
    if (!e->obj->IsBlocking()) return;
    if (!dynamic_cast<CMario*>(e->obj)) return; // koopa will be at here later on
    if (dynamic_cast<CQuestionBlock*>(e->obj)) return;

    if (e->ny != -1)
    {
        y -= 16; // RED ALERT
        if (e->nx == -1)
            vx = MUSHROOM_WALKING_SPEED;
        else if (e->nx == 0)
            vx = -MUSHROOM_WALKING_SPEED;
		this->SetState(MUSHROOM_STATE_HIT);
    }
}

void CMushroom::GetBoundingBox(float& l, float& t, float& r, float& b)
{
    l = x - MUSHROOM_BBOX_WIDTH / 2;
    t = y - MUSHROOM_BBOX_HEIGHT / 2;
    r = l + MUSHROOM_BBOX_WIDTH;
    b = t + MUSHROOM_BBOX_HEIGHT;
}


void CMushroom::SetState(int state)
{
    CGameObject::SetState(state);
    switch (state)
    {
    case MUSHROOM_STATE_HIT:
        vy = -0.0002f;
        SpawnStart();
        //vx = MUSHROOM_WALKING_SPEED;
        //ax = MUSHROOM_GRAVITY;
        break;
    case MUSHROOM_STATE_NOT_HIT:
        vy = 0;
		vx = 0;
        break;
    }
}