#include "entities/items/SuperLeaf.h"

CSuperLeaf::CSuperLeaf(int id, float x, float y, int z) : CGameObject(id, x, y, z)
{
	this->ax = 0;
	this->ay = 0;
	this->SetState(SUPERLEAF_STATE_NOT_HIT);
}

void CSuperLeaf::Render()
{
	if (isVisible == 0) return;

	int aniId = ID_ANI_SUPERLEAF_RIGHT;
	if (floatingLeft == 1)
		aniId = ID_ANI_SUPERLEAF_LEFT;
    CAnimations* animations = CAnimations::GetInstance();
    animations->Get(aniId)->Render(x, y);

    //RenderBoundingBox();
}

void CSuperLeaf::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	if (isVisible == 0) return;

	if (bounceUp == 1)
	{
		if (GetTickCount64() - bounceUpStart > SUPERLEAF_BOUNCE_UP_TIME)
		{
			bounceUp = 0;
			SetState(SUPERLEAF_STATE_FLOATING_RIGHT);
		}
	}
	else if (floatingRight == 1)
	{
		if (GetTickCount64() - floatingRightStart > SUPERLEAF_FLOATING_TIME)
		{
			floatingRight = 0;
			SetState(SUPERLEAF_STATE_FLOATING_LEFT);
		}
	}
	else if (floatingLeft == 1)
	{
		if (GetTickCount64() - floatingLeftStart > SUPERLEAF_FLOATING_TIME)
		{
			floatingLeft = 0;
			SetState(SUPERLEAF_STATE_FLOATING_RIGHT);
		}
	}

    vy += ay * dt;
    vx += ax * dt;

	x += vx * dt;
	y += vy * dt;

    CGameObject::Update(dt, coObjects);
    CCollision::GetInstance()->Process(this, dt, coObjects);
}

void CSuperLeaf::OnNoCollision(DWORD dt)
{
	return;
}

void CSuperLeaf::OnCollisionWith(LPCOLLISIONEVENT e)
{
	return;
}

void CSuperLeaf::GetBoundingBox(float& l, float& t, float& r, float& b)
{
    l = x - SUPERLEAF_BBOX_WIDTH / 2;
    t = y - SUPERLEAF_BBOX_HEIGHT / 2;
    r = l + SUPERLEAF_BBOX_WIDTH;
    b = t + SUPERLEAF_BBOX_HEIGHT;
}


void CSuperLeaf::SetState(int state)
{
    switch (state)
    {
    case SUPERLEAF_STATE_NOT_HIT:
        break;
	case SUPERLEAF_STATE_BOUNCE_UP:
        vx = 0;
        vy = SUPERLEAF_BOUNCE_UP_SPEED;
		StartBounceUp();
        break;
	case SUPERLEAF_STATE_FLOATING_RIGHT:
		vx = SUPERLEAF_FLOATING_X_SPEED;
		vy = SUPERLEAF_FLOATING_Y_SPEED;
		StartFloatingRight();
		break;
	case SUPERLEAF_STATE_FLOATING_LEFT:
		vx = -SUPERLEAF_FLOATING_X_SPEED;
		vy = SUPERLEAF_FLOATING_Y_SPEED;
		StartFloatingLeft();
		break;
    }
	CGameObject::SetState(state);
}
