#include "CoinBrick.h"
#include "core/Game.h"
#include "world/Chunk.h"
#include "core/PlayScene.h"

CCoinBrick::CCoinBrick(int id, float x, float y, int z, int orignalChunkId) : CGameObject(id, x, y, z)
{
	this->originalChunkId = orignalChunkId;
	this->SetState(COIN_BRICK_STATE_NOT_REVEALED);
}

void CCoinBrick::Render()
{
	int aniId = ID_ANI_BRICK;
	if (isRevealed)
	{
		aniId = ID_ANI_STATIC_HIDDEN_COIN;
	}
	CAnimations::GetInstance()->Get(aniId)->Render(x, y);

	//RenderBoundingBox();
}

void CCoinBrick::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	if (isRevealed)
	{
		if (GetTickCount64() - revealStart > COIN_BRICK_REVEAL_TIMEOUT)
		{
			SetState(COIN_BRICK_STATE_NOT_REVEALED);
		}
	}
	CGameObject::Update(dt, coObjects);
	CCollision::GetInstance()->Process(this, dt, coObjects);
}

void CCoinBrick::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x - BRICK_CELL_WIDTH / 2;
	t = y - BRICK_CELL_HEIGHT / 2;
	r = l + BRICK_CELL_WIDTH;
	b = t + BRICK_CELL_HEIGHT;
}

void CCoinBrick::SetState(int state)
{
	CGameObject::SetState(state);
	switch (state)
	{
		case COIN_BRICK_STATE_NOT_REVEALED:
		{
			isRevealed = false;
			break;
		}
		case COIN_BRICK_STATE_BREAK:
		{
			LPCHUNK chunk = ((LPPLAYSCENE)CGame::GetInstance()->GetCurrentScene())->GetChunk(originalChunkId);
			chunk->SetIsObjectDeleted(this->GetId(), true);
			isDeleted = true;
			CParticle::GenerateParticleInChunk(this, 5);
			break;
		}
		case COIN_BRICK_STATE_REVEALED:
		{
			StartReveal();
			break;
		}
		case COIN_BRICK_STATE_CONSUMED:
		{
			LPCHUNK chunk = ((LPPLAYSCENE)CGame::GetInstance()->GetCurrentScene())->GetChunk(originalChunkId);
			chunk->SetIsObjectDeleted(this->GetId(), true);
			isDeleted = true;
			break;
		}
	}
}

