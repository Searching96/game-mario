#include "FlyingKoopa.h"
#include "entities/enemies/Koopa.h"
#include "engine/rendering/Particle.h"
#include "world/Platform.h"
#include "entities/blocks/Box.h"
#include "entities/player/Mario.h"
#include "entities/enemies/WingedGoomba.h"

#include "core/PlayScene.h"

#include "debug/debug.h"

CFlyingKoopa::CFlyingKoopa(int id, float x, float y, int z, int originalChunkId) : CGameObject(id, x, y, z)
{
	this->originalChunkId = originalChunkId;
	this->ax = 0;
	this->ay = 0;
	x0 = x;
	y0 = y;
	SetState(FLYING_KOOPA_STATE_FLYING_DOWN);
}

void CFlyingKoopa::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	if (state == KOOPA_STATE_WALKING_LEFT || state == KOOPA_STATE_WALKING_RIGHT)
	{
		left = x - KOOPA_BBOX_WIDTH / 2;
		right = left + KOOPA_BBOX_WIDTH;
		bottom = y + KOOPA_TEXTURE_HEIGHT / 2;
		top = bottom - KOOPA_BBOX_HEIGHT;
	}
	else // Shell states (including being held, dynamic, static)
	{
		left = x - KOOPA_BBOX_WIDTH / 2;
		top = y - KOOPA_BBOX_HEIGHT / 2;
		right = left + KOOPA_BBOX_WIDTH;
		bottom = top + KOOPA_BBOX_HEIGHT;
	}
}

void CFlyingKoopa::OnNoCollision(DWORD dt)
{
	x += vx * dt;
	y += vy * dt;
}

void CFlyingKoopa::OnCollisionWith(LPCOLLISIONEVENT e)
{
	if (state == KOOPA_STATE_DIE_ON_COLLIDE_WITH_ENEMY) return;

	if (e->ny < 0) 
		vy = 0;

	if (e->obj->IsBlocking() && (state == KOOPA_STATE_SHELL_STATIC) && (vx != 0)) 
		vx = 0;

	if (dynamic_cast<CKoopa*>(e->obj))
		OnCollisionWithKoopa(e);
}

void CFlyingKoopa::OnCollisionWithKoopa(LPCOLLISIONEVENT e)
{
	if (this == e->obj)
		return;
	CKoopa* k = dynamic_cast<CKoopa*>(e->obj);
	if (k->IsHeld())
	{
		CMario* player = (CMario*)((LPPLAYSCENE)CGame::GetInstance()->GetCurrentScene())->GetPlayer();
		player->SetIsHoldingKoopa(0);
		k->SetIsHeld(0);

		k->SetState(KOOPA_STATE_DIE_ON_COLLIDE_WITH_ENEMY);
		this->SetState(KOOPA_STATE_DIE_ON_COLLIDE_WITH_HELD_KOOPA);

		return;
	}
	if (this->state == KOOPA_STATE_SHELL_DYNAMIC && k->GetState() != KOOPA_STATE_SHELL_DYNAMIC)
	{
		if (k->IsDead() == 0)
		{
			k->SetState(KOOPA_STATE_DIE_ON_COLLIDE_WITH_DYNAMIC_KOOPA);
		}
		return;
	}
	if ((this->state == KOOPA_STATE_WALKING_LEFT || this->state == KOOPA_STATE_WALKING_RIGHT)
		&& (k->GetState() == KOOPA_STATE_WALKING_LEFT || k->GetState() == KOOPA_STATE_WALKING_RIGHT))
	{
		if (e->nx != 0)
		{
			this->SetState((vx > 0) ? KOOPA_STATE_WALKING_LEFT : KOOPA_STATE_WALKING_RIGHT);
			k->SetState((k->GetNx() > 0) ? KOOPA_STATE_WALKING_LEFT : KOOPA_STATE_WALKING_RIGHT);
		}
		return;
	}
}

void CFlyingKoopa::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	if (isDefeated == 1)
		return;

	if (isWinged == 1) {
		int flapInterval = 100;
		if (GetTickCount64() - flapStart > flapInterval) {
			flapStart = GetTickCount64();
			wingState = 1 - wingState;
		}
	}

	if (fabs(y - y00) >= FLYING_KOOPA_MOVE_OFFSET)
	{
		if (state == FLYING_KOOPA_STATE_FLYING_DOWN)
		{
			y = y00 + FLYING_KOOPA_MOVE_OFFSET;
			SetState(FLYING_KOOPA_STATE_FLYING_UP);
		}
		else if (state == FLYING_KOOPA_STATE_FLYING_UP)
		{
			y = y00 - FLYING_KOOPA_MOVE_OFFSET;
			SetState(FLYING_KOOPA_STATE_FLYING_DOWN);
		}
	}

	if ((isDead == 1) && (GetTickCount64() - dieStart > KOOPA_DIE_TIMEOUT))
	{
		LPCHUNK chunk = ((LPPLAYSCENE)CGame::GetInstance()->GetCurrentScene())->GetChunk(originalChunkId);
		chunk->SetIsObjectDeleted(this->GetId(), true);
		isDeleted = true;
		return;
	}

	CMario* player = (CMario*)((LPPLAYSCENE)CGame::GetInstance()->GetCurrentScene())->GetPlayer();

	vy += ay * dt;
	vx += ax * dt;

	if (state == KOOPA_STATE_SHELL_DYNAMIC) 
	{
		vx = (nx > 0) ? KOOPA_SHELL_SPEED : -KOOPA_SHELL_SPEED;
	}

	CGameObject::Update(dt, coObjects);
	CCollision::GetInstance()->Process(this, dt, coObjects);
}

void CFlyingKoopa::StartShell()
{
	shellStart = GetTickCount64();
	isKicked = false;
	SetState(KOOPA_STATE_SHELL_STATIC);
}


void CFlyingKoopa::Render()
{
	if (isDefeated == 1)
		return;

	int aniId = ID_ANI_KOOPA_WALKING_LEFT;
	if (state == KOOPA_STATE_SHELL_STATIC)
	{
		if (isReversed)
			aniId = ID_ANI_KOOPA_SHELL_STATIC_REVERSE_1;
		else
			aniId = ID_ANI_KOOPA_SHELL_STATIC_1;
	}

	if (isDead)
	{
		aniId = ID_ANI_KOOPA_SHELL_STATIC_REVERSE_1;
	}

	if (isWinged && !isDead)
	{
		if (wingState == 0)
		{
			CAnimations::GetInstance()->Get(ID_ANI_WING_FLAP_RIGHT)->Render(x + KOOPA_BBOX_WIDTH / 2, y - GOOMBA_BBOX_HEIGHT / 2);
		}
		else
		{
			CAnimations::GetInstance()->Get(ID_ANI_WING_CLOSE_RIGHT)->Render(x + KOOPA_BBOX_WIDTH / 2, y - GOOMBA_BBOX_HEIGHT / 2 + 2);
		}
	}

	CAnimations::GetInstance()->Get(aniId)->Render(x, y);
	//RenderBoundingBox();
}
void CFlyingKoopa::SetState(int state)
{
	if (isDead == 1) return;

	switch (state)
	{
		case FLYING_KOOPA_STATE_FLYING_DOWN:
		{
			y00 = y;
			vx = 0;
			vy = 0.1f;
			ax = 0;
			ay = 0;
			break;
		}
		case FLYING_KOOPA_STATE_FLYING_UP:
		{
			y00 = y;
			vx = 0;
			vy = -0.1f;
			ax = 0;
			ay = 0;
			break;
		}
		case KOOPA_STATE_DIE_ON_COLLIDE_WITH_DYNAMIC_KOOPA:
		{
			dieStart = GetTickCount64();
			vy = -0.5f;
			ay = KOOPA_GRAVITY;
			isWinged = 0;
			isDead = 1;
			break;
		}
		case KOOPA_STATE_DIE_ON_COLLIDE_WITH_HELD_KOOPA:
		{
			dieStart = GetTickCount64();
			vy = -0.35f;
			ay = KOOPA_GRAVITY;
			isWinged = 0;
			isDead = 1;
			break;
		}
		case KOOPA_STATE_SHELL_STATIC:
		{
			CMario* player = (CMario*)((LPPLAYSCENE)CGame::GetInstance()->GetCurrentScene())->GetPlayer();
			float mX, mY;
			player->GetPosition(mX, mY);
			float knockbackX = 0.05f;
			vx = ((mX < x) ? knockbackX : -knockbackX);
			vy = 0;
			ax = 0;
			ay = KOOPA_GRAVITY / 2;
			break;
		}
	}
	CGameObject::SetState(state);

	//DebugOut(L"[INFO] Koopa: state=%d\n", state);
}