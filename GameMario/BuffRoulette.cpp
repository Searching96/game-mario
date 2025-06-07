#include "BuffRoulette.h"
#include "PlayScene.h"

CBuffRoulette::CBuffRoulette(int id, float x, float y, int z, int nextScene) : CGameObject(id, x, y, z)
{
	this->SetState(BUFF_STATE_NOT_USED);
	next_scene = nextScene;
}

void CBuffRoulette::Render()
{
	int aniId = -1;

	if (state == BUFF_STATE_COMPLETED) return;

	switch (type)
	{
	case 1:
		aniId = (state == BUFF_STATE_USED) ? ID_ANI_MUSHROOM_USED : ID_ANI_MUSHROOM_NORMAL;
		break;
	case 2:
		aniId = (state == BUFF_STATE_USED) ? ID_ANI_FLOWER_USED : ID_ANI_FLOWER_NORMAL;
		break;
	case 3:
		aniId = (state == BUFF_STATE_USED) ? ID_ANI_STAR_USED : ID_ANI_STAR_NORMAL;
		break;
	}

	if (aniId != -1)
	{
		CAnimations* animations = CAnimations::GetInstance();
		animations->Get(aniId)->Render(x, y);
	}

	//RenderBoundingBox();
}

void CBuffRoulette::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	if (state == BUFF_STATE_COMPLETED || isDeleted) return;

	if (state == BUFF_STATE_NOT_USED)
	{
		if (lastTypeSwitch != -1 && GetTickCount64() - lastTypeSwitch > TYPE_SWITCH_TIMEOUT)
		{
			lastTypeSwitch = GetTickCount64();
			type = (type + 1) % 3 + 1;
		}
	}
	else if (state == BUFF_STATE_USED)
	{
		if (usedStart != -1 && GetTickCount64() - usedStart > USED_TIMEOUT)
		{
			CGame::GetInstance()->GetGameState()->AddCard(type);
			SetState(BUFF_STATE_COMPLETED);
		}
	}

	DebugOut(L"[BUFF] Type: %d, State: %d, lastTypeSwitch: %d, usedStart : %d\n", type, state, lastTypeSwitch, usedStart);

	y += vy * dt;

	CGameObject::Update(dt, coObjects);
	CCollision::GetInstance()->Process(this, dt, coObjects);
}

void CBuffRoulette::OnNoCollision(DWORD dt)
{
	return;
}

void CBuffRoulette::OnCollisionWith(LPCOLLISIONEVENT e)
{
	return;
}

void CBuffRoulette::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x - BUFF_BBOX_WIDTH / 2;
	t = y - BUFF_BBOX_HEIGHT / 2;
	r = l + BUFF_BBOX_WIDTH;
	b = t + BUFF_BBOX_HEIGHT;
}


void CBuffRoulette::SetState(int state)
{
	switch (state)
	{
	case BUFF_STATE_NOT_USED:
		break;
	case BUFF_STATE_USED:
		lastTypeSwitch = -1;
		vx = 0;
		vy = -BUFF_FLOAT_SPEED;
		usedStart = GetTickCount64();
		break;
	case BUFF_STATE_COMPLETED:
		this->Delete();
		CGame* game = CGame::GetInstance();
		LPPLAYSCENE(game->GetCurrentScene())->InitializeSwitchSceneSequence();
		game->InitiateSwitchScene(next_scene);
	}
	CGameObject::SetState(state);
}
