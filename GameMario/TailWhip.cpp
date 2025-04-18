#include "TailWhip.h"
#include "QuestionBlock.h"
#include "Goomba.h"
#include "Mario.h"

#include "PlayScene.h"

CTailWhip::CTailWhip(float x, float y) : CGameObject(x, y) {}

void CTailWhip::Render()
{
	if (notWhipping == 1)
		return;
	CAnimations* animations = CAnimations::GetInstance();
	animations->Get(ID_ANI_TAIL_WHIP)->Render(x, y);

	RenderBoundingBox();
}

void CTailWhip::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	if (notWhipping == 1)
		return;
	
	if (whipSpin == 2)
	{
		whipSpin = 0;
		SetState(TAIL_STATE_NOT_WHIPPING);
	}
	else if (whippingLeft == 1)
	{
		if (GetTickCount64() - whipLeftStart > TAIL_WHIP_LEFT_TIME)
		{
			whippingLeft = 0;
			SetState(TAIL_STATE_WHIPPING_RIGHT);
		}
	}
	else if (whippingRight == 1)
	{
		if (GetTickCount64() - whipRightStart > TAIL_WHIP_RIGHT_TIME)
		{
			whippingRight = 0;
			SetState(TAIL_STATE_WHIPPING_LEFT);
		}
	}

	CMario* mario = dynamic_cast<CMario*>(dynamic_cast<CPlayScene*>(CGame::GetInstance()->GetCurrentScene())->GetPlayer());

	float mX, mY, mNx;
	mario->GetPosition(mX, mY);
	mario->GetNx(mNx);

	if (whippingRight == 1)
		x = mX + 6;
	else if (whippingLeft == 1)
		x = mX - 6;
	y = mY + 6;

	CCollision::GetInstance()->Process(this, dt, coObjects);
}

void CTailWhip::OnCollisionWith(LPCOLLISIONEVENT e)
{
	if (dynamic_cast<CGoomba*>(e->obj))
		OnCollisionWithGoomba(e);
}

void CTailWhip::OnCollisionWithGoomba(LPCOLLISIONEVENT e)
{
	CGoomba* goomba = dynamic_cast<CGoomba*>(e->obj);
	if (goomba)
	{
		goomba->SetState(GOOMBA_STATE_DIE_ON_TAIL_WHIP);
	}
}

void CTailWhip::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x - TAIL_WHIP_BBOX_WIDTH / 2;
	t = y - TAIL_WHIP_BBOX_HEIGHT / 2;
	r = l + TAIL_WHIP_BBOX_WIDTH;
	b = t + TAIL_WHIP_BBOX_HEIGHT;
}

void CTailWhip::SetState(int state)
{
	switch (state)
	{
	case TAIL_STATE_WHIPPING_LEFT:
		whipSpin++;
		whippingLeft = 1;
		notWhipping = 0;
		whipLeftStart = GetTickCount64();
		break;
	case TAIL_STATE_WHIPPING_RIGHT:
		whipSpin++;
		whippingRight = 1;
		notWhipping = 0;
		whipRightStart = GetTickCount64();
		break;
	case TAIL_STATE_NOT_WHIPPING:
		whippingLeft = 0;
		whippingRight = 0;
		notWhipping = 1;
		break;
	}
	CGameObject::SetState(state);
}
