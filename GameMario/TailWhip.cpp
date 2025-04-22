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

CTailWhip::CTailWhip(float x, float y, CAttackParticle* attackParticle) : CGameObject(x, y) 
{
	this->attackParticle = attackParticle;
}

void CTailWhip::Render()
{
	CMario* mario = dynamic_cast<CMario*>(dynamic_cast<CPlayScene*>(CGame::GetInstance()->GetCurrentScene())->GetPlayer());
	if (mario->GetLevel() != MARIO_LEVEL_TAIL)
		return;
	if (notWhipping == 1)
		return;
	CAnimations* animations = CAnimations::GetInstance();
	animations->Get(ID_ANI_TAIL_WHIP)->Render(x, y);

	//RenderBoundingBox();
}

void CTailWhip::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	CMario* mario = dynamic_cast<CMario*>(dynamic_cast<CPlayScene*>(CGame::GetInstance()->GetCurrentScene())->GetPlayer());
	if (mario->GetLevel() != MARIO_LEVEL_TAIL)
		return;

	if (notWhipping == 1)
		return;
	
	if (whipSpin >= 2)
	{
		whipSpin = 0;
		SetState(TAIL_STATE_NOT_WHIPPING);
	}
	else if (whippingLeft == 1)
	{
		if (GetTickCount64() - whipLeftStart > TAIL_WHIP_LEFT_TIME)
		{
			whippingLeft = 0;
			SetState(TAIL_STATE_WHIPPING_RIGHT);
		}
	}
	else if (whippingRight == 1)
	{
		if (GetTickCount64() - whipRightStart > TAIL_WHIP_RIGHT_TIME)
		{
			whippingRight = 0;
			SetState(TAIL_STATE_WHIPPING_LEFT);
		}
	}

	float mX, mY, mNx;
	mario->GetPosition(mX, mY);
	mario->GetNx(mNx);

	if (whippingRight == 1)
		x = mX + 6;
	else if (whippingLeft == 1)
		x = mX - 6;
	y = mY + 6;

	CCollision::GetInstance()->Process(this, dt, coObjects);
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

void CTailWhip::OnCollisionWithCoinQBlock(LPCOLLISIONEVENT e)
{
	CCoinQBlock* cqb = dynamic_cast<CCoinQBlock*>(e->obj);
	if (cqb->GetState() == QUESTIONBLOCK_STATE_NOT_HIT)
		cqb->SetState(QUESTIONBLOCK_STATE_BOUNCE_UP);
}

void CTailWhip::OnCollisionWithGoomba(LPCOLLISIONEVENT e)
{
	CGoomba* goomba = dynamic_cast<CGoomba*>(e->obj);
	if (goomba)
	{
		if (goomba->GetState() == GOOMBA_STATE_DIE_ON_STOMP)
			return;
		if (goomba->GetState() == GOOMBA_STATE_DIE_ON_TAIL_WHIP)
			return;
		CMario* player = (CMario*)((LPPLAYSCENE)CGame::GetInstance()->GetCurrentScene())->GetPlayer();
		float mNx;
		player->GetNx(mNx);
		if (mNx > 0)
			attackParticle->SetPosition(x + 8, y);
		else
			attackParticle->SetPosition(x - 8, y);
		attackParticle->SetState(ATTACK_PARTICLE_STATE_EMERGING);
		goomba->SetState(GOOMBA_STATE_DIE_ON_TAIL_WHIP);
	}
}

void CTailWhip::OnCollisionWithPiranhaPlant(LPCOLLISIONEVENT e)
{
	CPiranhaPlant* plant = dynamic_cast<CPiranhaPlant*>(e->obj);
	if (plant)
	{
		if (plant->GetState() == PIRANHA_PLANT_STATE_HIDDEN) return;
		CMario* player = (CMario*)((LPPLAYSCENE)CGame::GetInstance()->GetCurrentScene())->GetPlayer();
		float mNx;
		player->GetNx(mNx);
		if (mNx > 0)
			attackParticle->SetPosition(x + 8, y);
		else
			attackParticle->SetPosition(x - 8, y);
		attackParticle->SetState(ATTACK_PARTICLE_STATE_EMERGING);
		plant->StartDeath();
	}
}

void CTailWhip::OnCollisionWithKoopa(LPCOLLISIONEVENT e)
{
	CKoopa* koopa = dynamic_cast<CKoopa*>(e->obj);
	if (koopa)
	{
		CMario* player = (CMario*)((LPPLAYSCENE)CGame::GetInstance()->GetCurrentScene())->GetPlayer();
		float mNx;
		player->GetNx(mNx);
		if (mNx > 0)
			attackParticle->SetPosition(x + 8, y);
		else
			attackParticle->SetPosition(x - 8, y);
		attackParticle->SetState(ATTACK_PARTICLE_STATE_EMERGING);
		int state = koopa->GetState();
		if (state == KOOPA_STATE_WALKING_LEFT || state == KOOPA_STATE_WALKING_RIGHT)
			koopa->StartShell();
		koopa->SetState(KOOPA_STATE_SHELL_STATIC);
		float vx = mNx > 0 ? 0.1f : -0.1f;
		koopa->SetFlying(true);
		koopa->SetSpeed(vx, -0.5f);
		koopa->SetReversed(true);
	}
}

void CTailWhip::OnCollisionWithWingedGoomba(LPCOLLISIONEVENT e)
{
	CWingedGoomba* wingedGoomba = dynamic_cast<CWingedGoomba*>(e->obj);
	if (wingedGoomba)
	{
		if (wingedGoomba->GetState() == WINGED_GOOMBA_STATE_DIE_ON_STOMP)
			return;
		if (wingedGoomba->GetState() == WINGED_GOOMBA_STATE_DIE_ON_TAIL_WHIP)
			return;
		CMario* player = (CMario*)((LPPLAYSCENE)CGame::GetInstance()->GetCurrentScene())->GetPlayer();
		float mNx;
		player->GetNx(mNx);
		if (mNx > 0)
			attackParticle->SetPosition(x + 8, y);
		else
			attackParticle->SetPosition(x - 8, y);
		attackParticle->SetState(ATTACK_PARTICLE_STATE_EMERGING);
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
	switch (state)
	{
	case TAIL_STATE_WHIPPING_LEFT:
		whipSpin++;
		whippingLeft = 1;
		notWhipping = 0;
		whipLeftStart = GetTickCount64();
		break;
	case TAIL_STATE_WHIPPING_RIGHT:
		whipSpin++;
		whippingRight = 1;
		notWhipping = 0;
		whipRightStart = GetTickCount64();
		break;
	case TAIL_STATE_NOT_WHIPPING:
		whippingLeft = 0;
		whippingRight = 0;
		notWhipping = 1;
		break;
	}
	CGameObject::SetState(state);
}
