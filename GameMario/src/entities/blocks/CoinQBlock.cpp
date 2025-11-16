#include "CoinQBlock.h"
#include "entities/items/Coin.h"
#include "core/Game.h"
#include "world/Chunk.h"
#include "core/PlayScene.h"

CCoinQBlock::CCoinQBlock(int id, float x, float y, int z, int originalChunkId, CCoin* coin) : CGameObject(id, x, y, z)
{
	this->originalChunkId = originalChunkId;
	this->coin = coin;
	SetState(QUESTIONBLOCK_STATE_NOT_HIT);
}

void CCoinQBlock::Render()
{
	int aniId = ID_ANI_QUESTIONBLOCK;
	if (isHit || state != QUESTIONBLOCK_STATE_NOT_HIT)
	{
		aniId = ID_ANI_QUESTIONBLOCK_HIT;
	}

	CAnimations::GetInstance()->Get(aniId)->Render(x, y);

	//RenderBoundingBox();
}

void CCoinQBlock::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	if (bounceUp)
	{
		if (fabs(y - y0) >= QUESTIONBLOCK_BOUNCE_OFFSET)
		{
			bounceUp = 0;
			y = y0 - QUESTIONBLOCK_BOUNCE_OFFSET;
			SetState(QUESTIONBLOCK_STATE_BOUNCE_DOWN);
		}
	}
	else if (bounceDown == 1)
	{
		if (fabs(y - y0) >= QUESTIONBLOCK_BOUNCE_OFFSET)
		{
			bounceDown = 0;
			y = y0 + QUESTIONBLOCK_BOUNCE_OFFSET;
			SetState(QUESTIONBLOCK_STATE_BOUNCE_COMPLETE);
		}
	}
	x += vx * dt;
	y += vy * dt;
	CGameObject::Update(dt, coObjects);
	CCollision::GetInstance()->Process(this, dt, coObjects);
}

void CCoinQBlock::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x - QUESTIONBLOCK_BBOX_WIDTH / 2;
	t = y - QUESTIONBLOCK_BBOX_HEIGHT / 2;
	r = l + QUESTIONBLOCK_BBOX_WIDTH;
	b = t + QUESTIONBLOCK_BBOX_HEIGHT;
}

void CCoinQBlock::SetState(int state)
{
	switch (state)
	{
	case QUESTIONBLOCK_STATE_NOT_HIT:
		isHit = false;
		break;
	case QUESTIONBLOCK_STATE_BOUNCE_UP:
	{
		LPCHUNK chunk = ((LPPLAYSCENE)CGame::GetInstance()->GetCurrentScene())->GetChunk(originalChunkId);
		chunk->SetIsObjectConsumed(this->GetId(), true);
		y0 = y;
		vy = QUESTIONBLOCK_BOUNCE_SPEED;
		coin->Activate();
		StartBounceUp();
		break;
	}
	case QUESTIONBLOCK_STATE_BOUNCE_DOWN:
		y0 = y;
		vy = -QUESTIONBLOCK_BOUNCE_SPEED;
		StartBounceDown();
		break;
	case QUESTIONBLOCK_STATE_BOUNCE_COMPLETE:
		vy = 0.0f;
		isHit = true;
		break;
	}
	CGameObject::SetState(state);
}

