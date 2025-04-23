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
	if (state == PIRANHA_PLANT_STATE_DIED)
	{
		aniId = ID_ANI_PIRANHA_PLANT_DIED;
	}
	else if (moveUp || moveDown)
	{
		switch (direction)
		{
		case 0:
		case 1:
		case 2:
		case 3:
			aniId = ID_ANI_PIRANHA_PLANT_LEFT_MOVE;
			break;
		case 4:
		case 5:
		case 6:
		case 7:
			aniId = ID_ANI_PIRANHA_PLANT_RIGHT_MOVE;
			break;
		}
	}
	else
	{
		switch (direction)
		{
		case 0:
		case 1:
			aniId = ID_ANI_PIRANHA_PLANT_BOTTOM_LEFT_SHOOT;
			break;
		case 2:
		case 3:
			aniId = ID_ANI_PIRANHA_PLANT_TOP_LEFT_SHOOT;
			break;
		case 4:
		case 5:
			aniId = ID_ANI_PIRANHA_PLANT_TOP_RIGHT_SHOOT;
			break;
		case 6:
		case 7:
			aniId = ID_ANI_PIRANHA_PLANT_BOTTOM_RIGHT_SHOOT;
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
	if (fireball == NULL) return;
	switch (direction)
	{
	case 0:
		fireball->SetState(FIREBALL_STATE_SHOOT_BOTTOM_LEFT);
		//DebugOut(L"[INFO] Shoot left down sharp\n");
		break;
	case 1:
		fireball->SetState(FIREBALL_STATE_SHOOT_LOWER_CENTER_LEFT);
		//DebugOut(L"[INFO] Shoot left down shallow\n");
		break;
	case 2:
		fireball->SetState(FIREBALL_STATE_SHOOT_UPPER_CENTER_LEFT);
		//DebugOut(L"[INFO] Shoot left up shallow\n");
		break;
	case 3:
		fireball->SetState(FIREBALL_STATE_SHOOT_TOP_LEFT);
		//DebugOut(L"[INFO] Shoot left up sharp\n");
		break;
	case 4:
		fireball->SetState(FIREBALL_STATE_SHOOT_TOP_RIGHT);
		//DebugOut(L"[INFO] Shoot right up sharp\n");
		break;
	case 5:
		fireball->SetState(FIREBALL_STATE_SHOOT_UPPER_CENTER_RIGHT);
		//DebugOut(L"[INFO] Shoot right up shallow\n");
		break;
	case 6:
		fireball->SetState(FIREBALL_STATE_SHOOT_LOWER_CENTER_RIGHT);
		//DebugOut(L"[INFO] Shoot right down shallow\n");
		break;
	case 7:
		fireball->SetState(FIREBALL_STATE_SHOOT_BOTTOM_RIGHT);
		//DebugOut(L"[INFO] Shoot right down sharp\n");
		break;
	default:
		break;
	}
}

bool CPiranhaPlant::IsMarioOnTop()
{
	CMario* player = (CMario*)((LPPLAYSCENE)CGame::GetInstance()->GetCurrentScene())->GetPlayer();
	float mario_X, mario_Y;
	player->GetPosition(mario_X, mario_Y);
	return (fabs(mario_X - this->x) < PIRANHA_PLANT_BBOX_WIDTH && fabs(mario_Y - this->y) < PIRANHA_PLANT_BBOX_HEIGHT);
}

void CPiranhaPlant::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	if (state == PIRANHA_PLANT_STATE_DIED)
	{
		if (GetTickCount64() - deathStart > PIRANHA_PLANT_DIE_TIMEOUT)
		{
			isDeleted = true;
		}
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
			fireball->SetEnable(0);
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
		StartDeath();
		break;
	}
}
