#include "WingedKoopa.h"
#include "Particle.h"
#include "LifeBrick.h"
#include "CoinBrick.h"
#include "BuffQBlock.h"
#include "CoinQBlock.h"
#include "WingedGoomba.h"
#include "PiranhaPlant.h"
#include "Platform.h"
#include "Box.h"
#include "Mario.h"
#include "CoinBrick.h"
#include "ActivatorBrick.h"

#include "PlayScene.h"

#include "debug.h"

CWingedKoopa::CWingedKoopa(int id, float x, float y, int z, int originalChunkId, int nx, bool isWinged) : CGameObject(id, x, y, z)
{
	this->originalChunkId = originalChunkId;
	this->ax = 0;
	this->ay = WINGED_KOOPA_GRAVITY;
	this->nx = nx;
	this->isWinged = isWinged;
	x0 = x;
	y0 = y;
	shellStart = -1;
	if (nx > 0)
		SetState(WINGED_KOOPA_STATE_MOVING_RIGHT);
	else
		SetState(WINGED_KOOPA_STATE_MOVING_LEFT);
}

void CWingedKoopa::GetBoundingBox(float& left, float& top, float& right, float& bottom)  
{  
   if (state == WINGED_KOOPA_STATE_MOVING_LEFT || state == WINGED_KOOPA_STATE_MOVING_RIGHT)  
   {  
       left = x - WINGED_KOOPA_BBOX_WIDTH / 2;  
       right = left + WINGED_KOOPA_BBOX_WIDTH;  
       bottom = y + WINGED_KOOPA_TEXTURE_HEIGHT / 2;  
       top = bottom - WINGED_KOOPA_BBOX_HEIGHT;  
   }  
   else // Shell states (including being held, dynamic, static)  
   {  
       left = x - WINGED_KOOPA_BBOX_WIDTH / 2;  
       top = y - WINGED_KOOPA_BBOX_HEIGHT / 2;  
       right = left + WINGED_KOOPA_BBOX_WIDTH;  
       bottom = top + WINGED_KOOPA_BBOX_HEIGHT;  
   }  
}

void CWingedKoopa::OnNoCollision(DWORD dt)
{
	x += vx * dt;
	y += vy * dt;
}

void CWingedKoopa::OnCollisionWith(LPCOLLISIONEVENT e)
{
	if (state == WINGED_KOOPA_STATE_DIE_ON_COLLIDE_WITH_ENEMY) return;

	if (e->ny < 0)
	{
		if (isWinged)
		{
			vy = WINGED_KOOPA_BOUNCING_SPEED_Y;
			ay = WINGED_KOOPA_GRAVITY / 1.5;
		}
		else
			vy = 0;
	}

	if (e->nx != 0 && e->obj->IsBlocking())
	{
		if (state == WINGED_KOOPA_STATE_SHELL_DYNAMIC)
		{
			vx = -vx;
			nx = -nx;
		}
		else
		{
			this->SetState((vx > 0) ? WINGED_KOOPA_STATE_MOVING_LEFT : WINGED_KOOPA_STATE_MOVING_RIGHT);
		}
	}

	if (dynamic_cast<CCoinQBlock*>(e->obj) || dynamic_cast<CBuffQBlock*>(e->obj))
		OnCollisionWithQuestionBlock(e);
	else if (dynamic_cast<CLifeBrick*>(e->obj))
		OnCollisionWithLifeBrick(e);
	else if (dynamic_cast<CGoomba*>(e->obj))
		OnCollisionWithGoomba(e);
	else if (dynamic_cast<CWingedGoomba*>(e->obj))
		OnCollisionWithWingedGoomba(e);
	else if (dynamic_cast<CPiranhaPlant*>(e->obj))
		OnCollisionWithPiranhaPlant(e);
	else if (dynamic_cast<CCoinBrick*>(e->obj))
		OnCollisionWithCoinBrick(e);
	else if (dynamic_cast<CActivatorBrick*>(e->obj))
		OnCollisionWithActivatorBrick(e);
	else if (dynamic_cast<CWingedKoopa*>(e->obj))
		OnCollisionWithWingedKoopa(e);
	else if (dynamic_cast<CKoopa*>(e->obj))
		OnCollisionWithKoopa(e);
}

void CWingedKoopa::OnCollisionWithQuestionBlock(LPCOLLISIONEVENT e)
{
	if (e->nx != 0)
	{
		if (state == WINGED_KOOPA_STATE_MOVING_LEFT)
		{
			this->SetState(WINGED_KOOPA_STATE_MOVING_RIGHT);
			return;
		}
		else if (state == WINGED_KOOPA_STATE_MOVING_RIGHT)
		{
			this->SetState(WINGED_KOOPA_STATE_MOVING_LEFT);
			return;
		}
	}

	bool isAbleToBounce = ((isWinged) && (state == KOOPA_STATE_WALKING_LEFT || state == KOOPA_STATE_WALKING_RIGHT || state == KOOPA_STATE_SHELL_STATIC));
	if (isAbleToBounce)
	{
		bool isHit = dynamic_cast<CCoinQBlock*>(e->obj)
			? dynamic_cast<CCoinQBlock*>(e->obj)->IsHit()
			: dynamic_cast<CBuffQBlock*>(e->obj)->IsHit();
		bool isBouncing = dynamic_cast<CCoinQBlock*>(e->obj)
			? (dynamic_cast<CCoinQBlock*>(e->obj)->GetState() != QUESTIONBLOCK_STATE_NOT_HIT)
			: (dynamic_cast<CBuffQBlock*>(e->obj)->GetState() != QUESTIONBLOCK_STATE_NOT_HIT);
		if (isBouncing && !isHit)
		{
			int preState = state;
			this->StartShell();
			this->SetState(WINGED_KOOPA_STATE_SHELL_STATIC);
			float knockbackVx = (preState == WINGED_KOOPA_STATE_MOVING_RIGHT) ? 0.1f : -0.1f; // Bounce left for walking left and shell static
			float knockbackVy = -0.5f;
			this->SetIsFlying(true);
			this->SetIsReversed(true);
			this->SetSpeed(knockbackVx, knockbackVy);
		}
		return;
	}

	if (state == WINGED_KOOPA_STATE_SHELL_DYNAMIC)
	{
		if (CCoinQBlock* cqb = dynamic_cast<CCoinQBlock*>(e->obj))
		{
			if (e->nx != 0 && cqb->GetState() == QUESTIONBLOCK_STATE_NOT_HIT)
			{
				cqb->SetState(QUESTIONBLOCK_STATE_BOUNCE_UP);
			}
		}
		else if (CBuffQBlock* bqb = dynamic_cast<CBuffQBlock*>(e->obj))
		{
			if (e->nx != 0 && bqb->GetState() == QUESTIONBLOCK_STATE_NOT_HIT)
			{
				CMario* player = (CMario*)(((LPPLAYSCENE)CGame::GetInstance()->GetCurrentScene())->GetPlayer());
				if (player->GetLevel() == MARIO_LEVEL_SMALL)
				{
					CMushroom* mushroom = bqb->GetMushroom();
					if (mushroom)
					{
						float mX, mY;
						mushroom->GetPosition(mX, mY);
						if (x < mX)
							mushroom->SetCollisionNx(1);
						else
							mushroom->SetCollisionNx(-1);
					}
					bqb->SetToSpawn(0);
				}
				else
				{
					bqb->SetToSpawn(1);
				}
				bqb->SetState(QUESTIONBLOCK_STATE_BOUNCE_UP);
			}
		}
	}
}


void CWingedKoopa::OnCollisionWithLifeBrick(LPCOLLISIONEVENT e)
{
	CLifeBrick* lb = dynamic_cast<CLifeBrick*>(e->obj);
	bool isAbleToBounce = ((isWinged) && (state == KOOPA_STATE_WALKING_LEFT || state == KOOPA_STATE_WALKING_RIGHT || state == KOOPA_STATE_SHELL_STATIC));
	if (isAbleToBounce)
	{
		bool isHit = lb->IsHit();
		bool isBouncing = lb->GetState() != QUESTIONBLOCK_STATE_NOT_HIT;
		if (isBouncing && !isHit)
		{
			int preState = state;
			this->StartShell();
			this->SetState(WINGED_KOOPA_STATE_SHELL_STATIC);
			float knockbackVx = (preState == WINGED_KOOPA_STATE_MOVING_RIGHT) ? 0.1f : -0.1f; // Bounce left for walking left and shell static
			float knockbackVy = -0.5f;
			this->SetIsFlying(true);
			this->SetIsReversed(true);
			this->SetSpeed(knockbackVx, knockbackVy);
		}
		return;
	}

	if (state == WINGED_KOOPA_STATE_SHELL_DYNAMIC)
	{
		if (e->nx != 0 && lb->GetState() == QUESTIONBLOCK_STATE_NOT_HIT)
		{
			lb->SetState(QUESTIONBLOCK_STATE_BOUNCE_UP);
		}
	}
}

void CWingedKoopa::OnCollisionWithCoinBrick(LPCOLLISIONEVENT e)
{
	CCoinBrick* cb = dynamic_cast<CCoinBrick*>(e->obj);
	bool isAbleToBounce = ((isWinged) && (state == KOOPA_STATE_WALKING_LEFT || state == KOOPA_STATE_WALKING_RIGHT || state == KOOPA_STATE_SHELL_STATIC));
	if (isAbleToBounce)
	{
		if (e->obj->GetState() == COIN_BRICK_STATE_BREAK)
		{
			int preState = state;
			this->StartShell();
			this->SetState(WINGED_KOOPA_STATE_SHELL_STATIC);
			float knockbackVx = (preState == WINGED_KOOPA_STATE_MOVING_RIGHT) ? 0.1f : -0.1f; // Bounce left for walking left and shell static
			float knockbackVy = -0.5f;
			this->SetIsFlying(true);
			this->SetIsReversed(true);
			this->SetSpeed(knockbackVx, knockbackVy);
		}
		return;
	}

	if (state == WINGED_KOOPA_STATE_SHELL_DYNAMIC)
	{
		if (e->nx != 0)
		{
			cb->SetState(COIN_BRICK_STATE_BREAK);
		}
	}
}

void CWingedKoopa::OnCollisionWithActivatorBrick(LPCOLLISIONEVENT e)
{
	CActivatorBrick* ab = dynamic_cast<CActivatorBrick*>(e->obj);
	bool isAbleToBounce = ((isWinged) && (state == KOOPA_STATE_WALKING_LEFT || state == KOOPA_STATE_WALKING_RIGHT || state == KOOPA_STATE_SHELL_STATIC));
	if (isAbleToBounce)
	{
		bool isHit = ab->IsHit();
		bool isBouncing = ab->GetState() != QUESTIONBLOCK_STATE_NOT_HIT;
		if (isBouncing && !isHit)
		{
			int preState = state;
			this->StartShell();
			this->SetState(WINGED_KOOPA_STATE_SHELL_STATIC);
			float knockbackVx = (preState == WINGED_KOOPA_STATE_MOVING_RIGHT) ? 0.1f : -0.1f; // Bounce left for walking left and shell static
			float knockbackVy = -0.5f;
			this->SetIsFlying(true);
			this->SetIsReversed(true);
			this->SetSpeed(knockbackVx, knockbackVy);
		}
		return;
	}

	if (state == WINGED_KOOPA_STATE_SHELL_DYNAMIC)
	{
		if (e->nx != 0 && ab->GetState() == QUESTIONBLOCK_STATE_NOT_HIT)
		{
			ab->SetState(QUESTIONBLOCK_STATE_BOUNCE_UP);
		}
	}
}

void CWingedKoopa::OnCollisionWithGoomba(LPCOLLISIONEVENT e)
{
	CGoomba* g = dynamic_cast<CGoomba*>(e->obj);
	if (g->IsDead() || g->IsDefeated())
		return;

	if (this->state == WINGED_KOOPA_STATE_SHELL_DYNAMIC)
	{
		if (g->IsDead() == 0)
		{
			g->SetState(GOOMBA_STATE_DIE_ON_TAIL_WHIP);
		}
	}
	else if (this->state != WINGED_KOOPA_STATE_SHELL_STATIC)
	{
		if (e->nx != 0)
		{
			this->SetState((vx > 0) ? WINGED_KOOPA_STATE_MOVING_LEFT : WINGED_KOOPA_STATE_MOVING_RIGHT);
			float eVx, eVy;
			g->GetSpeed(eVx, eVy);
			g->SetSpeed(-eVx, eVy);
		}
	}
}

void CWingedKoopa::OnCollisionWithWingedGoomba(LPCOLLISIONEVENT e)
{
	CWingedGoomba* wg = dynamic_cast<CWingedGoomba*>(e->obj);
	if (this->state == WINGED_KOOPA_STATE_SHELL_DYNAMIC)
	{
		if (wg->IsDead() == 0)
		{
			wg->SetState(WINGED_GOOMBA_STATE_DIE_ON_TAIL_WHIP);
		}
	}
	else if (this->state != WINGED_KOOPA_STATE_SHELL_STATIC)
	{
		if (e->nx != 0 && wg->IsWinged() == 0)
		{
			this->SetState((vx > 0) ? WINGED_KOOPA_STATE_MOVING_LEFT : WINGED_KOOPA_STATE_MOVING_RIGHT);
			float eVx, eVy;
			wg->GetSpeed(eVx, eVy);
			wg->SetSpeed(-eVx, eVy);
		}
	}
}

void CWingedKoopa::OnCollisionWithKoopa(LPCOLLISIONEVENT e)
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

void CWingedKoopa::OnCollisionWithWingedKoopa(LPCOLLISIONEVENT e)
{
	if (this == e->obj)
		return;
	CWingedKoopa* wk = dynamic_cast<CWingedKoopa*>(e->obj);
	if (wk->IsHeld())
	{
		CMario* player = (CMario*)((LPPLAYSCENE)CGame::GetInstance()->GetCurrentScene())->GetPlayer();
		player->SetIsHoldingKoopa(0);
		wk->SetIsHeld(0);

		wk->SetState(KOOPA_STATE_DIE_ON_COLLIDE_WITH_ENEMY);
		this->SetState(KOOPA_STATE_DIE_ON_COLLIDE_WITH_HELD_KOOPA);

		return;
	}
	if (this->state == KOOPA_STATE_SHELL_DYNAMIC && wk->GetState() != KOOPA_STATE_SHELL_DYNAMIC)
	{
		if (!wk->IsDead())
		{
			wk->SetState(KOOPA_STATE_DIE_ON_COLLIDE_WITH_DYNAMIC_KOOPA);
		}
		return;
	}
	if ((this->state == KOOPA_STATE_WALKING_LEFT || this->state == KOOPA_STATE_WALKING_RIGHT)
		&& (wk->GetState() == KOOPA_STATE_WALKING_LEFT || wk->GetState() == KOOPA_STATE_WALKING_RIGHT))
	{
		if (e->nx != 0)
		{
			this->SetState((vx > 0) ? KOOPA_STATE_WALKING_LEFT : KOOPA_STATE_WALKING_RIGHT);
			wk->SetState((wk->GetNx() > 0) ? KOOPA_STATE_WALKING_LEFT : KOOPA_STATE_WALKING_RIGHT);
		}
		return;
	}
}

void CWingedKoopa::OnCollisionWithPiranhaPlant(LPCOLLISIONEVENT e)
{
	CPiranhaPlant* pp = dynamic_cast<CPiranhaPlant*>(e->obj);
	if (this->state == WINGED_KOOPA_STATE_SHELL_DYNAMIC)
	{
		if (pp->GetState() != PIRANHA_PLANT_STATE_DIE && pp->GetState() != PIRANHA_PLANT_STATE_HIDDEN)
		{
			pp->SetState(PIRANHA_PLANT_STATE_DIE);
		}
	}
}

bool CWingedKoopa::CheckCollisionWithTerrain(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	// Check for collision with blocking objects
	vector<LPCOLLISIONEVENT> coEvents;

	coEvents.clear();
	CCollision::GetInstance()->Scan(this, dt, coObjects, coEvents);

	bool collideWithTerrain = false;
	for (size_t i = 0; i < coEvents.size(); i++)
	{
		LPCOLLISIONEVENT e = coEvents[i];
		if (e->obj->IsBlocking())
		{
			collideWithTerrain = true;
			break;
		}
	}

	for (size_t i = 0; i < coEvents.size(); i++) delete coEvents[i];
	return collideWithTerrain;
}

void CWingedKoopa::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	DebugOutTitle(L"vx:%f vy:%f if:%d\n", vx, vy, (int)isFlying);

	if (isDefeated == 1)
		return;

	if (isWinged == 1) {
		int flapInterval = 500;
		if (GetTickCount64() - flapStart > flapInterval) {
			flapStart = GetTickCount64();
			wingState = 1 - wingState;
		}
	}

	if (isHeld == 1)
	{
		vx = 0;
		vy = 0;

		vector<CChunk*> chunkList = dynamic_cast<CPlayScene*>(CGame::GetInstance()->GetCurrentScene())->GetChunks();

		LPCHUNK currentChunk = nullptr;

		for (auto chunk : chunkList)
		{
			if (!chunk->IsLoaded()) continue;
			for (auto obj : chunk->GetObjects())
			{
				if (obj == this) currentChunk = chunk;
			}
			if (x < chunk->GetEndX() && x > chunk->GetStartX() && currentChunk != nullptr && chunk != currentChunk)
			{
				currentChunk->RemoveObject(this);
				chunk->AddObject(this);
				chunk->AddEnemy(this);
			}
		}
	}

	if ((isDead == 1) && (GetTickCount64() - dieStart > WINGED_KOOPA_DIE_TIMEOUT))
	{
		LPCHUNK chunk = ((LPPLAYSCENE)CGame::GetInstance()->GetCurrentScene())->GetChunk(originalChunkId);
		chunk->SetIsObjectDeleted(this->GetId(), true);
		isDeleted = true;
		return;
	}

	if (isFlying && vy == 0)
	{
		vx = 0;
		isFlying = false;
		return;
	}

	CMario* player = (CMario*)((LPPLAYSCENE)CGame::GetInstance()->GetCurrentScene())->GetPlayer();

	// Get out of shell
	if ((state == WINGED_KOOPA_STATE_SHELL_STATIC || state == WINGED_KOOPA_STATE_BEING_HELD)
		&& !isKicked && (GetTickCount64() - shellStart > WINGED_KOOPA_SHELL_TIMEOUT))
	{
		if (player->IsRunning() && isHeld)
		{
			isHeld = false;
			player->SetIsHoldingKoopa(false);

			if (CheckCollisionWithTerrain(dt, coObjects))
			{
				this->SetState(WINGED_KOOPA_STATE_DIE_ON_COLLIDE_WITH_TERRAIN);
				player->StartKick();
				return;
			}

			switch (player->GetLevel())
			{
			case MARIO_LEVEL_TAIL:
				player->SetState(MARIO_STATE_TAIL_DOWN);
				break;
			case MARIO_LEVEL_BIG:
				player->SetState(MARIO_STATE_POWER_DOWN);
				break;
			case MARIO_LEVEL_SMALL:
				player->SetState(MARIO_STATE_DIE_ON_BEING_KILLED);
				break;
			}
		}
		y -= 6; // RED ALLERT
		this->SetState((player->GetNx() < 0) ? WINGED_KOOPA_STATE_MOVING_RIGHT : WINGED_KOOPA_STATE_MOVING_LEFT);
	}

	if (isHeld == 1)
	{
		if (player->IsRunning() == 0)
		{
			this->SetState(WINGED_KOOPA_STATE_SHELL_DYNAMIC);
			this->SetNx(player->GetNx());
			this->SetSpeed((nx > 0) ? WINGED_KOOPA_SHELL_SPEED : -WINGED_KOOPA_SHELL_SPEED, 0);
			isHeld = false;
			player->SetIsHoldingKoopa(false);
			player->StartKick();

			if (CheckCollisionWithTerrain(dt, coObjects))
			{
				SetState(WINGED_KOOPA_STATE_DIE_ON_COLLIDE_WITH_TERRAIN);
			}

			return;
		}

		float mX, mY;
		player->GetPosition(mX, mY);
		int mNx = player->GetNx();
		if (player->GetLevel() == MARIO_LEVEL_TAIL)
		{
			if (mNx > 0)
				x = mX + 12;
			else
				x = mX - 12;
			y = mY + 1;
		}
		else if (player->GetLevel() == MARIO_LEVEL_BIG)
		{
			if (mNx > 0)
				x = mX + 10;
			else
				x = mX - 10;
			y = mY + 1;
		}
		else if (player->GetLevel() == MARIO_LEVEL_SMALL)
		{
			if (mNx > 0)
				x = mX + 8;
			else
				x = mX - 8;
			y = mY - 1;
		}
		return;
	}

	if (isWinged)
		ay -= WINGED_KOOPA_GRAVITY_DECREMENT;

	vy += ay * dt;
	vx += ax * dt;

	if (state == WINGED_KOOPA_STATE_SHELL_DYNAMIC) {
		vx = (nx > 0) ? WINGED_KOOPA_SHELL_SPEED : -WINGED_KOOPA_SHELL_SPEED;
	}
	else if (state == WINGED_KOOPA_STATE_MOVING_RIGHT) {
		vx = (isWinged ? WINGED_KOOPA_BOUNCING_SPEED_X : WINGED_KOOPA_WALKING_SPEED);
	}
	else if (state == WINGED_KOOPA_STATE_MOVING_LEFT) {
		vx = (isWinged ? -WINGED_KOOPA_BOUNCING_SPEED_X : - WINGED_KOOPA_WALKING_SPEED);
	}

	CGameObject::Update(dt, coObjects);
	CCollision::GetInstance()->Process(this, dt, coObjects);
}

void CWingedKoopa::StartShell()
{
	shellStart = GetTickCount64();
	isKicked = false;
	SetState(WINGED_KOOPA_STATE_SHELL_STATIC);
}

void CWingedKoopa::Render()
{
	if (isDefeated == 1)
		return;

	int aniId;
	if (state == WINGED_KOOPA_STATE_SHELL_STATIC || state == WINGED_KOOPA_STATE_BEING_HELD)
	{
		if (!isKicked && GetTickCount64() - shellStart > WINGED_KOOPA_SHELL_ALERT_TIMEOUT)
		{
			if (isReversed)
				aniId = ID_ANI_WINGED_KOOPA_SHELL_STATIC_REVERSE_2;
			else
				aniId = ID_ANI_WINGED_KOOPA_SHELL_STATIC_2;
		}
		else
		{
			if (isReversed)
				aniId = ID_ANI_WINGED_KOOPA_SHELL_STATIC_REVERSE_1;
			else
				aniId = ID_ANI_WINGED_KOOPA_SHELL_STATIC_1;
		}
	}
	else if (state == WINGED_KOOPA_STATE_SHELL_DYNAMIC)
	{
		if (isReversed)
			aniId = ID_ANI_WINGED_KOOPA_SHELL_DYNAMIC_REVERSE;
		else
			aniId = ID_ANI_WINGED_KOOPA_SHELL_DYNAMIC;
	}
	else if (state == WINGED_KOOPA_STATE_MOVING_LEFT)
		aniId = ID_ANI_WINGED_KOOPA_MOVING_LEFT;
	else aniId = ID_ANI_WINGED_KOOPA_MOVING_RIGHT;

	if (isDead)
	{
		aniId = ID_ANI_WINGED_KOOPA_SHELL_STATIC_REVERSE_1;
	}

	CAnimations::GetInstance()->Get(aniId)->Render(x, y);
	//RenderBoundingBox();

	if (isWinged && !isDead)
	{
		if (wingState == 0)
		{
			if (nx > 0) 
				CAnimations::GetInstance()->Get(ID_ANI_WING_FLAP_LEFT)->Render(x - WINGED_KOOPA_BBOX_WIDTH / 2 + 1, y - GOOMBA_BBOX_HEIGHT / 2);
			else
				CAnimations::GetInstance()->Get(ID_ANI_WING_FLAP_RIGHT)->Render(x + WINGED_KOOPA_BBOX_WIDTH / 2, y - GOOMBA_BBOX_HEIGHT / 2);
		}
		else
		{
			if (nx > 0)
				CAnimations::GetInstance()->Get(ID_ANI_WING_CLOSE_LEFT)->Render(x - WINGED_KOOPA_BBOX_WIDTH / 2 + 1, y - GOOMBA_BBOX_HEIGHT / 2 + 2);
			else
				CAnimations::GetInstance()->Get(ID_ANI_WING_CLOSE_RIGHT)->Render(x + WINGED_KOOPA_BBOX_WIDTH / 2, y - GOOMBA_BBOX_HEIGHT / 2 + 2);
		}
	}
}
void CWingedKoopa::SetState(int state)
{
	if (isDead == 1) return;

	// If transitioning from shell to walking state
	if ((this->state == WINGED_KOOPA_STATE_SHELL_STATIC || this->state == WINGED_KOOPA_STATE_SHELL_DYNAMIC) &&
		(state == WINGED_KOOPA_STATE_MOVING_LEFT || state == WINGED_KOOPA_STATE_MOVING_RIGHT))
	{
		// Adjust y position to account for hitbox difference
		// In shell: hitbox is centered (height/2 above and below y)
		// In walking: hitbox extends from bottom up
		float heightDifference = (WINGED_KOOPA_TEXTURE_HEIGHT - WINGED_KOOPA_BBOX_HEIGHT) / 2.0f;
		y -= heightDifference; // Move Koopa up to compensate
	}

	switch (state)
	{
	case WINGED_KOOPA_STATE_DIE_ON_COLLIDE_WITH_ENEMY:
	{
		dieStart = GetTickCount64();
		CMario* player = (CMario*)((LPPLAYSCENE)CGame::GetInstance()->GetCurrentScene())->GetPlayer();
		CParticle::GenerateParticleInChunk(this, 0, 100);
		CGame::GetInstance()->GetGameState()->AddScore(100);
		vx = (player->GetNx() > 0) ? 0.2f : -0.2f;
		vy = -0.35f;
		ax = 0;
		ay = WINGED_KOOPA_GRAVITY;
		isDead = 1;
		break;
	}
	case WINGED_KOOPA_STATE_DIE_ON_COLLIDE_WITH_TERRAIN:
		dieStart = GetTickCount64();
		CParticle::GenerateParticleInChunk(this, 0, 100);
		CGame::GetInstance()->GetGameState()->AddScore(100);
		vx = 0;
		vy = -0.4f;
		ax = 0;
		ay = WINGED_KOOPA_GRAVITY;
		isDead = 1;
		break;
	case KOOPA_STATE_DIE_ON_COLLIDE_WITH_DYNAMIC_KOOPA:
		dieStart = GetTickCount64();
		vy = -0.5f;
		ay = WINGED_KOOPA_GRAVITY;
		isDead = 1;
		break;
	case KOOPA_STATE_DIE_ON_COLLIDE_WITH_HELD_KOOPA:
		dieStart = GetTickCount64();
		vy = -0.35f;
		ay = WINGED_KOOPA_GRAVITY;
		isDead = 1;
		break;
	case WINGED_KOOPA_STATE_SHELL_STATIC:
		vx = 0;
		vy = 0;
		ax = 0;
		break;
	case WINGED_KOOPA_STATE_SHELL_DYNAMIC:
		isKicked = true;
		//vy = 0;
		//ax = 0;
		break;
	case WINGED_KOOPA_STATE_MOVING_LEFT:
		if (GetTickCount64() - lastTurnAroundTime < WINGED_KOOPA_TURNAROUND_TIMEOUT)
			return;
		lastTurnAroundTime = GetTickCount64();
		if (this->state == WINGED_KOOPA_STATE_SHELL_STATIC && vy != 0)
			return;
		isKicked = false;
		vx = (isWinged ? -WINGED_KOOPA_BOUNCING_SPEED_X : - WINGED_KOOPA_WALKING_SPEED);
		nx = -1;
		break;
	case WINGED_KOOPA_STATE_MOVING_RIGHT:
		if (GetTickCount64() - lastTurnAroundTime < WINGED_KOOPA_TURNAROUND_TIMEOUT)
			return;
		lastTurnAroundTime = GetTickCount64();
		if (this->state == WINGED_KOOPA_STATE_SHELL_STATIC && vy != 0)
			return;
		isKicked = false;
		vx = (isWinged ? WINGED_KOOPA_BOUNCING_SPEED_X : WINGED_KOOPA_WALKING_SPEED);
		nx = 1;
		break;
	case WINGED_KOOPA_STATE_BEING_HELD:
		isHeld = 1;
		isFlying = false;
		break;
	}
	CGameObject::SetState(state);

	//DebugOut(L"[INFO] Koopa: state=%d\n", state);
}