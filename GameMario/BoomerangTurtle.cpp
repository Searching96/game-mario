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
	left = x - BOOMERANG_TURTLE_BBOX_WIDTH / 2;
	top = y - BOOMERANG_TURTLE_BBOX_HEIGHT / 2;
	right = left + BOOMERANG_TURTLE_BBOX_WIDTH;
	bottom = top + BOOMERANG_TURTLE_BBOX_HEIGHT;
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
	// Debug out walkbetweenstart
	DebugOutTitle(L"[BOOMERANG TURTLE] wbts: %llu, wats: %llu, ps: %llu, ip: %d\n", walkBetweenThrowsStart, walkAfterThrowsStart, prepareStart, isPreparing);

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

	if (walkAfterThrowsStart > walkBetweenThrowsStart)
	{
		if ((GetTickCount64() - walkAfterThrowsStart > BOOMERANG_TURTLE_WALKING_TIMEOUT_2) && !isPreparing)
		{
			prepareStart = GetTickCount64();
			isPreparing = true;
		}
	}
	else
	{
		if ((GetTickCount64() - walkBetweenThrowsStart > BOOMERANG_TURTLE_WALKING_TIMEOUT_1) && !isPreparing)
		{
			prepareStart = GetTickCount64();
			isPreparing = true;
		}
	}

	if ((GetTickCount64() - prepareStart > BOOMERANG_TURTLE_PREPARE_TIMEOUT) && isPreparing)
	{
		if (walkAfterThrowsStart > walkBetweenThrowsStart)
		{
			walkBetweenThrowsStart = GetTickCount64();
			isPreparing = false;
		}
		else
		{
			walkAfterThrowsStart = GetTickCount64();
			isPreparing = false;
		}
	}

	if (fabs(x - x00) >= BOOMERANG_TURTLE_MOVE_OFFSET)
	{
		x00 = x;
		vx = -vx;
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
	{
		if (isPreparing)
			aniId = ID_ANI_BOOMERANG_TURTLE_PREPARE_LEFT;
		else
			aniId = ID_ANI_BOOMERANG_TURTLE_WALKING_LEFT;
	}
	else if (state == BOOMERANG_TURTLE_STATE_WALKING_RIGHT)
	{
		if (isPreparing)
			aniId = ID_ANI_BOOMERANG_TURTLE_PREPARE_RIGHT;
		else
			aniId = ID_ANI_BOOMERANG_TURTLE_WALKING_RIGHT;
	}
	else if (state == BOOMERANG_TURTLE_STATE_DIE_LEFT)
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
	case BOOMERANG_TURTLE_STATE_DIE_RIGHT:
		dieStart = GetTickCount64();
		vy = -0.5f;
		ay = BOOMERANG_TURTLE_GRAVITY;
		isDead = 1;
		walkAfterThrowsStart = GetTickCount64();
		isPreparing = false;
		break;
	case BOOMERANG_TURTLE_STATE_DIE_LEFT:
		dieStart = GetTickCount64();
		vx = 0.5f;
		vy = -0.5f;
		ay = BOOMERANG_TURTLE_GRAVITY;
		isDead = 1;
		walkAfterThrowsStart = GetTickCount64();
		isPreparing = false;
		break;
	case BOOMERANG_TURTLE_STATE_WALKING_RIGHT:
		vx = BOOMERANG_TURTLE_WALKING_SPEED;
		x00 = x;
		break;
	case BOOMERANG_TURTLE_STATE_WALKING_LEFT:
		vx = -BOOMERANG_TURTLE_WALKING_SPEED;
		x00 = x;
		break;
	}
}
