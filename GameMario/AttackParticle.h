#pragma once

#include "GameObject.h"
#include "Animation.h"
#include "Animations.h"

#define ID_ANI_ATTACK_PARTICLE				30000

#define ATTACK_PARTICLE_BBOX_WIDTH			0
#define ATTACK_PARTICLE_BBOX_HEIGHT			0

#define ATTACK_PARTICLE_STATE_EMERGING		0
#define ATTACK_PARTICLE_STATE_NOT_EMERGING	100

#define ATTACK_PARTICLE_EMERGE_TIME			200

class CAttackParticle : public CGameObject
{
protected:
	int isEmerging = 0;
	ULONGLONG emergingStart = -1;
public:
	CAttackParticle(float x, float y);
	void Render();
	void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	void GetBoundingBox(float& l, float& t, float& r, float& b);
	int IsCollidable() { return 0; };
	int IsBlocking() { return 0; }
	void SetState(int state);
	void StartEmerging() { isEmerging = 1; emergingStart = GetTickCount64(); }
};