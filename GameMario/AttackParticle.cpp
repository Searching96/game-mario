#include "AttackParticle.h"
#include "Mario.h"

#include "PlayScene.h"

CAttackParticle::CAttackParticle(float x, float y) : CGameObject(x, y) {}

void CAttackParticle::Render()
{
	if (isEmerging == 0)
		return;
	CAnimations* animations = CAnimations::GetInstance();
	animations->Get(ID_ANI_ATTACK_PARTICLE)->Render(x, y);

	RenderBoundingBox();
}

void CAttackParticle::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	if (isEmerging == 1)
	{
		if (GetTickCount64() - emergingStart > ATTACK_PARTICLE_EMERGE_TIME)
		{
			isEmerging = 0;
			SetState(ATTACK_PARTICLE_STATE_NOT_EMERGING);
		}
	}
}

void CAttackParticle::GetBoundingBox(float& l, float& t, float& r, float& b)
{
   l = x - ATTACK_PARTICLE_BBOX_WIDTH / 2;  
   t = y - ATTACK_PARTICLE_BBOX_HEIGHT / 2;  
   r = l + ATTACK_PARTICLE_BBOX_WIDTH;  
   b = t + ATTACK_PARTICLE_BBOX_HEIGHT;  
}

void CAttackParticle::SetState(int state)
{
	switch (state)
	{
	case ATTACK_PARTICLE_STATE_NOT_EMERGING:
		isEmerging = 0;
		break;
	case ATTACK_PARTICLE_STATE_EMERGING:
		isEmerging = 1;
		emergingStart = GetTickCount64();
		break;
	}
	CGameObject::SetState(state);
}
