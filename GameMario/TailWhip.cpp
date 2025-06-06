#include "TailWhip.h"
#include "QuestionBlock.h"
#include "Goomba.h"
#include "Koopa.h"
#include "Mario.h"
#include "WingedGoomba.h"
#include "BuffQBlock.h"
#include "CoinQBlock.h"
#include "PiranhaPlant.h"
#include "LifeBrick.h"
#include "CoinBrick.h"
#include "ActivatorBrick.h"
#include "WingedKoopa.h"
#include "BoomerangTurtle.h"
#include "HiddenCoinBrick.h"

#include "PlayScene.h"

CTailWhip::CTailWhip(int id, float x, float y, int z) : CGameObject(id, x, y, z)
{
	notWhipping = 1;
	whippingLeft = 0;
	whippingRight = 0;
	whipSpin = 0;
	whipLeftStart = -1;
	whipRightStart = -1;
}

CTailWhip::~CTailWhip()
{
}

void CTailWhip::UpdatePosition(float marioX, float marioY, int marioNx)
{
	// Calculate whip position relative to Mario
	// Adjust offsets as needed for visual alignment
	if (whippingRight == 1)
		x = marioX + 6;
	else if (whippingLeft == 1)
		x = marioX - 8;
	else // If not actively whipping left/right, maybe hide it or default position?
		x = (marioNx > 0) ? marioX + 6 : marioX - 8; // Keep relative position even if mid-spin

	y = marioY + 6; // Adjust Y offset as needed
}

void CTailWhip::Render()
{
	//// Don't render if not whipping
	//if (notWhipping == 1) return;

	//if (notWhipping == 1) RenderBoundingBox(); // Debug only
}

void CTailWhip::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	//if (notWhipping == 0) // Only update timers if active 
	//{ 
	//	if (whipSpin >= 2) 
	//	{
	//		whipSpin = 0;
	//		SetState(TAIL_STATE_NOT_WHIPPING);
	//	}
	//	else if (whippingLeft == 1) 
	//	{
	//		if (GetTickCount64() - whipLeftStart > TAIL_WHIP_LEFT_TIME) 
	//		{
	//			whippingLeft = 0;
	//			SetState(TAIL_STATE_WHIPPING_RIGHT); // Transition state
	//		}
	//	}
	//	else if (whippingRight == 1) 
	//	{
	//		if (GetTickCount64() - whipRightStart > TAIL_WHIP_RIGHT_TIME) 
	//		{
	//			whippingRight = 0;
	//			SetState(TAIL_STATE_WHIPPING_LEFT); // Transition state
	//		}
	//	}
	//}

	if (notWhipping == 0)
	{
		if (whippingLeft == 1)
		{
			if (GetTickCount64() - whipLeftStart > TAIL_WHIP_LEFT_TIME)
			{
				whippingLeft = 0;
				SetState(TAIL_STATE_NOT_WHIPPING); // Transition state
			}
		}
		else if (whippingRight == 1)
		{
			if (GetTickCount64() - whipRightStart > TAIL_WHIP_RIGHT_TIME)
			{
				whippingRight = 0;
				SetState(TAIL_STATE_NOT_WHIPPING); // Transition state
			}
		}
	}

	if (IsActive()) 
	{
		CCollision::GetInstance()->Process(this, dt, coObjects);
	}
}

void CTailWhip::OnCollisionWith(LPCOLLISIONEVENT e)
{
	if (e->obj->IsDeleted()) return;
	if (dynamic_cast<CGoomba*>(e->obj))
		OnCollisionWithGoomba(e);
	else if (dynamic_cast<CWingedGoomba*>(e->obj))
		OnCollisionWithWingedGoomba(e);
	else if (dynamic_cast<CKoopa*>(e->obj))
		OnCollisionWithKoopa(e);
	else if (dynamic_cast<CBuffQBlock*>(e->obj))
		OnCollisionWithBuffQBlock(e);
	else if (dynamic_cast<CCoinQBlock*>(e->obj))
		OnCollisionWithCoinQBlock(e);
	else if (dynamic_cast<CLifeBrick*>(e->obj))
		OnCollisionWithLifeBrick(e);
	else if (dynamic_cast<CPiranhaPlant*>(e->obj))
		OnCollisionWithPiranhaPlant(e);
	else if (dynamic_cast<CCoinBrick*>(e->obj))
		OnCollisionWithCoinBrick(e);
	else if (dynamic_cast<CActivatorBrick*>(e->obj))
		OnCollisionWithActivatorBrick(e);
	else if (dynamic_cast<CWingedKoopa*>(e->obj))
		OnCollisionWithWingedKoopa(e);
	else if (dynamic_cast<CBoomerangTurtle*>(e->obj))
		OnCollisionWithBooomerangTurtle(e);
	else if (dynamic_cast<CHiddenCoinBrick*>(e->obj))
		OnCollisionWithHiddenCoinBrick(e);
}

void CTailWhip::OnCollisionWithBuffQBlock(LPCOLLISIONEVENT e)
{
	CBuffQBlock* bqb = dynamic_cast<CBuffQBlock*>(e->obj);
	CMario* player = (CMario*)((LPPLAYSCENE)CGame::GetInstance()->GetCurrentScene())->GetPlayer();
	if (bqb && !bqb->IsHit() && bqb->GetState() == QUESTIONBLOCK_STATE_NOT_HIT)
	{
		if (player->GetLevel() == MARIO_LEVEL_SMALL)
		{
			CMushroom* mr = bqb->GetMushroom();
			if (mr)
			{
				float mX, mY;
				mr->GetPosition(mX, mY);
				if (x < mX)
					mr->SetCollisionNx(1);
				else
					mr->SetCollisionNx(-1);
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

void CTailWhip::OnCollisionWithCoinQBlock(LPCOLLISIONEVENT e) 
{
	CCoinQBlock* cqb = dynamic_cast<CCoinQBlock*>(e->obj);
	if (cqb && !cqb->IsHit() && cqb->GetState() == QUESTIONBLOCK_STATE_NOT_HIT)
	{
		cqb->SetState(QUESTIONBLOCK_STATE_BOUNCE_UP);
	}
}

void CTailWhip::OnCollisionWithLifeBrick(LPCOLLISIONEVENT e)
{
	CLifeBrick* lb = dynamic_cast<CLifeBrick*>(e->obj);
	if (lb && !lb->IsHit() && lb->GetState() == QUESTIONBLOCK_STATE_NOT_HIT) 
	{
		lb->SetState(QUESTIONBLOCK_STATE_BOUNCE_UP);
	}
}

void CTailWhip::OnCollisionWithActivatorBrick(LPCOLLISIONEVENT e)
{
	CActivatorBrick* ab = dynamic_cast<CActivatorBrick*>(e->obj);
	if (ab && !ab->IsHit() && ab->GetState() == QUESTIONBLOCK_STATE_NOT_HIT)
	{
		ab->SetState(QUESTIONBLOCK_STATE_BOUNCE_UP);
	}
}

void CTailWhip::OnCollisionWithCoinBrick(LPCOLLISIONEVENT e)
{
	CCoinBrick* cb = dynamic_cast<CCoinBrick*>(e->obj);
	if (cb)
	{
		if (cb->IsRevealed()) return;

		cb->SetState(COIN_BRICK_STATE_BREAK);
		
	}
}

void CTailWhip::OnCollisionWithGoomba(LPCOLLISIONEVENT e)
{
	CGoomba* g = dynamic_cast<CGoomba*>(e->obj);
	if (g)
	{
		if (g->IsDead() == 1 || g->IsDefeated() == 1) 
		{
			return;
		}
		CMario* player = (CMario*)((LPPLAYSCENE)CGame::GetInstance()->GetCurrentScene())->GetPlayer();
		player->CalculateScore(g);
		CParticle::GenerateParticleInChunk(this, 3);
		g->SetState(GOOMBA_STATE_DIE_ON_TAIL_WHIP);
	}
}

void CTailWhip::OnCollisionWithPiranhaPlant(LPCOLLISIONEVENT e)
{
	CPiranhaPlant* pp = dynamic_cast<CPiranhaPlant*>(e->obj);
	if (pp)
	{
		if (pp->GetState() == PIRANHA_PLANT_STATE_HIDDEN 
			|| pp->GetState() == PIRANHA_PLANT_STATE_DIE
			|| pp->IsDefeated() == 1) 
		{
			return;
		}
		CMario* player = (CMario*)((LPPLAYSCENE)CGame::GetInstance()->GetCurrentScene())->GetPlayer();
		player->CalculateScore(pp);
		CParticle::GenerateParticleInChunk(this, 3);
		pp->SetState(PIRANHA_PLANT_STATE_DIE);
	}
}

void CTailWhip::OnCollisionWithKoopa(LPCOLLISIONEVENT e)
{
	CKoopa* k = dynamic_cast<CKoopa*>(e->obj);
	if (k)
	{
		if (k->IsDead() || k->IsDefeated())
		{
			return;
		}

		if (k->GetState() == KOOPA_STATE_WALKING_LEFT || k->GetState() == KOOPA_STATE_WALKING_RIGHT)
		{
			CMario* player = (CMario*)((LPPLAYSCENE)CGame::GetInstance()->GetCurrentScene())->GetPlayer();
			player->CalculateScore(k);
		}
		CParticle::GenerateParticleInChunk(this, 3);
		k->StartShell();
		k->SetState(KOOPA_STATE_SHELL_STATIC);
		float knockbackVx = (this->nx > 0) ? 0.075f : -0.075f;
		float knockbackVy = -0.5f;
		k->SetIsFlying(true);
		k->SetIsReversed(true);
		k->SetSpeed(knockbackVx, knockbackVy);
	}
}


void CTailWhip::OnCollisionWithWingedKoopa(LPCOLLISIONEVENT e)
{
	CWingedKoopa* wk = dynamic_cast<CWingedKoopa*>(e->obj);
	if (wk)
	{
		if (wk->IsDead() || wk->IsDefeated())
		{
			return;
		}

		if (wk->GetState() == KOOPA_STATE_WALKING_LEFT || wk->GetState() == KOOPA_STATE_WALKING_RIGHT)
		{
			CMario* player = (CMario*)((LPPLAYSCENE)CGame::GetInstance()->GetCurrentScene())->GetPlayer();
			player->CalculateScore(wk);
		}
		CParticle::GenerateParticleInChunk(this, 3);
		wk->SetIsWinged(0);
		wk->StartShell();
		wk->SetState(KOOPA_STATE_SHELL_STATIC);
		float knockbackVx = (this->nx > 0) ? 0.075f : -0.075f;
		float knockbackVy = -0.35f;
		wk->SetIsFlying(true);
		wk->SetIsReversed(true);
		wk->SetSpeed(knockbackVx, knockbackVy);
	}
}

void CTailWhip::OnCollisionWithBooomerangTurtle(LPCOLLISIONEVENT e)
{
	CBoomerangTurtle* bt = dynamic_cast<CBoomerangTurtle*>(e->obj);
	if (bt)
	{
		if (bt->IsDead() == 1 || bt->IsDefeated() == 1) return;

		bt->SetState((nx > 0) ? BOOMERANG_TURTLE_STATE_DIE_BY_TAIL_RIGHT : BOOMERANG_TURTLE_STATE_DIE_BY_TAIL_LEFT);
		vy = -MARIO_JUMP_DEFLECT_SPEED;

		CMario* player = (CMario*)((LPPLAYSCENE)CGame::GetInstance()->GetCurrentScene())->GetPlayer();
		player->CalculateScore(bt, true);
		CParticle::GenerateParticleInChunk(bt, 3);
	}
}

void CTailWhip::OnCollisionWithHiddenCoinBrick(LPCOLLISIONEVENT e)
{
	CHiddenCoinBrick* hcb = dynamic_cast<CHiddenCoinBrick*>(e->obj);
	if (hcb && !hcb->IsHit() && hcb->GetState() == QUESTIONBLOCK_STATE_NOT_HIT)
	{
		hcb->SetState(QUESTIONBLOCK_STATE_BOUNCE_UP);
	}
}

void CTailWhip::OnCollisionWithWingedGoomba(LPCOLLISIONEVENT e)
{
	CWingedGoomba* wg = dynamic_cast<CWingedGoomba*>(e->obj);
	if (wg)
	{
		if (wg->IsDead() == 1 || wg->IsDefeated() == 1) 
		{
			return;
		}
		CMario* player = (CMario*)((LPPLAYSCENE)CGame::GetInstance()->GetCurrentScene())->GetPlayer();
		player->CalculateScore(wg);
		CParticle::GenerateParticleInChunk(this, 3);
		wg->SetState(WINGED_GOOMBA_STATE_DIE_ON_TAIL_WHIP);
	}
}

void CTailWhip::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x - TAIL_WHIP_BBOX_WIDTH / 2;
	t = y - TAIL_WHIP_BBOX_HEIGHT / 2;
	r = l + TAIL_WHIP_BBOX_WIDTH;
	b = t + TAIL_WHIP_BBOX_HEIGHT;
}

void CTailWhip::SetState(int state)
{
	// Reset timers/flags when changing state
	whippingLeft = 0;
	whippingRight = 0;
	notWhipping = 1; // Default to not whipping

	switch (state)
	{
	case TAIL_STATE_WHIPPING_LEFT:
		whipSpin++;
		whippingLeft = 1;
		notWhipping = 0; // Now active
		whipLeftStart = GetTickCount64();
		this->nx = -1; // Set facing direction for collision knockback
		break;
	case TAIL_STATE_WHIPPING_RIGHT:
		whipSpin++;
		whippingRight = 1;
		notWhipping = 0; // Now active
		whipRightStart = GetTickCount64();
		this->nx = 1; // Set facing direction
		break;
	case TAIL_STATE_NOT_WHIPPING:
		// Flags already reset above
		whipSpin = 0; // Reset spin count
		break;
	}
	CGameObject::SetState(state); // Call base class
}