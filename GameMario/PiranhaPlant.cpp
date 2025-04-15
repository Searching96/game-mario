#include "PiranhaPlant.h"
#include "Game.h"
#include "PlayScene.h"

CPiranhaPlant::CPiranhaPlant(float x, float y) : CGameObject(x, y)
{
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

	CAnimations::GetInstance()->Get(aniId)->Render(x, y + PIRANHA_PLANT_BBOX_OFFSET);

	//RenderBoundingBox();
}

int CPiranhaPlant::GetAiming()
{
	CMario* player = dynamic_cast<CMario*>(dynamic_cast<CPlayScene*>(CGame::GetInstance()->GetCurrentScene())->GetPlayer());
	float mario_X, mario_Y;
	player->GetPosition(mario_X, mario_Y);
	if (mario_X < this->x)
	{
		if (mario_Y < this->y)
		{
			return 1; // left up
		}
		else
		{
			return 2; // left down
		}
	}
	else
	{
		if (mario_Y < this->y)
		{
			return 0; // right up
		}
		else
		{
			return 3; // right down
		}
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
		if (GetTickCount64() - hoverStart > PIRANHA_PLANT_SHOOT_TIMEOUT)
		{
			//Shoot
			switch (GetAiming())
			{
			case 0:
				// shoot right up
				break;
			case 1:
				// shoot left up
				break;
			case 2:
				// shoot left down
				break;
			case 3:
				// shoot right down
				break;
			}
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
	t = y - PIRANHA_PLANT_BBOX_HEIGHT / 2 + PIRANHA_PLANT_BBOX_OFFSET;
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
		lastMove = GetTickCount64();
		break;
	case PIRANHA_PLANT_STATE_ASCEND:
		y0 = y;
		vy = PIRANHA_PLANT_BOUNCE_SPEED;
		StartMoveUp();
		break;
	case PIRANHA_PLANT_STATE_DESCEND:
		y0 = y;
		vy = -PIRANHA_PLANT_BOUNCE_SPEED;
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
