#include "Goomba.h"
#include "Mario.h"
#include "Koopa.h"
#include "PlayScene.h"

CGoomba::CGoomba(float x, float y):CGameObject(x, y)
{
	this->ax = 0;
	this->ay = GOOMBA_GRAVITY;
	SetState(GOOMBA_STATE_WALKING);
}

void CGoomba::GetBoundingBox(float &left, float &top, float &right, float &bottom)
{
	if (state == GOOMBA_STATE_DIE_ON_STOMP)
	{
		left = x - GOOMBA_BBOX_WIDTH/2;
		top = y - GOOMBA_BBOX_HEIGHT_DIE/2;
		right = left + GOOMBA_BBOX_WIDTH;
		bottom = top + GOOMBA_BBOX_HEIGHT_DIE;
	}
	else
	{ 
		left = x - GOOMBA_BBOX_WIDTH/2;
		top = y - GOOMBA_BBOX_HEIGHT/2;
		right = left + GOOMBA_BBOX_WIDTH;
		bottom = top + GOOMBA_BBOX_HEIGHT;
	}
}

void CGoomba::OnNoCollision(DWORD dt)
{
	x += vx * dt;
	y += vy * dt;
};

// Corrected Goomba::OnCollisionWith
void CGoomba::OnCollisionWith(LPCOLLISIONEVENT e)
{
	if (!e->obj->IsBlocking() && !dynamic_cast<CKoopa*>(e->obj))
		return;

	if (CKoopa* koopa = dynamic_cast<CKoopa*>(e->obj))
	{
		if (koopa->GetState() == KOOPA_STATE_SHELL_DYNAMIC)
		{
			if (this->state != GOOMBA_STATE_DIE_ON_STOMP && this->state != GOOMBA_STATE_DIE_ON_TAIL_WHIP)
			{
				DebugOut(L"Shell hit Goomba (Detected in Goomba.cpp)");
				this->SetState(GOOMBA_STATE_DIE_ON_TAIL_WHIP);
			}
			return;
		}
		else
		{
			if (e->nx != 0)
			{
				vx = -vx;
			}
			return;
		}
	}

	if (e->obj->IsBlocking())
	{
		if (e->ny != 0)
		{
			vy = 0;
		}
		else if (e->nx != 0)
		{
			vx = -vx; // Turn around when hitting a wall
		}
	}
}

void CGoomba::Update(DWORD dt, vector<LPGAMEOBJECT> *coObjects)
{
	CMario* player = (CMario*)((LPPLAYSCENE)CGame::GetInstance()->GetCurrentScene())->GetPlayer();
	float mX, mY;
	player->GetPosition(mX, mY);
	if (fabs(x - mX) >= 300)
		return;

	vy += ay * dt;
	vx += ax * dt;

	if ((state==GOOMBA_STATE_DIE_ON_STOMP) && (GetTickCount64() - dieOnStompStart > GOOMBA_DIE_TIMEOUT))
	{
		isDeleted = true;
		return;
	}
	if ((state == GOOMBA_STATE_DIE_ON_TAIL_WHIP) && (GetTickCount64() - dieOnWhipStart > GOOMBA_DIE_TIMEOUT))
	{
		isDeleted = true;
		return;
	}


	CGameObject::Update(dt, coObjects);
	CCollision::GetInstance()->Process(this, dt, coObjects);
}


void CGoomba::Render()
{
	int aniId = ID_ANI_GOOMBA_WALKING;
	if (state == GOOMBA_STATE_DIE_ON_STOMP) 
	{
		aniId = ID_ANI_GOOMBA_DIE_ON_STOMP;
	}
	else if (state == GOOMBA_STATE_DIE_ON_TAIL_WHIP)
	{
		aniId = ID_ANI_GOOMBA_DIE_ON_TAIL_WHIP;
	}

	CAnimations::GetInstance()->Get(aniId)->Render(x,y);

	//RenderBoundingBox();
}

void CGoomba::SetState(int state)
{
	CGameObject::SetState(state);
	switch (state)
	{
	case GOOMBA_STATE_DIE_ON_STOMP:
		dieOnStompStart = GetTickCount64();
		y += (GOOMBA_BBOX_HEIGHT - GOOMBA_BBOX_HEIGHT_DIE)/2 - 3;
		vx = 0;
		vy = 0;
		ay = 0; 
		break;
	case GOOMBA_STATE_DIE_ON_TAIL_WHIP:
		dieOnWhipStart = GetTickCount64();
		vy = -0.5f;
		ay = GOOMBA_GRAVITY;
		break;
	case GOOMBA_STATE_WALKING: 
		vx = -GOOMBA_WALKING_SPEED;
		break;
	}
}
