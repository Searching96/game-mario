#include "Mushroom.h"
#include "Mario.h"
#include "QuestionBlock.h"

CMushroom::CMushroom(float x, float y) : CGameObject(x, y)
{
    this->ax = 0;
    this->ay = 0;
	this->SetState(MUSHROOM_STATE_NOT_HIT);
}

void CMushroom::Render()
{
	if (isVisible == 0) return;

    CAnimations* animations = CAnimations::GetInstance();
    animations->Get(ID_ANI_MUSHROOM)->Render(x, y);

    //RenderBoundingBox();
}

void CMushroom::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	if (isVisible == 0) return;

	if (bounceUp == 1)
	{
		if (fabs(y - y0) >= MUSHROOM_BOUNCE_OFFSET)
		{
			y = y0 - MUSHROOM_BOUNCE_OFFSET;
			bounceUp = 0;
			SetState(MUSHROOM_STATE_BOUNCE_DOWN);
		}
	}
	else if (bounceDown == 1)
	{
		if (fabs(y - y0) >= MUSHROOM_BOUNCE_OFFSET)
		{
			y = y0 + MUSHROOM_BOUNCE_OFFSET;
			bounceDown = 0;
			SetState(MUSHROOM_STATE_RISE);
		}
	}
	else if (rise == 1)
	{
		if (y0 - y > MUSHROOM_RISE_OFFSET)
		{
			rise = 0;
			SetState(MUSHROOM_STATE_MOVING);
		}
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
	if (dynamic_cast<CQuestionBlock*>(e->obj)) return;
	if (!dynamic_cast<CMario*>(e->obj)) return; // koopa will be added here later on
}

void CMushroom::GetBoundingBox(float& l, float& t, float& r, float& b)
{
    l = x - QUESTIONBLOCK_BBOX_WIDTH / 2;
    t = y - MUSHROOM_BBOX_HEIGHT / 2;
    r = l + QUESTIONBLOCK_BBOX_WIDTH;
    b = t + MUSHROOM_BBOX_HEIGHT;
}


void CMushroom::SetState(int state)
{
    CGameObject::SetState(state);
    switch (state)
    {
    case MUSHROOM_STATE_NOT_HIT:
        break;
    case MUSHROOM_STATE_BOUNCE_UP:
		y0 = y;
        vy = MUSHROOM_BOUNCE_SPEED;
        StartBounceUp();
        break;
    case MUSHROOM_STATE_BOUNCE_DOWN:
		y0 = y;
        vy = -MUSHROOM_BOUNCE_SPEED;
        StartBounceDown();
        break;
	case MUSHROOM_STATE_RISE:
        y0 = y;
		vy = MUSHROOM_RISE_SPEED;
		StartRise();
        break;
	case MUSHROOM_STATE_MOVING:
		ay = MUSHROOM_GRAVITY;
		if (collisionNx > 0)
			vx = MUSHROOM_WALKING_SPEED;
		else
			vx = -MUSHROOM_WALKING_SPEED;
		break;
    }
}