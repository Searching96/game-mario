#include "Fireball.h"
#include "PiranhaPlant.h"
#include "QuestionBlock.h"

#include "Fireball.h"
#include "PiranhaPlant.h"
#include "QuestionBlock.h"

void CFireball::GetBoundingBox(float& l, float& t, float& r, float& b)
{
    l = x - FIREBALL_BBOX_WIDTH / 2;
    t = y - FIREBALL_BBOX_HEIGHT / 2;
    r = l + FIREBALL_BBOX_WIDTH;
    b = t + FIREBALL_BBOX_HEIGHT;
}



void CFireball::Render()
{
	int aniId = ID_ANI_FIREBALL_STATIC;
	if (state == FIREBALL_STATE_SHOOT_TOP_LEFT
		|| state == FIREBALL_STATE_SHOOT_BOTTOM_LEFT
		|| state == FIREBALL_STATE_SHOOT_BOTTOM_RIGHT
		|| state == FIREBALL_STATE_SHOOT_TOP_RIGHT)
		aniId = ID_ANI_FIREBALL_DYNAMIC;
	CAnimations* animations = CAnimations::GetInstance();
	animations->Get(aniId)->Render(x, y);

	//RenderBoundingBox();
}

void CFireball::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
    x += vx * dt;
    y += vy * dt;

    if (fabs(x - x0) >= FIREBALL_RANGE_WIDTH || fabs(y - y0) >= FIREBALL_RANGE_HEIGHT)
    {
        SetState(FIREBALL_STATE_SHOOT_COMPLETE);
        return;
    }

    if (state == FIREBALL_STATE_ASCEND)
    {
        float targetY = y0 - PIRANHA_PLANT_BBOX_HEIGHT;
        if (y <= targetY)
        {
            y = targetY;
            SetState(FIREBALL_STATE_STATIC);
        }
    }
    else if (state == FIREBALL_STATE_DESCEND)
    {
        float targetY = y0 + PIRANHA_PLANT_BBOX_HEIGHT;
        if (y >= targetY)
        {
            y = targetY;
            SetState(FIREBALL_STATE_STATIC);
        }
    }

	//DebugOut(L"[FIREBALL] x: %f, y: %f, vx: %f, vy: %f, state: %i\n", x, y, vx, vy, state);
}

void CFireball::SetState(int state)
{
    switch (state)
    {
    case FIREBALL_STATE_STATIC:
        vx = 0;
        vy = 0;
        break;
    case FIREBALL_STATE_ASCEND:
        vx = 0;
        vy = PIRANHA_PLANT_MOVE_SPEED;
        break;
    case FIREBALL_STATE_DESCEND:
        vx = 0;
        vy = -PIRANHA_PLANT_MOVE_SPEED;
        break;
    case FIREBALL_STATE_SHOOT_TOP_RIGHT:
        vx = FIREBALL_SPEED * 2;
        vy = -FIREBALL_SPEED;
        break;
    case FIREBALL_STATE_SHOOT_TOP_LEFT:
        vx = -FIREBALL_SPEED * 2;
        vy = -FIREBALL_SPEED;
        break;
    case FIREBALL_STATE_SHOOT_BOTTOM_LEFT:
        vx = -FIREBALL_SPEED * 2;
        vy = FIREBALL_SPEED;
        break;
    case FIREBALL_STATE_SHOOT_BOTTOM_RIGHT:
        vx = FIREBALL_SPEED * 2;
        vy = FIREBALL_SPEED;
        break;
    case FIREBALL_STATE_SHOOT_COMPLETE:
        x = x0;
        y = y0;
        vx = 0;
        vy = 0;
        break;
    }
    CGameObject::SetState(state);
}
