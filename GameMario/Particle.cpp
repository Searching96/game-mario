#include "Particle.h"
#include "PlayScene.h"

CParticle::CParticle(int id, float x, float y, int z, int point) : CGameObject(id, x, y, z) {
	vy = PARTICLE_FLOATING_VELOCITY;
	SetState(PARTICLE_STATE_EMERGING);
	this->point = point;
}

void CParticle::Render()
{
	if (isEmerging == 0) return;

	CSprites* sprites = CSprites::GetInstance();
	int spriteId = -1;
	switch (point)
	{
	case 100:
		spriteId = ID_SPRITE_POINT_100;
		break;
	case 200:
		spriteId = ID_SPRITE_POINT_200;
		break;
	case 400:
		spriteId = ID_SPRITE_POINT_400;
		break;
	case 800:
		spriteId = ID_SPRITE_POINT_800;
		break;
	case 1000:
		spriteId = ID_SPRITE_POINT_1000;
		break;
	case 2000:
		spriteId = ID_SPRITE_POINT_2000;
		break;
	case 4000:
		spriteId = ID_SPRITE_POINT_4000;
		break;
	case 8000:
		spriteId = ID_SPRITE_POINT_8000;
		break;
	default:
		spriteId = ID_SPRITE_1_UP; // Default to 1 up
		break;
	}
	if (spriteId != -1)
		sprites->Get(spriteId)->Draw(x, y);

	//RenderBoundingBox(); // Debug only
}

void CParticle::Update(DWORD dt, vector<LPGAMEOBJECT>* /*coObjects*/) // Ignore coObjects
{
	y += vy * dt;
	if (state == PARTICLE_STATE_EMERGING) {
		if (GetTickCount64() - emergingStart > PARTICLE_EMERGE_TIME) {
			SetState(PARTICLE_STATE_EMERGING_COMPLETE);
		}
	}
}

void CParticle::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x - PARTICLE_BBOX_WIDTH / 2;
	t = y - PARTICLE_BBOX_HEIGHT / 2;
	r = l + PARTICLE_BBOX_WIDTH;
	b = t + PARTICLE_BBOX_HEIGHT;
}

void CParticle::SetState(int state)
{
	CGameObject::SetState(state);
	switch (state)
	{

	case PARTICLE_STATE_EMERGING:
		if (isEmerging == 0) {
			isEmerging = 1;
			emergingStart = GetTickCount64();
		}
		break;
	case PARTICLE_STATE_EMERGING_COMPLETE:
		this->Delete();
		break;
	}
}

void CParticle::GenerateParticleInChunk(LPGAMEOBJECT obj, int point) {
	if (obj == nullptr) return;

	vector<LPCHUNK> chunks = ((LPPLAYSCENE)CGame::GetInstance()->GetCurrentScene())->GetLoadedChunks();
	
	if (chunks.size() == 0) return;

	for (auto chunk : chunks)
	{
		if (chunk->IsObjectInChunk(obj))
		{
			float obj_x, obj_y;
			obj->GetPosition(obj_x, obj_y);
			CParticle* particle = new CParticle(DEPENDENT_ID, obj_x, obj_y, INT_MAX, point);
			chunk->AddObject(particle);
		}
	}
}
