#include "engine/rendering/Particle.h"
#include "core/PlayScene.h"

#define BRICK_PARTICLE_GRAVITY 0.001f
#define BRICK_PARTICLE_BOUNCE_VX 0.07f
#define BRICK_PARTICLE_BOUNCE_VY 0.27f


CParticle::CParticle(int id, float x, float y, int z, int type, int point) : CGameObject(id, x, y, z) {
	vy = type <= 1 ? PARTICLE_FLOATING_VELOCITY : 0;
	ay = type == 5 ? BRICK_PARTICLE_GRAVITY : 0;
	SetState(PARTICLE_STATE_EMERGING);
	this->type = type;
	this->point = point;
}

void CParticle::Render()
{
	if (isEmerging == 0) return;

	CAnimations* ani = CAnimations::GetInstance();
	int aniId = -1;
	switch (type) {
	case 0: // Point particle
	{
		switch (point)
		{

		case 200:
			aniId = ID_ANI_POINT_200;
			break;
		case 400:
			aniId = ID_ANI_POINT_400;
			break;
		case 800:
			aniId = ID_ANI_POINT_800;
			break;
		case 1000:
			aniId = ID_ANI_POINT_1000;
			break;
		case 2000:
			aniId = ID_ANI_POINT_2000;
			break;
		case 4000:
			aniId = ID_ANI_POINT_4000;
			break;
		case 8000:
			aniId = ID_ANI_POINT_8000;
			break;
		case 100:
		default:
			aniId = ID_ANI_POINT_100;
			break;
		}
		break;
	}
	case 1: // 1-Up particle
		aniId = ID_ANI_1_UP;
		break;
	case 2: // Tanooki transition particle
		aniId = ID_ANI_TANOOKI_TRANSITION;
		break;
	case 3: // Hit particle
		aniId = ID_ANI_HIT_PARTICLE;
		break;
	case 4: // Death smoke particle
		aniId = ID_ANI_DEATH_SMOKE;
		break;
	case 5: // Brick particle
		aniId = ID_ANI_BRICK_PARTICLE;
		break;
	}

	if (aniId != -1)
		ani->Get(aniId)->Render(x, y);

	//RenderBoundingBox(); // Debug only
}

void CParticle::Update(DWORD dt, vector<LPGAMEOBJECT>* /*coObjects*/) // Ignore coObjects
{
	vy += ay * dt;
	x += vx * dt;
	y += vy * dt;
	if (state == PARTICLE_STATE_EMERGING) {
		int emergeTime = 0;
		switch (type) {
		case 1: // 1-Up particle
			emergeTime = PARTICLE_EMERGE_TIME;
			break;
		case 2: // Tanooki transition particle
			emergeTime = TANOOKI_TRANSITION_EMERGE_TIME;
			break;
		case 3: // Hit particle
			emergeTime = HIT_PARTICLE_EMERGE_TIME;
			break;
		case 4: // Death smoke particle
			emergeTime = DEATH_SMOKE_EMERGE_TIME;
			break;
		case 5: // Brick particle
			emergeTime = BRICK_PARTICLE_EMERGE_TIME;
			break;
		case 0: // Point particle
		default:
			emergeTime = PARTICLE_EMERGE_TIME;
			break;
		}
		if (GetTickCount64() - emergingStart > emergeTime) {
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

void CParticle::GenerateParticleInChunk(LPGAMEOBJECT obj, int type, int point) {
	if (obj == nullptr) return;

	vector<LPCHUNK> chunks = ((LPPLAYSCENE)CGame::GetInstance()->GetCurrentScene())->GetLoadedChunks();

	if (chunks.size() == 0) return;

	for (auto chunk : chunks)
	{
		if (chunk->IsObjectInChunk(obj))
		{
			float obj_x, obj_y;
			obj->GetPosition(obj_x, obj_y);
			if (type != 5) {
				chunk->AddObject(new CParticle(DEPENDENT_ID, obj_x, obj_y, INT_MAX, type, point));
			}
			else {
				CParticle* particle1 = new CParticle(DEPENDENT_ID, obj_x - 8, obj_y - 8, INT_MAX, type, point);
				particle1->SetSpeed(-BRICK_PARTICLE_BOUNCE_VX, -BRICK_PARTICLE_BOUNCE_VY * 1.5);
				chunk->AddObject(particle1);
				CParticle* particle2 = new CParticle(DEPENDENT_ID, obj_x + 8, obj_y - 8, INT_MAX, type, point);
				particle2->SetSpeed(BRICK_PARTICLE_BOUNCE_VX, -BRICK_PARTICLE_BOUNCE_VY * 1.5);
				chunk->AddObject(particle2);
				CParticle* particle3 = new CParticle(DEPENDENT_ID, obj_x - 8, obj_y + 8, INT_MAX, type, point);
				particle3->SetSpeed(-BRICK_PARTICLE_BOUNCE_VX, -BRICK_PARTICLE_BOUNCE_VY);
				chunk->AddObject(particle3);
				CParticle* particle4 = new CParticle(DEPENDENT_ID, obj_x + 8, obj_y + 8, INT_MAX, type, point);
				particle4->SetSpeed(BRICK_PARTICLE_BOUNCE_VX, -BRICK_PARTICLE_BOUNCE_VY);
				chunk->AddObject(particle4);
			}
		}
	}
}
