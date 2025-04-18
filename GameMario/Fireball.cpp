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
	if (state == FIREBALL_STATE_STATIC) return;
	int aniId = ID_ANI_FIREBALL_DYNAMIC;
	CAnimations* animations = CAnimations::GetInstance();
	animations->Get(aniId)->Render(x, y);

	//RenderBoundingBox();
}

void CFireball::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	if (state == FIREBALL_STATE_STATIC) return;
	x += vx * dt;
	y += vy * dt;

	if (fabs(x - x0) >= FIREBALL_RANGE_WIDTH || fabs(y - y0) >= FIREBALL_RANGE_HEIGHT)
	{
		SetState(FIREBALL_STATE_STATIC);
		return;
	}

	//DebugOut(L"[FIREBALL] x: %f, y: %f, vx: %f, vy: %f, state: %i\n", x, y, vx, vy, state);
	//DebugOut(L"[FIREBALL] x0: %f, y0: %f\n", x0, y0);
}

void CFireball::SetState(int state)
{
	switch (state)
	{
	case FIREBALL_STATE_STATIC:
		vx = 0;
		vy = 0;
		SetEnable(0);
		break;
	case FIREBALL_STATE_SHOOT_TOP_RIGHT:
		x = x0;
		y = y0;
		vx = FIREBALL_SPEED;
		vy = -FIREBALL_SPEED;
		break;
	case FIREBALL_STATE_SHOOT_TOP_LEFT:
		x = x0;
		y = y0;
		vx = -FIREBALL_SPEED;
		vy = -FIREBALL_SPEED;
		break;
	case FIREBALL_STATE_SHOOT_UPPER_CENTER_RIGHT:
		x = x0;
		y = y0;
		vx = FIREBALL_SPEED * 4 / 5;
		vy = -FIREBALL_SPEED * 1 / 5;
		break;
	case FIREBALL_STATE_SHOOT_UPPER_CENTER_LEFT:
		x = x0;
		y = y0;
		vx = -FIREBALL_SPEED * 4 / 5;
		vy = -FIREBALL_SPEED * 1 / 5;
		break;
	case FIREBALL_STATE_SHOOT_LOWER_CENTER_RIGHT:
		x = x0;
		y = y0;
		vx = FIREBALL_SPEED * SQUARE_ROOT_25_29;
		vy = FIREBALL_SPEED * SQUARE_ROOT_4_29;
		break;
	case FIREBALL_STATE_SHOOT_LOWER_CENTER_LEFT:
		x = x0;
		y = y0;
		vx = -FIREBALL_SPEED * SQUARE_ROOT_25_29;
		vy = FIREBALL_SPEED * SQUARE_ROOT_4_29;
		break;
	case FIREBALL_STATE_SHOOT_BOTTOM_RIGHT:
		x = x0;
		y = y0;
		vx = FIREBALL_SPEED;
		vy = FIREBALL_SPEED;
		break;
	case FIREBALL_STATE_SHOOT_BOTTOM_LEFT:
		x = x0;
		y = y0;
		vx = -FIREBALL_SPEED;
		vy = FIREBALL_SPEED;
		break;
	}
	CGameObject::SetState(state);
}
