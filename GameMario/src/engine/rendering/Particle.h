#pragma once
#include "core/GameObject.h"

#define ID_ANI_POINT_100 1130000
#define ID_ANI_POINT_200 1130001
#define ID_ANI_POINT_400 1130002
#define ID_ANI_POINT_800 1130003
#define ID_ANI_POINT_1000 1130010
#define ID_ANI_POINT_2000 1130011
#define ID_ANI_POINT_4000 1130012
#define ID_ANI_POINT_8000 1130013

#define ID_ANI_1_UP 1130020

#define ID_ANI_TANOOKI_TRANSITION 1130100
#define ID_ANI_DEATH_SMOKE 1130200
#define ID_ANI_HIT_PARTICLE 1130300

#define ID_ANI_BRICK_PARTICLE 1130400

#define PARTICLE_BBOX_WIDTH			0
#define PARTICLE_BBOX_HEIGHT			0
#define PARTICLE_FLOATING_VELOCITY -0.05f

#define PARTICLE_STATE_EMERGING		0
#define PARTICLE_STATE_EMERGING_COMPLETE	100

#define PARTICLE_EMERGE_TIME			600
#define DEATH_SMOKE_EMERGE_TIME			400
#define TANOOKI_TRANSITION_EMERGE_TIME		300
#define HIT_PARTICLE_EMERGE_TIME		100
#define BRICK_PARTICLE_EMERGE_TIME		1000

#define DEPENDENT_ID				9999 // taken from PlayScene.cpp

class CParticle :
	public CGameObject
{
protected:
	float ax, ay; // acceleration for brick
	int isEmerging = 0;
	int type; // 0 : point, 1: 1-up, 2: tanooki transition, 3: hit particle, 4: death smoke, 5: brick particle
	int point;
	ULONGLONG emergingStart = -1;
public:
	CParticle(int id, float x, float y, int z, int type, int point = 0);
	virtual ~CParticle() {}
	void Render();
	void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects = nullptr);
	void GetBoundingBox(float& l, float& t, float& r, float& b);
	int IsCollidable() { return 0; };
	int IsBlocking() { return 0; }
	void SetState(int state);
	void SetAccel(float ax, float ay) { this->ax = ax; this->ay = ay; }
	void StartEmerging() { isEmerging = 1; emergingStart = GetTickCount64(); }
	static void GenerateParticleInChunk(LPGAMEOBJECT obj, int type, int point = 0);
};

