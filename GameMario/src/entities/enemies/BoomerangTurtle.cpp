#include "entities/enemies/BoomerangTurtle.h"
#include "entities/player/Mario.h"

#include "core/PlayScene.h"

CBoomerangTurtle::CBoomerangTurtle(int id, float x, float y, int z, int originalChunkId, vector<CBoomerang*> boomerangList) : CGameObject(id, x, y, z)
{
	this->originalChunkId = originalChunkId;
	this->ax = 0;
	this->ay = BOOMERANG_TURTLE_GRAVITY;
	this->boomerangList = boomerangList;
	x0 = x;
	y0 = y;
	walkAfterThrowsStart = GetTickCount64() - 2000;
	SetState(BOOMERANG_TURTLE_STATE_WALKING_RIGHT);
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

	if (dynamic_cast<CBoomerang*>(e->obj))
		OnCollisionWithBoomerang(e);
}

void CBoomerangTurtle::OnCollisionWithBoomerang(LPCOLLISIONEVENT e)
{
	CBoomerang* b = dynamic_cast<CBoomerang*>(e->obj);
	if (b)
	{
		if (b->GetState() != BOOMERANG_STATE_HOMING) return;

		b->SetIsVisible(false);
		b->SetState(BOOMERANG_STATE_IDLING);
	}
}

void CBoomerangTurtle::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	//DebugOutTitle(L"[BOOMERANG TURTLE] wbts: %llu, wats: %llu, ps: %llu, ip: %d, bi: %d\n", walkBetweenThrowsStart, walkAfterThrowsStart, prepareStart, isPreparing, boomerangIndex);
	//DebugOutTitle(L"[BOOMERANG 0] ii = %d, iv = %d\n", boomerangList[0]->IsIdling(), boomerangList[0]->IsVisible());

	if (isDefeated)
		return;
	
	CMario* player = (CMario*)((LPPLAYSCENE)CGame::GetInstance()->GetCurrentScene())->GetPlayer();
	float mX, mY;
	player->GetPosition(mX, mY);
	int camWidth = CGame::GetInstance()->GetBackBufferWidth();
	if (mX + camWidth < x) return;

	for (int i = 0; i < boomerangList.size(); i++)
	{
		if (boomerangList[i]->IsIdling())
		{
			if (nx < 0)
				boomerangList[i]->SetPosition(x - 6, y - 14);
			else if (nx > 0)
				boomerangList[i]->SetPosition(x + 6, y - 14);
		}
	}

	vy += ay * dt;
	vx += ax * dt;

	if ((isDead == 1) && (GetTickCount64() - dieStart > BOOMERANG_TURTLE_DIE_TIMEOUT))
	{
		LPCHUNK chunk = ((LPPLAYSCENE)CGame::GetInstance()->GetCurrentScene())->GetChunk(originalChunkId);
		chunk->SetIsObjectDeleted(this->GetId(), true);
		isDeleted = true;
		return;
	}

	// Prepare boomerang
	if (!isDead)
	{
		if (walkAfterThrowsStart > walkBetweenThrowsStart)
		{
			if ((GetTickCount64() - walkAfterThrowsStart > BOOMERANG_TURTLE_WALKING_TIMEOUT_2) && !isPreparing)
			{
				for (int i = 0; i < boomerangList.size(); i++)
				{
					boomerangList[i]->SetIsVisible(false);
					boomerangList[i]->SetState(BOOMERANG_STATE_IDLING);
				}
				prepareStart = GetTickCount64();
				isPreparing = true;
				boomerangIndex = 0;
				boomerangList[boomerangIndex]->SetIsVisible(true);
			}
		}
		else if (walkAfterThrowsStart < walkBetweenThrowsStart)
		{
			if ((GetTickCount64() - walkBetweenThrowsStart > BOOMERANG_TURTLE_WALKING_TIMEOUT_1) && !isPreparing)
			{
				prepareStart = GetTickCount64();
				isPreparing = true;
				boomerangIndex = 1;
				boomerangList[boomerangIndex]->SetIsVisible(true);
			}
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
		boomerangList[boomerangIndex]->SetNx(nx);
		boomerangList[boomerangIndex]->SetState(BOOMERANG_STATE_SWINGING_UP);
	}

	if (fabs(x - x00) >= BOOMERANG_TURTLE_MOVE_OFFSET)
	{
		x00 = x;
		vx = -vx;
	}

	int randomNum = rand() % 300;
	if (randomNum == 299)
	{
		vy = -0.3f;
	}

	if (mX < x && state != BOOMERANG_TURTLE_STATE_WALKING_RIGHT)
	{
		SetState(BOOMERANG_TURTLE_STATE_WALKING_RIGHT);
	}
	else if (mX > x && state != BOOMERANG_TURTLE_STATE_WALKING_LEFT)
	{
		SetState(BOOMERANG_TURTLE_STATE_WALKING_LEFT);
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
	else if (state == BOOMERANG_TURTLE_STATE_DIE_LEFT || state == BOOMERANG_TURTLE_STATE_DIE_BY_TAIL_LEFT)
		aniId = ID_ANI_BOOMERANG_TURTLE_DIE_LEFT;
	else if (state == BOOMERANG_TURTLE_STATE_DIE_RIGHT || state == BOOMERANG_TURTLE_STATE_DIE_BY_TAIL_RIGHT)
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
		vx = 0.0f;
		vy = -0.2f;
		ay = BOOMERANG_TURTLE_GRAVITY;
		isDead = 1;
		break;
	case BOOMERANG_TURTLE_STATE_DIE_LEFT:
		dieStart = GetTickCount64();
		vx = 0.0f;
		vy = -0.2f;
		ay = BOOMERANG_TURTLE_GRAVITY;
		isDead = 1;
		break;
	case BOOMERANG_TURTLE_STATE_DIE_BY_TAIL_RIGHT:
		dieStart = GetTickCount64();
		vx = 0.0f;
		vy = -0.5f;
		ay = BOOMERANG_TURTLE_GRAVITY;
		isDead = 1;
		break;
	case BOOMERANG_TURTLE_STATE_DIE_BY_TAIL_LEFT:
		dieStart = GetTickCount64();
		vx = 0.0f;
		vy = -0.5f;
		ay = BOOMERANG_TURTLE_GRAVITY;
		isDead = 1;
		break;
	case BOOMERANG_TURTLE_STATE_WALKING_RIGHT:
		vx = BOOMERANG_TURTLE_WALKING_SPEED;
		isPreparing = false;
		x00 = x;
		nx = 1;
		for (int i = 0; i < boomerangList.size(); i++)
			if (boomerangList[i]->IsIdling()) boomerangList[i]->SetNx(nx);
		break;
	case BOOMERANG_TURTLE_STATE_WALKING_LEFT:
		vx = -BOOMERANG_TURTLE_WALKING_SPEED;
		isPreparing = false;
		x00 = x;
		nx = -1;
		for (int i = 0; i < boomerangList.size(); i++)
			if (boomerangList[i]->IsIdling()) boomerangList[i]->SetNx(nx);
		break;
	}
}
