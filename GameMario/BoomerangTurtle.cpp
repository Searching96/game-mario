#include "BoomerangTurtle.h"
#include "Mario.h"

#include "PlayScene.h"

CBoomerangTurtle::CBoomerangTurtle(int id, float x, float y, int z, int originalChunkId) : CGameObject(id, x, y, z)
{
	this->originalChunkId = originalChunkId;
	this->ax = 0;
	this->ay = BOOMERANG_TURTLE_GRAVITY;
	x0 = x;
	y0 = y;
	SetState(BOOMERANG_TURTLE_STATE_WALKING_LEFT);
}

void CBoomerangTurtle::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	left = x - GOOMBA_BBOX_WIDTH / 2;
	top = y - GOOMBA_BBOX_HEIGHT / 2;
	right = left + GOOMBA_BBOX_WIDTH;
	bottom = top + GOOMBA_BBOX_HEIGHT;
}

void CBoomerangTurtle::OnNoCollision(DWORD dt)
{
	x += vx * dt;
	y += vy * dt;
};

void CBoomerangTurtle::OnCollisionWith(LPCOLLISIONEVENT e)
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
}

void CBoomerangTurtle::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	if (isDefeated)
		return;

	vy += ay * dt;
	vx += ax * dt;

	if ((isDead == 1) && (GetTickCount64() - dieStart > BOOMERANG_TURTLE_DIE_TIMEOUT))
	{
		LPCHUNK chunk = ((LPPLAYSCENE)CGame::GetInstance()->GetCurrentScene())->GetChunk(originalChunkId);
		chunk->SetIsObjectDeleted(this->GetId(), true);
		isDeleted = true;
		return;
	}

	CGameObject::Update(dt, coObjects);
	CCollision::GetInstance()->Process(this, dt, coObjects);
}


void CBoomerangTurtle::Render()
{
	if (isDefeated)
		return;

	int aniId = ID_ANI_GOOMBA_WALKING;
	if (state == BOOMERANG_TURTLE_STATE_WALKING_LEFT)
		aniId = ID_ANI_BOOMERANG_TURTLE_WALKING_LEFT;
	else if (state == BOOMERANG_TURTLE_STATE_WALKING_RIGHT)
		aniId = ID_ANI_BOOMERANG_TURTLE_WALKING_RIGHT;
	if (state == BOOMERANG_TURTLE_STATE_DIE_LEFT)
		aniId = ID_ANI_BOOMERANG_TURTLE_DIE_LEFT;
	else if (state == BOOMERANG_TURTLE_STATE_DIE_RIGHT)
		aniId = ID_ANI_BOOMERANG_TURTLE_DIE_RIGHT;

	CAnimations::GetInstance()->Get(aniId)->Render(x, y);

	//RenderBoundingBox();
}

void CBoomerangTurtle::SetState(int state)
{
	if (isDead == 1) return;
	CGameObject::SetState(state);
	switch (state)
	{
	case GOOMBA_STATE_DIE_ON_STOMP:
		dieStart = GetTickCount64();
		y += (GOOMBA_BBOX_HEIGHT - GOOMBA_BBOX_HEIGHT_DIE) / 2 - 3;
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
