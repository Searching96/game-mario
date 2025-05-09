#include "Goomba.h"
#include "Mario.h"
#include "Koopa.h"
#include "PlayScene.h"

CGoomba::CGoomba(int id, float x, float y, int z, int originalChunkId) : CGameObject(id, x, y, z)
{
	this->originalChunkId = originalChunkId;
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

	if (dynamic_cast<CGoomba*>(e->obj) || dynamic_cast<CWingedGoomba*>(e->obj))
	{
		do
		{
			if (CWingedGoomba* wg = dynamic_cast<CWingedGoomba*>(e->obj))
				if (wg->GetIsWinged() == 1)
					break;

			if (e->nx != 0)
			{
				vx = -vx; // Bounce off another Goomba
				float eVx, eVy;
				e->obj->GetSpeed(eVx, eVy);
				e->obj->SetSpeed(-eVx, eVy);
			}
		} while (false);
	}

	if (dynamic_cast<CKoopa*>(e->obj))
		OnCollisionWithKoopa(e);
}

void CGoomba::OnCollisionWithKoopa(LPCOLLISIONEVENT e)
{
	if (e->obj->GetState() == KOOPA_STATE_SHELL_DYNAMIC)
	{
		if (isDead == 0)
		{
			this->SetState(GOOMBA_STATE_DIE_ON_TAIL_WHIP);
		}
	}
	else
	{
		if (e->nx != 0)
		{
			vx = -vx;
			CKoopa* k = dynamic_cast<CKoopa*>(e->obj);
			float eVx, eVy;
			k->GetSpeed(eVx, eVy);
			k->SetState(eVx > 0 ? KOOPA_STATE_WALKING_LEFT : KOOPA_STATE_WALKING_RIGHT);
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

	if ((isDead == 1) && (GetTickCount64() - dieStart > GOOMBA_DIE_TIMEOUT))
	{
		LPCHUNK chunk = ((LPPLAYSCENE)CGame::GetInstance()->GetCurrentScene())->GetChunk(originalChunkId);
		chunk->SetObjectIsDeleted(this->GetId(), true);
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
		aniId = ID_ANI_GOOMBA_DIE_ON_STOMP;
	else if (state == GOOMBA_STATE_DIE_ON_TAIL_WHIP)
		aniId = ID_ANI_GOOMBA_DIE_ON_TAIL_WHIP;

	CAnimations::GetInstance()->Get(aniId)->Render(x,y);

	//RenderBoundingBox();
}

void CGoomba::SetState(int state)
{
	if (isDead == 1) return;
	CGameObject::SetState(state);
	switch (state)
	{
	case GOOMBA_STATE_DIE_ON_STOMP:
		// get current chunk
		dieStart = GetTickCount64();
		y += (GOOMBA_BBOX_HEIGHT - GOOMBA_BBOX_HEIGHT_DIE)/2 - 3;
		vx = 0;
		vy = 0;
		ay = 0;
		isDead = 1;
		break;
	case GOOMBA_STATE_DIE_ON_TAIL_WHIP:
		dieStart = GetTickCount64();
		vy = -0.5f;
		ay = GOOMBA_GRAVITY;
		isDead = 1;
		break;
	case GOOMBA_STATE_DIE_ON_HELD_KOOPA:
		dieStart = GetTickCount64();
		vy = -0.35f;
		ay = GOOMBA_GRAVITY;
		isDead = 1;
		break;
	case GOOMBA_STATE_WALKING: 
		vx = -GOOMBA_WALKING_SPEED;
		break;
	}
}
