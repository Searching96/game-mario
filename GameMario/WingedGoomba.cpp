#include "WingedGoomba.h"

CWingedGoomba::CWingedGoomba(float x, float y) : CGameObject(x, y)
{
	this->ax = 0;
	this->ay = WINGED_GOOMBA_GRAVITY;
	SetState(WINGED_GOOMBA_STATE_WALKING);
}

void CWingedGoomba::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	if (state == WINGED_GOOMBA_STATE_DIE_ON_STOMP)
	{
		left = x - WINGED_GOOMBA_BBOX_WIDTH / 2;
		top = y - WINGED_GOOMBA_BBOX_HEIGHT_DIE / 2;
		right = left + WINGED_GOOMBA_BBOX_WIDTH;
		bottom = top + WINGED_GOOMBA_BBOX_HEIGHT_DIE;
	}
	else
	{
		left = x - WINGED_GOOMBA_BBOX_WIDTH / 2;
		top = y - WINGED_GOOMBA_BBOX_HEIGHT / 2;
		right = left + WINGED_GOOMBA_BBOX_WIDTH;
		bottom = top + WINGED_GOOMBA_BBOX_HEIGHT;
	}
}

void CWingedGoomba::OnNoCollision(DWORD dt)
{
	x += vx * dt;
	y += vy * dt;
};

void CWingedGoomba::OnCollisionWith(LPCOLLISIONEVENT e)
{
	if (!e->obj->IsBlocking()) return;
	if (dynamic_cast<CWingedGoomba*>(e->obj)) return;

	if (e->ny != 0)
	{
		vy = 0;
	}
	else if (e->nx != 0)
	{
		vx = -vx;
	}
}

void CWingedGoomba::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	vy += ay * dt;
	vx += ax * dt;

	if ((state == WINGED_GOOMBA_STATE_DIE_ON_STOMP) && (GetTickCount64() - dieOnStompStart > WINGED_GOOMBA_DIE_TIMEOUT))
	{
		isDeleted = true;
		return;
	}

	CGameObject::Update(dt, coObjects);
	CCollision::GetInstance()->Process(this, dt, coObjects);
}

void CWingedGoomba::Render()
{
	int aniId = ID_ANI_WINGED_GOOMBA_WALKING;
	if (state == WINGED_GOOMBA_STATE_DIE_ON_STOMP)
	{
		aniId = ID_ANI_WINGED_GOOMBA_DIE_ON_STOMP;
	}
	else if (state == WINGED_GOOMBA_STATE_DIE_ON_TAIL_WHIP)
	{
		aniId = ID_ANI_WINGED_GOOMBA_DIE_ON_TAIL_WHIP;
	}

	CAnimations::GetInstance()->Get(aniId)->Render(x, y);
	//RenderBoundingBox();
}

void CWingedGoomba::SetState(int state)
{
	CGameObject::SetState(state);
	switch (state)
	{
	case WINGED_GOOMBA_STATE_DIE_ON_STOMP:
		dieOnStompStart = GetTickCount64();
		y += (WINGED_GOOMBA_BBOX_HEIGHT - WINGED_GOOMBA_BBOX_HEIGHT_DIE) / 2;
		vx = 0;
		vy = 0;
		ay = 0;
		break;
	case WINGED_GOOMBA_STATE_DIE_ON_TAIL_WHIP:
		vy = -0.5f;
		ay = WINGED_GOOMBA_GRAVITY;
		break;
	case WINGED_GOOMBA_STATE_WALKING:
		vx = -WINGED_GOOMBA_WALKING_SPEED;
		break;
	}
}
