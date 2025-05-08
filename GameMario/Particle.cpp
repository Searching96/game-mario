#include "Particle.h"

CParticle::CParticle(float x, float y, int z, int point) : CGameObject(x, y, z) {
	isEmerging = 0;
	emergingStart = -1;
	point = point;
}

void CParticle::Render()
{
	if (isEmerging == 0) return;

	//CAnimations* animations = CAnimations::GetInstance();
	//LPANIMATION ani = animations->Get(ID_ANI_ATTACK_PARTICLE);
	//if (ani)
	//	ani->Render(x, y);

	//RenderBoundingBox(); // Debug only
}

void CParticle::Update(DWORD dt, vector<LPGAMEOBJECT>* /*coObjects*/) // Ignore coObjects
{
	//if (isEmerging == 1) {
	//	if (GetTickCount64() - emergingStart > ATTACK_PARTICLE_EMERGE_TIME) {
	//		SetState(ATTACK_PARTICLE_STATE_NOT_EMERGING);
	//	}
	//}
}

void CParticle::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	//l = x - ATTACK_PARTICLE_BBOX_WIDTH / 2;
	//t = y - ATTACK_PARTICLE_BBOX_HEIGHT / 2;
	//r = l + ATTACK_PARTICLE_BBOX_WIDTH;
	//b = t + ATTACK_PARTICLE_BBOX_HEIGHT;
}

void CParticle::SetState(int state)
{
	CGameObject::SetState(state);
	//switch (state)
	//{
	//case ATTACK_PARTICLE_STATE_NOT_EMERGING:
	//	isEmerging = 0;
	//	emergingStart = -1;
	//	break;
	//case ATTACK_PARTICLE_STATE_EMERGING:
	//	if (isEmerging == 0) {
	//		isEmerging = 1;
	//		emergingStart = GetTickCount64();
	//	}
	//	break;
	//}
}
