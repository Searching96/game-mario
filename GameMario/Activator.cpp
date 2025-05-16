#include "Activator.h"
#include "Game.h"
#include "Chunk.h"
#include "PlayScene.h"

CActivator::CActivator(int id, float x, float y, int z, int orignalChunkId) : CGameObject(id, x, y, z)
{
	this->originalChunkId = orignalChunkId;
}

void CActivator::Render()
{
	int aniId = ID_ANI_ACTIVATOR_UNACTIVATED;
	if (isActivated)
	{
		aniId = ID_ANI_ACTIVATOR_ACTIVATED;
	}
	CAnimations::GetInstance()->Get(aniId)->Render(x, y);

	//RenderBoundingBox();
}

void CActivator::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	CGameObject::Update(dt, coObjects);
	CCollision::GetInstance()->Process(this, dt, coObjects);
}

void CActivator::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x - ACTIVATOR_CELL_WIDTH / 2;
	t = y - ACTIVATOR_CELL_HEIGHT / 2;
	r = l + ACTIVATOR_CELL_WIDTH;
	b = t + ACTIVATOR_CELL_HEIGHT;
}

void CActivator::SetState(int state)
{
	CGameObject::SetState(state);
	switch (state)
	{
		case COIN_BRICK_STATE_BREAK:
		{
			LPCHUNK chunk = ((LPPLAYSCENE)CGame::GetInstance()->GetCurrentScene())->GetChunk(originalChunkId);
			chunk->SetIsObjectDeleted(this->GetId(), true);
			isDeleted = true;
			break;
		}
	}
}

