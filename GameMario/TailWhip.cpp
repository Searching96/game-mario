#include "TailWhip.h"
#include "QuestionBlock.h"
#include "Goomba.h"
#include "Koopa.h"
#include "Mario.h"
#include "WingedGoomba.h"
#include "BuffQBlock.h"

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
	
	if (whipSpin == 2)
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
	if (dynamic_cast<CGoomba*>(e->obj))
		OnCollisionWithGoomba(e);
	else if (dynamic_cast<CWingedGoomba*>(e->obj))
		OnCollisionWithWingedGoomba(e);
	else if (dynamic_cast<CKoopa*>(e->obj))
		OnCollisionWithKoopa(e);
	else if (dynamic_cast<CBuffQBlock*>(e->obj))
		OnCollisionWithBuffQBlock(e);
	else if (dynamic_cast<CQuestionBlock*>(e->obj))
		OnCollisionWithQuestionBlock(e);
}

void CTailWhip::OnCollisionWithQuestionBlock(LPCOLLISIONEVENT e)
{
	CQuestionBlock* qb = dynamic_cast<CQuestionBlock*>(e->obj);
	if (e->nx != 0 && qb->GetState() == QUESTIONBLOCK_STATE_NOT_HIT)
		qb->SetState(QUESTIONBLOCK_STATE_BOUNCE_UP);
}

void CTailWhip::OnCollisionWithBuffQBlock(LPCOLLISIONEVENT e)
{
	CBuffQBlock* bqb = dynamic_cast<CBuffQBlock*>(e->obj);
	if (e->nx != 0 && bqb->GetState() == QUESTIONBLOCK_STATE_NOT_HIT)
		bqb->SetState(QUESTIONBLOCK_STATE_BOUNCE_UP);
}

void CTailWhip::OnCollisionWithGoomba(LPCOLLISIONEVENT e)
{
	CGoomba* goomba = dynamic_cast<CGoomba*>(e->obj);
	if (goomba)
	{
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
		koopa->StartShell();
		koopa->SetReversed(true);
	}
}

void CTailWhip::OnCollisionWithWingedGoomba(LPCOLLISIONEVENT e)
{
	CWingedGoomba* wingedGoomba = dynamic_cast<CWingedGoomba*>(e->obj);
	if (wingedGoomba)
	{
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
