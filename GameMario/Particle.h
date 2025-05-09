#pragma once
#include "GameObject.h"

#define ID_SPRITE_POINT_100 1120100
#define ID_SPRITE_POINT_200 1120101
#define ID_SPRITE_POINT_400 1120102
#define ID_SPRITE_POINT_800 1120103
#define ID_SPRITE_POINT_1000 1120110
#define ID_SPRITE_POINT_2000 1120111
#define ID_SPRITE_POINT_4000 1120112
#define ID_SPRITE_POINT_8000 1120113
#define ID_SPRITE_1_UP 1120120

#define PARTICLE_BBOX_WIDTH			0
#define PARTICLE_BBOX_HEIGHT			0
#define PARTICLE_FLOATING_VELOCITY -0.05f

#define PARTICLE_STATE_EMERGING		0
#define PARTICLE_STATE_EMERGING_COMPLETE	100

#define PARTICLE_EMERGE_TIME			600

#define DEPENDENT_ID				9999 // taken from PlayScene.cpp

class CParticle :
    public CGameObject
{
protected:
	int isEmerging = 0;
	int point;
	ULONGLONG emergingStart = -1;
public:
	CParticle(int id, float x, float y, int z, int point);
	virtual ~CParticle() {}
	void Render();
	void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects = nullptr);
	void GetBoundingBox(float& l, float& t, float& r, float& b);
	int IsCollidable() { return 0; };
	int IsBlocking() { return 0; }
	void SetState(int state);
	void StartEmerging() { isEmerging = 1; emergingStart = GetTickCount64(); }
	static void GenerateParticleInChunk(LPGAMEOBJECT obj, int point);
};

