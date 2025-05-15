#include "Koopa.h"
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

#include "PlayScene.h"

#include "debug.h"

CKoopa::CKoopa(int id, float x, float y, int z, int originalChunkId) : CGameObject(id, x, y, z)
{
	this->originalChunkId = originalChunkId;
	this->ax = 0;
	this->ay = KOOPA_GRAVITY;
	x0 = x;
	y0 = y;
	shellStart = -1;
	SetState(KOOPA_STATE_WALKING_LEFT);
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
		else
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
		//else if (state == KOOPA_STATE_WALKING_RIGHT)
		//{
		//	this->SetState(KOOPA_STATE_WALKING_LEFT);
		//	return;
		//}
	}

	bool isAbleToBounce = (state == KOOPA_STATE_WALKING_LEFT || state == KOOPA_STATE_WALKING_RIGHT || state == KOOPA_STATE_SHELL_STATIC);
	if (isAbleToBounce)
	{
		bool isHit = dynamic_cast<CCoinQBlock*>(e->obj)
			? dynamic_cast<CCoinQBlock*>(e->obj)->GetIsHit()
			: dynamic_cast<CBuffQBlock*>(e->obj)->GetIsHit();
		// get isbouncing the same way as get ishit
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
		bool isHit = lb->GetIsHit();
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

void CKoopa::OnCollisionWithPiranhaPlant(LPCOLLISIONEVENT e)
{
	CPiranhaPlant* pp = dynamic_cast<CPiranhaPlant*>(e->obj);
	if (this->state == KOOPA_STATE_SHELL_DYNAMIC)
	{
		if (pp->GetState() != PIRANHA_PLANT_STATE_DIED)
		{
			pp->SetState(PIRANHA_PLANT_STATE_DIED);
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

void CKoopa::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	DebugOutTitle(L"isdf: %d\n", (int)isDefeated);

	if (isDefeated == 1)
		return;

	if (isHeld == 1)
	{
		vx = 0;
		vy = 0;

		vector<CChunk*> chunkList = dynamic_cast<CPlayScene*>(CGame::GetInstance()->GetCurrentScene())->GetChunks();


		for (auto chunk : chunkList)
		{
			if (!chunk->IsLoaded()) continue;
			for (auto obj : chunk->GetObjects())
			{
				if (obj == this) chunk->RemoveObject(this);
			}
			if (x < chunk->GetEndX() && x > chunk->GetStartX())
			{
				chunk->AddObject(this);
				chunk->AddEnemy(this);
			}
		}
	}

	if (state == KOOPA_STATE_SHELL_DYNAMIC)
	{
		vx = (nx > 0) ? KOOPA_SHELL_SPEED : -KOOPA_SHELL_SPEED;
	}

	if ((isDead == 1) && (GetTickCount64() - dieStart > KOOPA_DIE_TIMEOUT))
	{
		LPCHUNK chunk = ((LPPLAYSCENE)CGame::GetInstance()->GetCurrentScene())->GetChunk(originalChunkId);
		chunk->SetIsObjectDeleted(this->GetId(), true);
		isDeleted = true;
		return;
	}

	CMario* player = (CMario*)((LPPLAYSCENE)CGame::GetInstance()->GetCurrentScene())->GetPlayer();

	if (isFlying && vy == 0)
	{
		vx = 0;
		isFlying = false;
		return;
	}

	// Get out of shell
	if ((state == KOOPA_STATE_SHELL_STATIC || state == KOOPA_STATE_BEING_HELD)
		&& !isKicked && (GetTickCount64() - shellStart > KOOPA_SHELL_TIMEOUT))
	{
		if (player->GetIsRunning() == 1 && isHeld == 1)
		{
			isHeld = 0;
			player->SetIsHoldingKoopa(0);

			int isKilledOnCollideWithTerrain = 0;

			// Check for collision with blocking objects
			vector<LPCOLLISIONEVENT> coEvents;
			vector<LPCOLLISIONEVENT> coEventsResult;

			coEvents.clear();
			CCollision::GetInstance()->Scan(this, dt, coObjects, coEvents);

			for (int i = 0; i < coEvents.size(); i++)
			{
				LPCOLLISIONEVENT e = coEvents[i];
				if (e->obj->IsBlocking())
				{
					this->SetState(KOOPA_STATE_DIE_ON_COLLIDE_WITH_TERRAIN);
					isHeld = 0;
					player->SetIsHoldingKoopa(0);
					player->StartKick();
					isKilledOnCollideWithTerrain = 1;
					break;
				}
			}

			for (int i = 0; i < coEvents.size(); i++) delete coEvents[i];
			if (isKilledOnCollideWithTerrain == 1)
				return;

			y -= 6; // RED ALLERT
			this->SetState((player->GetNx() > 0) ? KOOPA_STATE_WALKING_RIGHT : KOOPA_STATE_WALKING_LEFT);
			if (player->GetLevel() == MARIO_LEVEL_TAIL)
				player->SetState(MARIO_STATE_TAIL_DOWN);
			else if (player->GetLevel() == MARIO_LEVEL_BIG)
				player->SetState(MARIO_STATE_POWER_DOWN);
			else if (player->GetLevel() == MARIO_LEVEL_SMALL)
				player->SetState(MARIO_STATE_DIE);

		}
		this->SetPosition(x, y - 2);
		SetState(KOOPA_STATE_WALKING_LEFT);
	}

	if (isHeld == 1)
	{
		// Check overlap with enemies
		vector<LPCOLLISIONEVENT> coEvents;
		coEvents.clear();
		CCollision::GetInstance()->Scan(this, dt, coObjects, coEvents);

		int isKilledOnCollideWithEnemy = 0;
		for (size_t i = 0; i < coEvents.size(); i++)
		{
			LPCOLLISIONEVENT e = coEvents[i];
			if (CGoomba* g = dynamic_cast<CGoomba*>(e->obj))
			{
				if (g->IsDefeated() == 1 || g->IsDead() == 1) continue;
				this->SetState(KOOPA_STATE_DIE_ON_COLLIDE_WITH_ENEMY);
				e->obj->SetState(WINGED_GOOMBA_STATE_DIE_ON_HELD_KOOPA);
				isKilledOnCollideWithEnemy = 1;
			}
			else if (CWingedGoomba* wg = dynamic_cast<CWingedGoomba*>(e->obj))
			{
				if (wg->IsDefeated() == 1 || wg->IsDead() == 1) continue;
				this->SetState(KOOPA_STATE_DIE_ON_COLLIDE_WITH_ENEMY);
				e->obj->SetState(GOOMBA_STATE_DIE_ON_HELD_KOOPA);
				isKilledOnCollideWithEnemy = 1;
			}
			else if (CPiranhaPlant* pp = dynamic_cast<CPiranhaPlant*>(e->obj))
			{
				if (pp->IsDefeated() == 1 || pp->GetState() == PIRANHA_PLANT_STATE_DIED) continue;
				this->SetState(KOOPA_STATE_DIE_ON_COLLIDE_WITH_ENEMY);
				e->obj->SetState(PIRANHA_PLANT_STATE_DIED);
				isKilledOnCollideWithEnemy = 1;
			}
		}

		for (size_t i = 0; i < coEvents.size(); i++) delete coEvents[i];
		if (isKilledOnCollideWithEnemy == 1)
		{
			player->SetIsHoldingKoopa(0);
			isHeld = 0;
			vy += ay * dt;
			y += vy * dt;
			return;
		}

		if (player->GetIsRunning() == 0)
		{
			this->SetState(KOOPA_STATE_SHELL_DYNAMIC);
			this->SetNx(player->GetNx());
			this->SetSpeed((nx > 0) ? KOOPA_SHELL_SPEED : -KOOPA_SHELL_SPEED, 0);
			isHeld = 0;
			player->SetIsHoldingKoopa(0);
			player->StartKick();

			// Check collision with blocking objects (terrain)
			vector<LPCOLLISIONEVENT> coEvents;
			vector<LPCOLLISIONEVENT> coEventsResult;

			coEvents.clear();
			CCollision::GetInstance()->Scan(this, dt, coObjects, coEvents);

			for (size_t i = 0; i < coEvents.size(); i++)
			{
				LPCOLLISIONEVENT e = coEvents[i];
				if (e->obj->IsBlocking())
				{
					SetState(KOOPA_STATE_DIE_ON_COLLIDE_WITH_TERRAIN);
					isHeld = 0;
					player->SetIsHoldingKoopa(0);
					player->StartKick();
					break;
				}
			}

			for (size_t i = 0; i < coEvents.size(); i++) delete coEvents[i];
			return;
		}

		float mX, mY;
		player->GetPosition(mX, mY);
		float mNx = player->GetNx();
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

	if (state == KOOPA_STATE_WALKING_LEFT) {
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
	//float l, t, r, b;
	//GetBoundingBox(l, t, r, b);
	//DebugOut(L"[INFO] Bounding box: left=%f, top=%f, right=%f, bottom=%f\n", l, t, r, b);
	//DebugOut(L"[INFO] Koopa: x=%f, y=%f\n", x, y);

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

	if (state == KOOPA_STATE_DIE_ON_COLLIDE_WITH_ENEMY || state == KOOPA_STATE_DIE_ON_COLLIDE_WITH_TERRAIN)
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
		float heightDifference = (KOOPA_TEXTURE_HEIGHT - KOOPA_BBOX_HEIGHT) / 2;
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
		if (this->state == KOOPA_STATE_SHELL_STATIC && vy != 0)
			return;
		isKicked = false;
		vx = -KOOPA_WALKING_SPEED;
		break;
	case KOOPA_STATE_WALKING_RIGHT:
		if (this->state == KOOPA_STATE_SHELL_STATIC && vy != 0)
			return;
		isKicked = false;
		vx = KOOPA_WALKING_SPEED;
		break;
	case KOOPA_STATE_BEING_HELD:
		isHeld = 1;
		isFlying = false;
		break;
	}
	CGameObject::SetState(state);

	//DebugOut(L"[INFO] Koopa: state=%d\n", state);
}