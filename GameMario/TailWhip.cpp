#include "TailWhip.h"
#include "QuestionBlock.h"
#include "Goomba.h"
#include "Koopa.h"
#include "Mario.h"
#include "WingedGoomba.h"
#include "BuffQBlock.h"
#include "CoinQBlock.h"
#include "PiranhaPlant.h"

#include "PlayScene.h"

CTailWhip::CTailWhip(float x, float y) : CGameObject(x, y)
{
	this->attackParticle = new CAttackParticle(x, y);
	notWhipping = 1;
	whippingLeft = 0;
	whippingRight = 0;
	whipSpin = 0;
	whipLeftStart = -1;
	whipRightStart = -1;
}

CTailWhip::~CTailWhip()
{
	delete attackParticle;
	attackParticle = nullptr;
}

void CTailWhip::UpdatePosition(float marioX, float marioY, int marioNx)
{
	// Calculate whip position relative to Mario
	// Adjust offsets as needed for visual alignment
	if (whippingRight == 1)
		x = marioX + 6;
	else if (whippingLeft == 1)
		x = marioX - 6;
	else // If not actively whipping left/right, maybe hide it or default position?
		x = (marioNx > 0) ? marioX + 6 : marioX - 6; // Keep relative position even if mid-spin

	y = marioY + 6; // Adjust Y offset as needed

	// Also update particle position relative to whip/mario
	if (attackParticle) {
		// Example: particle appears where whip impacts
		float particleX = (marioNx > 0) ? x + TAIL_WHIP_BBOX_WIDTH / 2 + 4 : x - TAIL_WHIP_BBOX_WIDTH / 2 - 4; // Offset from whip edge
		float particleY = y;
		attackParticle->SetPosition(particleX, particleY);
	}
}

void CTailWhip::Render()
{
	// Don't render if not whipping
	if (notWhipping == 1) return;

	CAnimations* animations = CAnimations::GetInstance();
	animations->Get(ID_ANI_TAIL_WHIP)->Render(x, y);

	if (attackParticle != nullptr) {
		attackParticle->Render();
	}

	 RenderBoundingBox(); // Debug only
}

void CTailWhip::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	if (notWhipping == 0) { // Only update timers if active
		if (whipSpin >= 2) {
			whipSpin = 0;
			SetState(TAIL_STATE_NOT_WHIPPING);
		}
		else if (whippingLeft == 1) {
			if (GetTickCount64() - whipLeftStart > TAIL_WHIP_LEFT_TIME) {
				whippingLeft = 0;
				SetState(TAIL_STATE_WHIPPING_RIGHT); // Transition state
			}
		}
		else if (whippingRight == 1) {
			if (GetTickCount64() - whipRightStart > TAIL_WHIP_RIGHT_TIME) {
				whippingRight = 0;
				SetState(TAIL_STATE_WHIPPING_LEFT); // Transition state
			}
		}
	}

	if (attackParticle != nullptr) {
		attackParticle->Update(dt, nullptr);
	}


	if (IsActive()) {
		CCollision::GetInstance()->Process(this, dt, coObjects);
	}
	else {
	}

}

void CTailWhip::OnCollisionWith(LPCOLLISIONEVENT e)
{
	if (dynamic_cast<CAttackParticle*>(e->obj))
		return;
	if (dynamic_cast<CTailWhip*>(e->obj))
		return;
	if (dynamic_cast<CMario*>(e->obj))
		return;
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
	else if (dynamic_cast<CPiranhaPlant*>(e->obj))
		OnCollisionWithPiranhaPlant(e);
	else if (dynamic_cast<CQuestionBlock*>(e->obj))
		OnCollisionWithQuestionBlock(e);
}

void CTailWhip::OnCollisionWithQuestionBlock(LPCOLLISIONEVENT e)
{
	CQuestionBlock* qb = dynamic_cast<CQuestionBlock*>(e->obj);
	if (qb->GetState() == QUESTIONBLOCK_STATE_NOT_HIT)
		qb->SetState(QUESTIONBLOCK_STATE_BOUNCE_UP);
}

void CTailWhip::OnCollisionWithBuffQBlock(LPCOLLISIONEVENT e)
{
	CBuffQBlock* bqb = dynamic_cast<CBuffQBlock*>(e->obj);
	CMario* player = (CMario*)((LPPLAYSCENE)CGame::GetInstance()->GetCurrentScene())->GetPlayer();
	if (bqb)
	{
		if (bqb->GetState() == QUESTIONBLOCK_STATE_NOT_HIT)
		{
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
				bqb->SetToSpawn(1);
			bqb->SetState(QUESTIONBLOCK_STATE_BOUNCE_UP);
		}
	}
}

void CTailWhip::OnCollisionWithCoinQBlock(LPCOLLISIONEVENT e) {
	CCoinQBlock* cqb = dynamic_cast<CCoinQBlock*>(e->obj);
	if (cqb && cqb->GetState() == QUESTIONBLOCK_STATE_NOT_HIT) {
		cqb->SetState(QUESTIONBLOCK_STATE_BOUNCE_UP);
	}
}

void CTailWhip::OnCollisionWithGoomba(LPCOLLISIONEVENT e)
{
	CGoomba* goomba = dynamic_cast<CGoomba*>(e->obj);
	if (goomba && goomba->GetState() != GOOMBA_STATE_DIE_ON_STOMP && goomba->GetState() != GOOMBA_STATE_DIE_ON_TAIL_WHIP)
	{
		if (attackParticle) attackParticle->SetState(ATTACK_PARTICLE_STATE_EMERGING);
		goomba->SetState(GOOMBA_STATE_DIE_ON_TAIL_WHIP);
	}
}

void CTailWhip::OnCollisionWithPiranhaPlant(LPCOLLISIONEVENT e)
{
	CPiranhaPlant* plant = dynamic_cast<CPiranhaPlant*>(e->obj);
	if (plant && plant->GetState() != PIRANHA_PLANT_STATE_HIDDEN)
	{
		if (attackParticle) attackParticle->SetState(ATTACK_PARTICLE_STATE_EMERGING);
		plant->Delete();
	}
}

void CTailWhip::OnCollisionWithKoopa(LPCOLLISIONEVENT e)
{
	CKoopa* koopa = dynamic_cast<CKoopa*>(e->obj);
	if (koopa && koopa->GetState() != KOOPA_STATE_BEING_HELD)
	{
		if (attackParticle) attackParticle->SetState(ATTACK_PARTICLE_STATE_EMERGING);
		koopa->StartShell();
		koopa->SetState(KOOPA_STATE_SHELL_STATIC);
		float knockback_vx = (this->nx > 0) ? 0.1f : -0.1f;
		float knockback_vy = -0.5f;
		koopa->SetFlying(true);
		koopa->SetReversed(true);
		koopa->SetSpeed(knockback_vx, knockback_vy);
	}
}

void CTailWhip::OnCollisionWithWingedGoomba(LPCOLLISIONEVENT e)
{
	CWingedGoomba* wingedGoomba = dynamic_cast<CWingedGoomba*>(e->obj);
	if (wingedGoomba && wingedGoomba->GetState() != WINGED_GOOMBA_STATE_DIE_ON_STOMP && wingedGoomba->GetState() != WINGED_GOOMBA_STATE_DIE_ON_TAIL_WHIP)
	{
		if (attackParticle) attackParticle->SetState(ATTACK_PARTICLE_STATE_EMERGING);
		wingedGoomba->SetState(WINGED_GOOMBA_STATE_DIE_ON_TAIL_WHIP);
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