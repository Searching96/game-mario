#include "entities/enemies/Koopa.h"
#include "engine/rendering/Particle.h"
#include "entities/blocks/LifeBrick.h"
#include "entities/blocks/CoinBrick.h"
#include "entities/blocks/BuffQBlock.h"
#include "entities/blocks/CoinQBlock.h"
#include "entities/enemies/WingedGoomba.h"
#include "entities/enemies/PiranhaPlant.h"
#include "world/Platform.h"
#include "entities/blocks/Box.h"
#include "entities/player/Mario.h"
#include "entities/blocks/CoinBrick.h"
#include "entities/blocks/ActivatorBrick.h"
#include "entities/enemies/WingedKoopa.h"
#include "entities/blocks/HiddenCoinBrick.h"
#include "entities/enemies/FlyingKoopa.h"

#include "core/PlayScene.h"

#include "debug/debug.h"

CKoopa::CKoopa(int id, float x, float y, int z, int originalChunkId, int nx) : CGameObject(id, x, y, z)
{
	this->originalChunkId = originalChunkId;
	this->ax = 0;
	this->ay = KOOPA_GRAVITY;
	this->nx = nx;
	x0 = x;
	y0 = y;
	shellStart = -1;
	SetState((nx > 0) ? KOOPA_STATE_WALKING_RIGHT : KOOPA_STATE_WALKING_LEFT);
}

void CKoopa::GetBoundingBox(float& left, float& top, float& right, float& bottom)
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

void CKoopa::OnNoCollision(DWORD dt)
{
	x += vx * dt;
	y += vy * dt;
}

void CKoopa::OnCollisionWith(LPCOLLISIONEVENT e)
{
	if (state == KOOPA_STATE_DIE_ON_COLLIDE_WITH_ENEMY) return;

	if (e->ny < 0)
	{
		vy = 0;
	}

	if (e->nx != 0 && e->obj->IsBlocking())
	{
		if (state == KOOPA_STATE_SHELL_DYNAMIC)
		{
			vx = -vx;
			nx = -nx;
		}
		else if (state != KOOPA_STATE_SHELL_STATIC)
		{
			this->SetState((vx > 0) ? KOOPA_STATE_WALKING_LEFT : KOOPA_STATE_WALKING_RIGHT);
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
	else if (dynamic_cast<CKoopa*>(e->obj))
		OnCollisionWithKoopa(e);
	else if (dynamic_cast<CWingedKoopa*>(e->obj))
		OnCollisionWithWingedKoopa(e);
	else if (dynamic_cast<CHiddenCoinBrick*>(e->obj))
		OnCollisionWithHiddenCoinBrick(e);
	else if (dynamic_cast<CFlyingKoopa*>(e->obj))
		OnCollisionWithFlyingKoopa(e);
}

void CKoopa::OnCollisionWithQuestionBlock(LPCOLLISIONEVENT e)
{
	if (e->nx != 0)
	{
		if (state == KOOPA_STATE_WALKING_LEFT)
		{
			this->SetState(KOOPA_STATE_WALKING_RIGHT);
			return;
		}
		else if (state == KOOPA_STATE_WALKING_RIGHT)
		{
			this->SetState(KOOPA_STATE_WALKING_LEFT);
			return;
		}
	}

	bool isAbleToBounce = (state == KOOPA_STATE_WALKING_LEFT || state == KOOPA_STATE_WALKING_RIGHT || state == KOOPA_STATE_SHELL_STATIC);
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
			this->SetState(KOOPA_STATE_SHELL_STATIC);
			float knockbackVx = (preState == KOOPA_STATE_WALKING_RIGHT) ? 0.1f : -0.1f; // Bounce left for walking left and shell static
			float knockbackVy = -0.5f;
			this->SetIsFlying(true);
			this->SetIsReversed(true);
			this->SetSpeed(knockbackVx, knockbackVy);
		}
		return;
	}

	if (state == KOOPA_STATE_SHELL_DYNAMIC)
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


void CKoopa::OnCollisionWithLifeBrick(LPCOLLISIONEVENT e)
{
	CLifeBrick* lb = dynamic_cast<CLifeBrick*>(e->obj);
	bool isAbleToBounce = (state == KOOPA_STATE_WALKING_LEFT || state == KOOPA_STATE_WALKING_RIGHT || state == KOOPA_STATE_SHELL_STATIC);
	if (isAbleToBounce)
	{
		bool isHit = lb->IsHit();
		bool isBouncing = lb->GetState() != QUESTIONBLOCK_STATE_NOT_HIT;
		if (isBouncing && !isHit)
		{
			int preState = state;
			this->StartShell();
			this->SetState(KOOPA_STATE_SHELL_STATIC);
			float knockbackVx = (preState == KOOPA_STATE_WALKING_RIGHT) ? 0.1f : -0.1f; // Bounce left for walking left and shell static
			float knockbackVy = -0.5f;
			this->SetIsFlying(true);
			this->SetIsReversed(true);
			this->SetSpeed(knockbackVx, knockbackVy);
		}
		return;
	}

	if (state == KOOPA_STATE_SHELL_DYNAMIC)
	{
		if (e->nx != 0 && lb->GetState() == QUESTIONBLOCK_STATE_NOT_HIT)
		{
			lb->SetState(QUESTIONBLOCK_STATE_BOUNCE_UP);
		}
	}
}

void CKoopa::OnCollisionWithCoinBrick(LPCOLLISIONEVENT e)
{
	CCoinBrick* cb = dynamic_cast<CCoinBrick*>(e->obj);
	if (cb->IsRevealed()) return;
	bool isAbleToBounce = (state == KOOPA_STATE_WALKING_LEFT || state == KOOPA_STATE_WALKING_RIGHT || state == KOOPA_STATE_SHELL_STATIC);
	if (isAbleToBounce)
	{
		if (e->obj->GetState() == COIN_BRICK_STATE_BREAK)
		{
			int preState = state;
			this->StartShell();
			this->SetState(KOOPA_STATE_SHELL_STATIC);
			float knockbackVx = (preState == KOOPA_STATE_WALKING_RIGHT) ? 0.1f : -0.1f; // Bounce left for walking left and shell static
			float knockbackVy = -0.5f;
			this->SetIsFlying(true);
			this->SetIsReversed(true);
			this->SetSpeed(knockbackVx, knockbackVy);
		}
		return;
	}

	if (state == KOOPA_STATE_SHELL_DYNAMIC)
	{
		if (e->nx != 0)
		{
			cb->SetState(COIN_BRICK_STATE_BREAK);
		}
	}
}


void CKoopa::OnCollisionWithHiddenCoinBrick(LPCOLLISIONEVENT e)
{
	CHiddenCoinBrick* hcb = dynamic_cast<CHiddenCoinBrick*>(e->obj);
	bool isAbleToBounce = (state == KOOPA_STATE_WALKING_LEFT || state == KOOPA_STATE_WALKING_RIGHT || state == KOOPA_STATE_SHELL_STATIC);
	if (isAbleToBounce)
	{
		if (e->obj->GetState() != QUESTIONBLOCK_STATE_NOT_HIT)
		{
			int preState = state;
			this->StartShell();
			this->SetState(KOOPA_STATE_SHELL_STATIC);
			float knockbackVx = (preState == KOOPA_STATE_WALKING_RIGHT) ? 0.1f : -0.1f; // Bounce left for walking left and shell static
			float knockbackVy = -0.5f;
			this->SetIsFlying(true);
			this->SetIsReversed(true);
			this->SetSpeed(knockbackVx, knockbackVy);
		}
		return;
	}

	//bool isAbleToBounce = (state == KOOPA_STATE_WALKING_LEFT || state == KOOPA_STATE_WALKING_RIGHT || state == KOOPA_STATE_SHELL_STATIC);
	//if (isAbleToBounce)
	//{
	//	bool isHit = dynamic_cast<CCoinQBlock*>(e->obj)
	//		? dynamic_cast<CCoinQBlock*>(e->obj)->IsHit()
	//		: dynamic_cast<CBuffQBlock*>(e->obj)->IsHit();
	//	bool isBouncing = dynamic_cast<CCoinQBlock*>(e->obj)
	//		? (dynamic_cast<CCoinQBlock*>(e->obj)->GetState() != QUESTIONBLOCK_STATE_NOT_HIT)
	//		: (dynamic_cast<CBuffQBlock*>(e->obj)->GetState() != QUESTIONBLOCK_STATE_NOT_HIT);
	//	if (isBouncing && !isHit)
	//	{
	//		int preState = state;
	//		this->StartShell();
	//		this->SetState(KOOPA_STATE_SHELL_STATIC);
	//		float knockbackVx = (preState == KOOPA_STATE_WALKING_RIGHT) ? 0.1f : -0.1f; // Bounce left for walking left and shell static
	//		float knockbackVy = -0.5f;
	//		this->SetIsFlying(true);
	//		this->SetIsReversed(true);
	//		this->SetSpeed(knockbackVx, knockbackVy);
	//	}
	//	return;
	//}

	if (state == KOOPA_STATE_SHELL_DYNAMIC)
	{
		if (e->nx != 0)
		{
			hcb->SetState(QUESTIONBLOCK_STATE_BOUNCE_UP);
		}
	}
}

void CKoopa::OnCollisionWithActivatorBrick(LPCOLLISIONEVENT e)
{
	CActivatorBrick* ab = dynamic_cast<CActivatorBrick*>(e->obj);
	bool isAbleToBounce = (state == KOOPA_STATE_WALKING_LEFT || state == KOOPA_STATE_WALKING_RIGHT || state == KOOPA_STATE_SHELL_STATIC);
	if (isAbleToBounce)
	{
		bool isHit = ab->IsHit();
		bool isBouncing = ab->GetState() != QUESTIONBLOCK_STATE_NOT_HIT;
		if (isBouncing && !isHit)
		{
			int preState = state;
			this->StartShell();
			this->SetState(KOOPA_STATE_SHELL_STATIC);
			float knockbackVx = (preState == KOOPA_STATE_WALKING_RIGHT) ? 0.1f : -0.1f; // Bounce left for walking left and shell static
			float knockbackVy = -0.5f;
			this->SetIsFlying(true);
			this->SetIsReversed(true);
			this->SetSpeed(knockbackVx, knockbackVy);
		}
		return;
	}

	if (state == KOOPA_STATE_SHELL_DYNAMIC)
	{
		if (e->nx != 0 && ab->GetState() == QUESTIONBLOCK_STATE_NOT_HIT)
		{
			ab->SetState(QUESTIONBLOCK_STATE_BOUNCE_UP);
		}
	}
}

void CKoopa::OnCollisionWithGoomba(LPCOLLISIONEVENT e)
{
	CGoomba* g = dynamic_cast<CGoomba*>(e->obj);
	if (this->state == KOOPA_STATE_SHELL_DYNAMIC)
	{
		if (g->IsDead() == 0)
		{
			g->SetState(GOOMBA_STATE_DIE_ON_TAIL_WHIP);
		}
	}
	else if (this->state != KOOPA_STATE_SHELL_STATIC)
	{
		if (e->nx != 0)
		{
			this->SetState((vx > 0) ? KOOPA_STATE_WALKING_LEFT : KOOPA_STATE_WALKING_RIGHT);
			float eVx, eVy;
			g->GetSpeed(eVx, eVy);
			g->SetSpeed(-eVx, eVy);
		}
	}
}

void CKoopa::OnCollisionWithWingedGoomba(LPCOLLISIONEVENT e)
{
	CWingedGoomba* wg = dynamic_cast<CWingedGoomba*>(e->obj);
	if (this->state == KOOPA_STATE_SHELL_DYNAMIC)
	{
		if (wg->IsDead() == 0)
		{
			wg->SetState(WINGED_GOOMBA_STATE_DIE_ON_TAIL_WHIP);
		}
	}
	else if (this->state != KOOPA_STATE_SHELL_STATIC)
	{
		if (e->nx != 0 && wg->IsWinged() == 0)
		{
			this->SetState((vx > 0) ? KOOPA_STATE_WALKING_LEFT : KOOPA_STATE_WALKING_RIGHT);
			float eVx, eVy;
			wg->GetSpeed(eVx, eVy);
			wg->SetSpeed(-eVx, eVy);
		}
	}
}


void CKoopa::OnCollisionWithKoopa(LPCOLLISIONEVENT e)
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

void CKoopa::OnCollisionWithWingedKoopa(LPCOLLISIONEVENT e)
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
		if (wk->IsDead() == 0)
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



void CKoopa::OnCollisionWithFlyingKoopa(LPCOLLISIONEVENT e)
{
	CFlyingKoopa* fk = dynamic_cast<CFlyingKoopa*>(e->obj);
	if (this->state == KOOPA_STATE_SHELL_DYNAMIC && fk->GetState() != KOOPA_STATE_SHELL_DYNAMIC)
	{
		if (fk->IsDead() == 0)
		{
			fk->SetState(KOOPA_STATE_DIE_ON_COLLIDE_WITH_DYNAMIC_KOOPA);
		}
		return;
	}
}

void CKoopa::OnCollisionWithPiranhaPlant(LPCOLLISIONEVENT e)
{
	CPiranhaPlant* pp = dynamic_cast<CPiranhaPlant*>(e->obj);
	if (this->state == KOOPA_STATE_SHELL_DYNAMIC)
	{
		if (pp->GetState() != PIRANHA_PLANT_STATE_DIE && pp->GetState() != PIRANHA_PLANT_STATE_HIDDEN)
		{
			pp->SetState(PIRANHA_PLANT_STATE_DIE);
		}
	}
}

bool CKoopa::IsPlatformEdge(float checkDistance, vector<LPGAMEOBJECT>& coObjects)
{
	float verticalTolerance = 2.5f;   // Use consistent tolerance (was 2.5f in provided code)
	float horizontalTolerance = 2.0f; // For adjacency
	float l, t, r, b;
	GetBoundingBox(l, t, r, b);
	float koopaBottomY = b;
	float direction = (state == KOOPA_STATE_WALKING_LEFT) ? -1.0f : 1.0f;

	// Find all platforms the Koopa is currently standing on
	vector<LPGAMEOBJECT> supportingPlatforms;
	for (const auto& obj : coObjects)
	{
		if (obj == this || obj->IsDeleted() || !obj->IsBlocking()) continue;

		float objL, objT, objR, objB;
		obj->GetBoundingBox(objL, objT, objR, objB);

		if (l < objR && r > objL && // Horizontal overlap
			koopaBottomY >= objT - verticalTolerance && koopaBottomY <= objT + verticalTolerance) // Vertical proximity
		{
			supportingPlatforms.push_back(obj);
		}
	}

	if (supportingPlatforms.empty())
	{
		// DebugOut(L"[INFO] No supporting platform found for Koopa at Y_bottom=%f\n", koopaBottomY);
		return false;
	}

	// Find consecutive platforms
	float combinedLeft = FLT_MAX;
	float combinedRight = -FLT_MAX;
	float combinedTop = 0.0f;

	for (const auto& platform : supportingPlatforms)
	{
		float platformL, platformT, platformR, platformB;
		platform->GetBoundingBox(platformL, platformT, platformR, platformB);

		if (combinedLeft == FLT_MAX)
		{
			combinedLeft = platformL;
			combinedRight = platformR;
			combinedTop = platformT;
			continue;
		}

		if (std::abs(platformT - combinedTop) <= verticalTolerance)
		{
			combinedLeft = min(combinedLeft, platformL);
			combinedRight = max(combinedRight, platformR);
		}
	}

	// Check adjacent platforms
	bool foundAdjacent;
	do
	{
		foundAdjacent = false;
		for (const auto& obj : coObjects)
		{
			if (obj == this || obj->IsDeleted() || !obj->IsBlocking()) continue;
			if (find(supportingPlatforms.begin(), supportingPlatforms.end(), obj) != supportingPlatforms.end()) continue;

			float objL, objT, objR, objB;
			obj->GetBoundingBox(objL, objT, objR, objB);

			bool isLeftAdjacent = (std::abs(objR - combinedLeft) <= horizontalTolerance);
			bool isRightAdjacent = (std::abs(objL - combinedRight) <= horizontalTolerance);
			bool isVerticallyAligned = (std::abs(objT - combinedTop) <= verticalTolerance);

			if (isVerticallyAligned && (isLeftAdjacent || isRightAdjacent))
			{
				combinedLeft = min(combinedLeft, objL);
				combinedRight = max(combinedRight, objR);
				supportingPlatforms.push_back(obj);
				foundAdjacent = true;
			}
		}
	} while (foundAdjacent);

	// Check edge
	float projectedX = x + direction * checkDistance;
	if (direction < 0)
	{
		if (projectedX <= combinedLeft + 0.001f) return true;
	}
	else
	{
		if (projectedX >= combinedRight - 0.001f) return true;
	}

	return false;
}

bool CKoopa::CheckCollisionWithTerrain(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
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

void CKoopa::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	//DebugOutTitle(L"isdf: %d\n", (int)isDefeated);

	if (isDefeated == 1)
		return;

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

	if ((isDead == 1) && (GetTickCount64() - dieStart > KOOPA_DIE_TIMEOUT))
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

	if (vx == 0.1 || vx == -0.1) vx = vx;

	CMario* player = (CMario*)((LPPLAYSCENE)CGame::GetInstance()->GetCurrentScene())->GetPlayer();

	// Get out of shell
	if ((state == KOOPA_STATE_SHELL_STATIC || state == KOOPA_STATE_BEING_HELD)
		&& !isKicked && (GetTickCount64() - shellStart > KOOPA_SHELL_TIMEOUT))
	{
		if (player->IsRunning() && isHeld)
		{
			isHeld = false;
			player->SetIsHoldingKoopa(false);

			if (CheckCollisionWithTerrain(dt, coObjects))
			{
				this->SetState(KOOPA_STATE_DIE_ON_COLLIDE_WITH_TERRAIN);
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
		this->SetState((player->GetNx() < 0) ? KOOPA_STATE_WALKING_RIGHT : KOOPA_STATE_WALKING_LEFT);
	}

	if (isHeld == 1)
	{
		if (player->IsRunning() == 0)
		{
			this->SetState(KOOPA_STATE_SHELL_DYNAMIC);
			this->SetNx(player->GetNx());
			this->SetSpeed((nx > 0) ? KOOPA_SHELL_SPEED : -KOOPA_SHELL_SPEED, 0);
			isHeld = false;
			player->SetIsHoldingKoopa(false);
			player->StartKick();

			if (CheckCollisionWithTerrain(dt, coObjects))
			{
				SetState(KOOPA_STATE_DIE_ON_COLLIDE_WITH_TERRAIN);
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

	vy += ay * dt;
	vx += ax * dt;

	if (state == KOOPA_STATE_SHELL_DYNAMIC) {
		vx = (nx > 0) ? KOOPA_SHELL_SPEED : -KOOPA_SHELL_SPEED;
	}
	else if (state == KOOPA_STATE_WALKING_LEFT) {
		if (IsPlatformEdge(0.1f, *coObjects)) {
			SetState(KOOPA_STATE_WALKING_RIGHT);
		}
	}
	else if (state == KOOPA_STATE_WALKING_RIGHT) {
		if (IsPlatformEdge(0.1f, *coObjects)) {
			SetState(KOOPA_STATE_WALKING_LEFT);
		}
	}

	//DebugOut(L"[INFO] Koopa: vx=%f, ax=%f, vy=%f, ay=%f\n", vx, ax, vy, ay);

	CGameObject::Update(dt, coObjects);
	CCollision::GetInstance()->Process(this, dt, coObjects);
}

void CKoopa::StartShell()
{
	shellStart = GetTickCount64();
	isKicked = false;
	SetState(KOOPA_STATE_SHELL_STATIC);
}


void CKoopa::Render()
{
	if (isDefeated == 1)
		return;

	int aniId;
	if (state == KOOPA_STATE_SHELL_STATIC || state == KOOPA_STATE_BEING_HELD)
	{
		if (!isKicked && GetTickCount64() - shellStart > KOOPA_SHELL_ALERT_TIMEOUT)
		{
			if (isReversed)
				aniId = ID_ANI_KOOPA_SHELL_STATIC_REVERSE_2;
			else
				aniId = ID_ANI_KOOPA_SHELL_STATIC_2;
		}
		else
		{
			if (isReversed)
				aniId = ID_ANI_KOOPA_SHELL_STATIC_REVERSE_1;
			else
				aniId = ID_ANI_KOOPA_SHELL_STATIC_1;
		}
	}
	else if (state == KOOPA_STATE_SHELL_DYNAMIC)
	{
		if (isReversed)
			aniId = ID_ANI_KOOPA_SHELL_DYNAMIC_REVERSE;
		else
			aniId = ID_ANI_KOOPA_SHELL_DYNAMIC;
	}
	else if (state == KOOPA_STATE_WALKING_LEFT)
		aniId = ID_ANI_KOOPA_WALKING_LEFT;
	else aniId = ID_ANI_KOOPA_WALKING_RIGHT;

	if (isDead)
	{
		aniId = ID_ANI_KOOPA_SHELL_STATIC_REVERSE_1;
	}

	CAnimations::GetInstance()->Get(aniId)->Render(x, y);
	//RenderBoundingBox();
}
void CKoopa::SetState(int state)
{
	if (isDead == 1) return;

	// If transitioning from shell to walking state
	if ((this->state == KOOPA_STATE_SHELL_STATIC || this->state == KOOPA_STATE_SHELL_DYNAMIC) &&
		(state == KOOPA_STATE_WALKING_LEFT || state == KOOPA_STATE_WALKING_RIGHT))
	{
		// Adjust y position to account for hitbox difference
		// In shell: hitbox is centered (height/2 above and below y)
		// In walking: hitbox extends from bottom up
		float heightDifference = (KOOPA_TEXTURE_HEIGHT - KOOPA_BBOX_HEIGHT) / 2.0f;
		y -= heightDifference; // Move Koopa up to compensate
	}

	switch (state)
	{
	case KOOPA_STATE_DIE_ON_COLLIDE_WITH_ENEMY:
	{
		dieStart = GetTickCount64();
		CMario* player = (CMario*)((LPPLAYSCENE)CGame::GetInstance()->GetCurrentScene())->GetPlayer();
		CParticle::GenerateParticleInChunk(this, 0, 100);
		CGame::GetInstance()->GetGameState()->AddScore(100);
		vx = (player->GetNx() > 0) ? 0.2f : -0.2f;
		vy = -0.35f;
		ax = 0;
		ay = KOOPA_GRAVITY;
		isDead = 1;
		break;
	}
	case KOOPA_STATE_DIE_ON_COLLIDE_WITH_TERRAIN:
		dieStart = GetTickCount64();
		CParticle::GenerateParticleInChunk(this, 0, 100);
		CGame::GetInstance()->GetGameState()->AddScore(100);
		vx = 0;
		vy = -0.4f;
		ax = 0;
		ay = KOOPA_GRAVITY;
		isDead = 1;
		break;
	case KOOPA_STATE_DIE_ON_COLLIDE_WITH_DYNAMIC_KOOPA:
		dieStart = GetTickCount64();
		vy = -0.5f;
		ay = KOOPA_GRAVITY;
		isDead = 1;
		break;
	case KOOPA_STATE_DIE_ON_COLLIDE_WITH_HELD_KOOPA:
		dieStart = GetTickCount64();
		vy = -0.35f;
		ay = KOOPA_GRAVITY;
		isDead = 1;
		break;
	case KOOPA_STATE_SHELL_STATIC:
		vx = 0;
		vy = 0;
		ax = 0;
		break;
	case KOOPA_STATE_SHELL_DYNAMIC:
		isKicked = true;
		//vy = 0;
		//ax = 0;
		break;
	case KOOPA_STATE_WALKING_LEFT:
		if (GetTickCount64() - lastTurnAroundTime < KOOPA_TURNAROUND_TIMEOUT)
			return;
		lastTurnAroundTime = GetTickCount64();
		if (this->state == KOOPA_STATE_SHELL_STATIC && vy != 0)
			return;
		isKicked = false;
		vx = -KOOPA_WALKING_SPEED;
		nx = -1;
		break;
	case KOOPA_STATE_WALKING_RIGHT:
		if (GetTickCount64() - lastTurnAroundTime < KOOPA_TURNAROUND_TIMEOUT)
			return;
		lastTurnAroundTime = GetTickCount64();
		if (this->state == KOOPA_STATE_SHELL_STATIC && vy != 0)
			return;
		isKicked = false;
		vx = KOOPA_WALKING_SPEED;
		nx = 1;
		break;
	case KOOPA_STATE_BEING_HELD:
		isHeld = 1;
		isFlying = false;
		break;
	}
	CGameObject::SetState(state);

	//DebugOut(L"[INFO] Koopa: state=%d\n", state);
}