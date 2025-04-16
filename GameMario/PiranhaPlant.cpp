#include "PiranhaPlant.h"
#include "Game.h"
#include "PlayScene.h"

CPiranhaPlant::CPiranhaPlant(float x, float y, CFireball* fireball) : CGameObject(x, y)
{
	this->fireball = fireball;
	this->x = x;
	this->y = y;
	SetState(PIRANHA_PLANT_STATE_HIDDEN);
}

void CPiranhaPlant::Render()
{
	if (state == PIRANHA_PLANT_STATE_HIDDEN) return;
	int aniId = ID_ANI_PIRANHA_PLANT_LEFT_MOVE;
	int direction = GetAiming();
	if (moveUp || moveDown)
		{
			switch (direction)
			{
			case 1:
			case 2:

				aniId = ID_ANI_PIRANHA_PLANT_LEFT_MOVE;
				break;
			case 0:
			case 3:
				aniId = ID_ANI_PIRANHA_PLANT_RIGHT_MOVE;
				break;
			}
		}
		else
		{
			switch (direction)
			{
			case 0:
				aniId = ID_ANI_PIRANHA_PLANT_TOP_RIGHT_SHOOT;
				break;
			case 1:
				aniId = ID_ANI_PIRANHA_PLANT_TOP_LEFT_SHOOT;
				break;
			case 2:
				aniId = ID_ANI_PIRANHA_PLANT_BOTTOM_LEFT_SHOOT;
				break;
			case 3:
				aniId = ID_ANI_PIRANHA_PLANT_BOTTOM_RIGHT_SHOOT;
				break;
			}
		}

		CAnimations::GetInstance()->Get(aniId)->Render(x, y);

		RenderBoundingBox();
}

int CPiranhaPlant::GetAiming()
{
	CMario* player = (CMario*)((LPPLAYSCENE)CGame::GetInstance()->GetCurrentScene())->GetPlayer();
	float mario_X, mario_Y;
	player->GetPosition(mario_X, mario_Y);
	if (mario_Y < this->y - PIRANHA_PLANT_BBOX_OFFSET)
	{
		if (mario_X > this->x)
		{
			return 0; // right up
		}
		else
		{
			return 1; // left up
		}
	}
	else
	{
		if (mario_X < this->x)
		{
			return 2; // left down
		}
		else
		{
			return 3; // right down
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
	if (fireball == NULL) return;
	switch (direction)
	{
	case 0:
		fireball->SetState(FIREBALL_STATE_SHOOT_TOP_RIGHT);
		DebugOut(L"[INFO] Shoot right up\n");
		break;
	case 1:
		fireball->SetState(FIREBALL_STATE_SHOOT_TOP_LEFT);
		DebugOut(L"[INFO] Shoot left up\n");
		break;
	case 2:
		fireball->SetState(FIREBALL_STATE_SHOOT_BOTTOM_LEFT);
		DebugOut(L"[INFO] Shoot left down\n");
		break;
	case 3:
		fireball->SetState(FIREBALL_STATE_SHOOT_BOTTOM_RIGHT);
		DebugOut(L"[INFO] Shoot right down\n");
		break;
	default:
		break;
	}
}

void CPiranhaPlant::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	if ((state == PIRANHA_PLANT_STATE_DIED) && (GetTickCount64() - die_start > PIRANHA_PLANT_DIE_TIMEOUT))
	{
		isDeleted = true;
		return;
	}

	if (state == PIRANHA_PLANT_STATE_HOVER)
	{
		if (!hasShot && GetTickCount64() - hoverStart > PIRANHA_PLANT_SHOOT_TIMEOUT)
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
		if (fabs(y - y0) >= PIRANHA_PLANT_MOVE_OFFSET)
		{
			moveUp = 0;
			moveUpStart = -1;
			y = y0 - PIRANHA_PLANT_MOVE_OFFSET;
			SetState(PIRANHA_PLANT_STATE_HOVER);
		}
	}
	else if (moveDown == 1)
	{
		if (fabs(y - y0) >= PIRANHA_PLANT_MOVE_OFFSET)
		{
			moveDown = 0;
			moveDownStart = -1;
			y = y0 + PIRANHA_PLANT_MOVE_OFFSET;
			SetState(PIRANHA_PLANT_STATE_HIDDEN);
		}
	}
	else if (GetTickCount64() - lastMove > PIRANHA_PLANT_HIDE_TIMEOUT)
	{
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
		y0 = y;
		vy = PIRANHA_PLANT_MOVE_SPEED;
		StartMoveUp();
		break;
	case PIRANHA_PLANT_STATE_DESCEND:
		y0 = y;
		vy = -PIRANHA_PLANT_MOVE_SPEED;
		StartMoveDown();
		break;
	case PIRANHA_PLANT_STATE_HOVER:
		vy = 0.0f;
		hoverStart = GetTickCount64();
		break;
	case PIRANHA_PLANT_STATE_DIED:
		die_start = GetTickCount64();
		break;
	}
}
