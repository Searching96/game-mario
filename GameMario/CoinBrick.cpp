#include "CoinBrick.h"
#include "Game.h"
#include "Chunk.h"
#include "PlayScene.h"

CCoinBrick::CCoinBrick(int id, float x, float y, int z, int orignalChunkId) : CGameObject(id, x, y, z)
{
	this->originalChunkId = orignalChunkId;
}

void CCoinBrick::Render()
{
	int aniId = ID_ANI_BRICK;
	CAnimations::GetInstance()->Get(aniId)->Render(x, y);

	//RenderBoundingBox();
}

void CCoinBrick::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	CGameObject::Update(dt, coObjects);
	CCollision::GetInstance()->Process(this, dt, coObjects);
}

void CCoinBrick::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x - QUESTIONBLOCK_BBOX_WIDTH / 2;
	t = y - QUESTIONBLOCK_BBOX_HEIGHT / 2;
	r = l + QUESTIONBLOCK_BBOX_WIDTH;
	b = t + QUESTIONBLOCK_BBOX_HEIGHT;
}

void CCoinBrick::SetState(int state)
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

