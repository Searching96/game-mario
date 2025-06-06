#include "PiranhaPlant.h"

CPiranhaPlant::CPiranhaPlant(int id, float x, float y, int z, int type, int originalChunkId) : CGameObject(id, x, y, z)
{
	this->originalChunkId = originalChunkId;
	this->x = x;
	this->y = y;
	this->type = type;
	x0 = x;
	y0 = y;
	SetState(PIRANHA_PLANT_STATE_ASCEND);
}

void CPiranhaPlant::Render()
{
	if (isDefeated == 1)
		return;

	if (state == PIRANHA_PLANT_STATE_HIDDEN || state == PIRANHA_PLANT_STATE_DIE) return;
	int aniId = ID_ANI_PIRANHA_PLANT_LEFT_MOVE;
	int direction = GetAiming();
	if (type == 2)
	{ 
		aniId = ID_ANI_GREEN_PLANT_UPRIGHT;
	}
	else if (moveUp || moveDown)
	{
		switch (direction)
		{
		case 0:
		case 1:
		case 2:
		case 3:
			aniId = type == 0 ? ID_ANI_PIRANHA_PLANT_LEFT_MOVE : ID_ANI_GREEN_PLANT_LEFT_MOVE;
			break;
		case 4:
		case 5:
		case 6:
		case 7:
			aniId = type == 0 ? ID_ANI_PIRANHA_PLANT_RIGHT_MOVE : ID_ANI_GREEN_PLANT_RIGHT_MOVE;
			break;
		}
	}
	else
	{
		switch (direction)
		{
		case 0:
		case 1:
			aniId = type == 0 ? ID_ANI_PIRANHA_PLANT_BOTTOM_LEFT_SHOOT : ID_ANI_GREEN_PLANT_BOTTOM_LEFT_SHOOT;
			break;
		case 2:
		case 3:
			aniId = type == 0 ? ID_ANI_PIRANHA_PLANT_TOP_LEFT_SHOOT : ID_ANI_GREEN_PLANT_TOP_LEFT_SHOOT;
			break;
		case 4:
		case 5:
			aniId = type == 0 ? ID_ANI_PIRANHA_PLANT_TOP_RIGHT_SHOOT : ID_ANI_GREEN_PLANT_TOP_RIGHT_SHOOT;
			break;
		case 6:
		case 7:
			aniId = type == 0 ? ID_ANI_PIRANHA_PLANT_BOTTOM_RIGHT_SHOOT : ID_ANI_GREEN_PLANT_BOTTOM_RIGHT_SHOOT;
			break;
		}
	}

	CAnimations::GetInstance()->Get(aniId)->Render(x, y);

	//RenderBoundingBox();
}

// 0 bottom left, 1 lower center left, 2 upper center left, 3 top left
// 4 top right, 5 upper center right, 6 lower center right, 7 bottom right
int CPiranhaPlant::GetAiming()
{
	CMario* player = (CMario*)((LPPLAYSCENE)CGame::GetInstance()->GetCurrentScene())->GetPlayer();
	float mario_X, mario_Y;
	player->GetPosition(mario_X, mario_Y);
	float dx = mario_X - x;
	float dy = mario_Y - y;

	//if (std::fabs(dx) < 1e-6f && std::fabs(dy) < 1e-6f) {
	//	return 4; // Mario is at the same position as the plant
	//}

	float abs_dx = std::fabs(dx);
	float abs_dy = std::fabs(dy);

	bool is_shallow = abs_dy < abs_dx;

	if (dx >= 0)
	{
		//Right
		if (dy >= 0) // Lower
		{
			// Right + Lower
			if (is_shallow) {
				return 6; // Lower Center Right
			}
			else {
				return 7; // Bottom Right
			}
		}
		else // Upper
		{
			// Right + Upper
			if (is_shallow) {
				return 5; // Upper Center Right
			}
			else {
				return 4; // Top Right
			}
		}
	}
	else
	{
		//Left
		if (dy >= 0) // Lower
		{
			// Left + Lower
			if (is_shallow) {
				return 1; // Lower Center Left
			}
			else {
				return 0; // Bottom Left
			}
		}
		else // Upper
		{
			// Left + Upper
			if (is_shallow) {
				return 2; // Upper Center Left
			}
			else {
				return 3; // Top Left
			}
		}
	}
}

bool CPiranhaPlant::IsMarioInRange()
{
	CGame* game = CGame::GetInstance();
	CMario* player = (CMario*)((LPPLAYSCENE)game->GetCurrentScene())->GetPlayer();
	float mario_X, mario_Y;
	player->GetPosition(mario_X, mario_Y);
	return fabs(mario_X - this->x) < game->GetBackBufferWidth() / 2
		&& fabs(mario_Y - this->y) < game->GetBackBufferHeight() / 2;
}

void CPiranhaPlant::Shoot(int direction)
{
	CFireball::Shoot(this, direction);
}

bool CPiranhaPlant::IsMarioOnTop()
{
	CMario* player = (CMario*)((LPPLAYSCENE)CGame::GetInstance()->GetCurrentScene())->GetPlayer();
	float mario_X, mario_Y;
	player->GetPosition(mario_X, mario_Y);
	return (fabs(mario_X - this->x) < PIRANHA_PLANT_BBOX_WIDTH && fabs(mario_Y - this->y) < PIRANHA_PLANT_BBOX_HEIGHT);
}

void CPiranhaPlant::OnNoCollision(DWORD dt) {}

void CPiranhaPlant::OnCollisionWith(LPCOLLISIONEVENT e)
{
	if (dynamic_cast<CKoopa*>(e->obj))
		OnCollisionWithKoopa(e);
}

void CPiranhaPlant::OnCollisionWithKoopa(LPCOLLISIONEVENT e)
{
	CKoopa* k = dynamic_cast<CKoopa*>(e->obj);
	if (k->IsHeld() == 0)
	{
		return;
	}

	CMario* player = (CMario*)((LPPLAYSCENE)CGame::GetInstance()->GetCurrentScene())->GetPlayer();
	player->SetIsHoldingKoopa(0);
	k->SetIsHeld(0);

	k->SetState(KOOPA_STATE_DIE_ON_COLLIDE_WITH_ENEMY);
	this->SetState(PIRANHA_PLANT_STATE_DIE);
}

void CPiranhaPlant::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	if (isDefeated == 1)
		return;

	if (state == PIRANHA_PLANT_STATE_DIE)
	{
		if (GetTickCount64() - deathStart > PIRANHA_PLANT_DIE_TIMEOUT)
		{
			LPCHUNK chunk = ((LPPLAYSCENE)CGame::GetInstance()->GetCurrentScene())->GetChunk(originalChunkId);
			chunk->SetIsObjectDeleted(this->GetId(), true);
			isDeleted = true;
		}
		return;
	}

	if (state == PIRANHA_PLANT_STATE_HOVER)
	{
		if (!hasShot && type != 2 && GetTickCount64() - hoverStart > PIRANHA_PLANT_SHOOT_TIMEOUT)
		{
			//Shoot
			if (IsMarioInRange())
			{
				Shoot(GetAiming());
				hasShot = true;
			}
		}
		if (GetTickCount64() - hoverStart > PIRANHA_PLANT_HOVER_TIMEOUT)
		{
			SetState(PIRANHA_PLANT_STATE_DESCEND);
		}
	}
	else if (moveUp == 1)
	{
		if (fabs(y - y00) >= PIRANHA_PLANT_MOVE_OFFSET)
		{
			moveUp = 0;
			moveUpStart = -1;
			y = y00 - PIRANHA_PLANT_MOVE_OFFSET;
			SetState(PIRANHA_PLANT_STATE_HOVER);
		}
	}
	else if (moveDown == 1)
	{
		if (fabs(y - y00) >= PIRANHA_PLANT_MOVE_OFFSET)
		{
			moveDown = 0;
			moveDownStart = -1;
			y = y00 + PIRANHA_PLANT_MOVE_OFFSET;
			SetState(PIRANHA_PLANT_STATE_HIDDEN);
		}
	}
	else if (GetTickCount64() - lastMove > PIRANHA_PLANT_HIDE_TIMEOUT)
	{
		if (IsMarioOnTop())
		{
			lastMove = GetTickCount64();
			return;
		}
		SetState(PIRANHA_PLANT_STATE_ASCEND);
	}
	x += vx * dt;
	y += vy * dt;
	CGameObject::Update(dt, coObjects);
	CCollision::GetInstance()->Process(this, dt, coObjects);
}

void CPiranhaPlant::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x - PIRANHA_PLANT_BBOX_WIDTH / 2;
	t = y - PIRANHA_PLANT_BBOX_HEIGHT / 2;
	r = l + PIRANHA_PLANT_BBOX_WIDTH;
	b = t + PIRANHA_PLANT_BBOX_HEIGHT;
}

void CPiranhaPlant::SetState(int state)
{
	CGameObject::SetState(state);
	switch (state)
	{
	case PIRANHA_PLANT_STATE_HIDDEN:
		vy = 0.0f;
		hasShot = false;
		lastMove = GetTickCount64();
		break;
	case PIRANHA_PLANT_STATE_ASCEND:
		y00 = y;
		vy = PIRANHA_PLANT_MOVE_SPEED;
		StartMoveUp();
		break;
	case PIRANHA_PLANT_STATE_DESCEND:
		y00 = y;
		vy = -PIRANHA_PLANT_MOVE_SPEED;
		StartMoveDown();
		break;
	case PIRANHA_PLANT_STATE_HOVER:
		vy = 0.0f;
		hoverStart = GetTickCount64();
		break;
	case PIRANHA_PLANT_STATE_DIE:
		StartDeath();
		CParticle::GenerateParticleInChunk(this, 4);
		break;
	}
}

