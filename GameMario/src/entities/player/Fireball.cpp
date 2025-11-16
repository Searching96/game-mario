#include "entities/player/Fireball.h"
#include "entities/enemies/PiranhaPlant.h"
#include "entities/blocks/QuestionBlock.h"

void CFireball::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x - FIREBALL_BBOX_WIDTH / 2;
	t = y - FIREBALL_BBOX_HEIGHT / 2;
	r = l + FIREBALL_BBOX_WIDTH;
	b = t + FIREBALL_BBOX_HEIGHT;
}



void CFireball::Render()
{
	if (state == FIREBALL_STATE_STATIC) return;
	int aniId = ID_ANI_FIREBALL_DYNAMIC;
	CAnimations* animations = CAnimations::GetInstance();
	animations->Get(aniId)->Render(x, y);

	//RenderBoundingBox();
}

void CFireball::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	if (state == FIREBALL_STATE_STATIC) return;
	x += vx * dt;
	y += vy * dt;

	float camX, camY;
	float camWidth = CGame::GetInstance()->GetBackBufferWidth() / 2.0f;
	float camHeight = CGame::GetInstance()->GetBackBufferHeight() / 2.0f;
	CGame::GetInstance()->GetCamPos(camX, camY);

	if (fabs(x - camX - camWidth) > camWidth * 1.125f || fabs(y - camY - camHeight) > camHeight * 1.125f)
	{
		this->Delete();
		return;
	}

	//DebugOut(L"[FIREBALL] x: %f, y: %f, vx: %f, vy: %f, state: %i\n", x, y, vx, vy, state);
	//DebugOut(L"[FIREBALL] x0: %f, y0: %f\n", x0, y0);
}

void CFireball::SetState(int state)
{
	switch (state)
	{
	case FIREBALL_STATE_STATIC:
		vx = 0;
		vy = 0;
		this->Delete();
		break;
	case FIREBALL_STATE_SHOOT_TOP_RIGHT:
		vx = FIREBALL_SPEED;
		vy = -FIREBALL_SPEED;
		break;
	case FIREBALL_STATE_SHOOT_TOP_LEFT:
		vx = -FIREBALL_SPEED;
		vy = -FIREBALL_SPEED;
		break;
	case FIREBALL_STATE_SHOOT_UPPER_CENTER_RIGHT:
		vx = FIREBALL_SPEED * 3 * INVERSE_SQUARE_ROOT_10;
		vy = -FIREBALL_SPEED * INVERSE_SQUARE_ROOT_10;
		break;
	case FIREBALL_STATE_SHOOT_UPPER_CENTER_LEFT:
		vx = -FIREBALL_SPEED * 3 * INVERSE_SQUARE_ROOT_10;
		vy = -FIREBALL_SPEED * INVERSE_SQUARE_ROOT_10;
		break;
	case FIREBALL_STATE_SHOOT_LOWER_CENTER_RIGHT:
		vx = FIREBALL_SPEED * 3 * INVERSE_SQUARE_ROOT_10;
		vy = FIREBALL_SPEED * INVERSE_SQUARE_ROOT_10;
		break;
	case FIREBALL_STATE_SHOOT_LOWER_CENTER_LEFT:
		vx = -FIREBALL_SPEED * 3 * INVERSE_SQUARE_ROOT_10;
		vy = FIREBALL_SPEED * INVERSE_SQUARE_ROOT_10;
		break;
	case FIREBALL_STATE_SHOOT_BOTTOM_RIGHT:
		vx = FIREBALL_SPEED;
		vy = FIREBALL_SPEED;
		break;
	case FIREBALL_STATE_SHOOT_BOTTOM_LEFT:
		vx = -FIREBALL_SPEED;
		vy = FIREBALL_SPEED;
		break;
	}
	CGameObject::SetState(state);
}

void CFireball::Shoot(LPGAMEOBJECT obj, int direction)
{
	if (obj == nullptr) return;

	vector<LPCHUNK> chunks = ((LPPLAYSCENE)CGame::GetInstance()->GetCurrentScene())->GetLoadedChunks();

	if (chunks.size() == 0) return;

	for (auto chunk : chunks)
	{
		if (chunk->IsObjectInChunk(obj))
		{
			float obj_x, obj_y;
			obj->GetPosition(obj_x, obj_y);
			CFireball* fireball = new CFireball(DEPENDENT_ID, obj_x, obj_y - 8, INT_MAX, direction);
			chunk->AddObject(fireball);
		}
	}
}