#include "Coin.h"

CCoin::CCoin(float x, float y, int type) : CGameObject(x, y)
{
	this->type = type;
	if (type == 0)
		this->SetState(COIN_STATE_STATIC);
	else
		this->SetState(COIN_STATE_DYNAMIC);
}

void CCoin::Render()
{
	int aniId = ID_ANI_COIN_STATIC;
	if (state != ID_ANI_COIN_STATIC)
		aniId = ID_ANI_COIN_DYNAMIC;
	CAnimations* animations = CAnimations::GetInstance();
	animations->Get(aniId)->Render(x, y);

	//RenderBoundingBox();
}

void CCoin::Update(DWORD dt)
{
	if (bounceUp == 1)
	{
		if (GetTickCount64() - bounceUpStart > COIN_BOUNCE_UP_TIME)
		{
			bounceUp = 0;
			SetState(COIN_STATE_BOUNCE_DOWN);
		}
		return;
	}
	else if (bounceDown == 1)
	{
		if (GetTickCount64() - bounceDownStart > COIN_BOUNCE_DOWN_TIME)
		{
			bounceDown = 0;
			SetState(COIN_STATE_BOUNCE_COMPLETE);
		}
		return;
	}
	x += vx * dt;
	y += vy * dt;
}

void CCoin::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x - COIN_BBOX_WIDTH / 2;
	t = y - COIN_BBOX_HEIGHT / 2;
	r = l + COIN_BBOX_WIDTH;
	b = t + COIN_BBOX_HEIGHT;
}

void CCoin::SetState(int state)
{
	CGameObject::SetState(state);
	switch (state)
	{
	case COIN_STATE_STATIC:
		break;
	case COIN_STATE_DYNAMIC:
		break;
	case COIN_STATE_BOUNCE_UP:
		vy = COIN_BOUNCE_SPEED;
		break;
	case COIN_STATE_BOUNCE_DOWN:
		vy = -COIN_BOUNCE_SPEED;
		break;
	case COIN_STATE_BOUNCE_COMPLETE:
		this->Delete();
		break;
	}
}
