#include "entities/blocks/HiddenCoinBrick.h"
#include "core/Game.h"
#include "world/Chunk.h"
#include "core/PlayScene.h"

CHiddenCoinBrick::CHiddenCoinBrick(int id, float x, float y, int z, int originalChunkId, vector<CCoin*> coinList) : CGameObject(id, x, y, z)
{
	this->originalChunkId = originalChunkId;
	this->coinList = coinList;
	SetState(QUESTIONBLOCK_STATE_NOT_HIT);
	remainingCoins = HIDDEN_COIN_BRICK_NUM_COINS;
}

void CHiddenCoinBrick::Render()
{
	int aniId = ID_ANI_BRICK;
	if (isHit)
	{
		aniId = ID_ANI_QUESTIONBLOCK_HIT;
	}

	CAnimations::GetInstance()->Get(aniId)->Render(x, y);

	//RenderBoundingBox();
}

void CHiddenCoinBrick::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
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

void CHiddenCoinBrick::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x - QUESTIONBLOCK_BBOX_WIDTH / 2;
	t = y - QUESTIONBLOCK_BBOX_HEIGHT / 2;
	r = l + QUESTIONBLOCK_BBOX_WIDTH;
	b = t + QUESTIONBLOCK_BBOX_HEIGHT;
}

void CHiddenCoinBrick::SetState(int state)
{
	switch (state)
	{
		case QUESTIONBLOCK_STATE_NOT_HIT:
			isHit = false;
			break;
		case QUESTIONBLOCK_STATE_BOUNCE_UP:
		{
			y0 = y;
			vy = QUESTIONBLOCK_BOUNCE_SPEED;
			CCoin* coin = coinList[remainingCoins - 1];
			coinList.pop_back();
			remainingCoins--;
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
		{
			vy = 0.0f;
			if (remainingCoins == 0)
			{
				LPCHUNK chunk = ((LPPLAYSCENE)CGame::GetInstance()->GetCurrentScene())->GetChunk(originalChunkId);
				chunk->SetIsObjectConsumed(this->GetId(), true);
				isHit = true;
			}
			else
			{
				state = QUESTIONBLOCK_STATE_NOT_HIT;
			}
			break;
		}
	}
	CGameObject::SetState(state);
}

