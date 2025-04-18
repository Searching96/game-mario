#include "Coin.h"
#include "QuestionBlock.h"

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
	if (isVisible == 0)
		return;
	//int aniId = ID_ANI_COIN_STATIC;
	//if (state != COIN_STATE_STATIC)
		//aniId = ID_ANI_COIN_DYNAMIC;
	int aniId = ID_ANI_COIN_DYNAMIC;
	CAnimations* animations = CAnimations::GetInstance();
	animations->Get(aniId)->Render(x, y);

	//RenderBoundingBox();
}

void CCoin::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	if (bounceUp == 1)
	{
		if (GetTickCount64() - bounceUpStart > COIN_BOUNCE_UP_TIME)
		{
			bounceUp = 0;
			SetState(COIN_STATE_BOUNCE_DOWN);
		}
	}
	if (bounceDown == 1)
	{
		if (GetTickCount64() - bounceDownStart > COIN_BOUNCE_DOWN_TIME)
		{
			bounceDown = 0;
			SetState(COIN_STATE_BOUNCE_COMPLETE);
		}
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
	switch (state)
	{
	case COIN_STATE_STATIC:
		isVisible = 1;
		break;
	case COIN_STATE_DYNAMIC:
		isVisible = 0;
		break;
	case COIN_STATE_BOUNCE_UP:
		vx = 0.0f;
		vy = COIN_BOUNCE_SPEED;
		isVisible = 1;
		StartBounceUp();
		break;
	case COIN_STATE_BOUNCE_DOWN:
		vx = 0.0f;
		vy = -COIN_BOUNCE_SPEED;
		StartBounceDown();
		break;
	case COIN_STATE_BOUNCE_COMPLETE:
		this->Delete();
		break;
	}
	CGameObject::SetState(state);
}
