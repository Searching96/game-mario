#include "entities/blocks/ActivatorBrick.h"
#include "core/Game.h"
#include "world/Chunk.h"
#include "core/PlayScene.h"

CActivatorBrick::CActivatorBrick(int id, float x, float y, int z, int orignalChunkId, CActivator* activator) : CGameObject(id, x, y, z)
{
	this->originalChunkId = orignalChunkId;
	this->activator = activator;
	this->SetState(QUESTIONBLOCK_STATE_NOT_HIT);
}

void CActivatorBrick::Render()
{
	int aniId = ID_ANI_BRICK;
	if (isHit || state != QUESTIONBLOCK_STATE_NOT_HIT)
	{
		aniId = ID_ANI_QUESTIONBLOCK_HIT;
	}
	CAnimations::GetInstance()->Get(aniId)->Render(x, y);

	//RenderBoundingBox();
}

void CActivatorBrick::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
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

void CActivatorBrick::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x - BRICK_CELL_WIDTH / 2;
	t = y - BRICK_CELL_HEIGHT / 2;
	r = l + BRICK_CELL_WIDTH;
	b = t + BRICK_CELL_HEIGHT;
}

void CActivatorBrick::SetState(int state)
{
	CGameObject::SetState(state);
	switch (state)
	{
		case QUESTIONBLOCK_STATE_NOT_HIT:
		{
			isHit = false;
			break;
		}
		case QUESTIONBLOCK_STATE_BOUNCE_UP:
		{
			LPCHUNK chunk = ((LPPLAYSCENE)CGame::GetInstance()->GetCurrentScene())->GetChunk(originalChunkId);
			chunk->SetIsObjectConsumed(this->GetId(), true);
			y0 = y;
			vy = QUESTIONBLOCK_BOUNCE_SPEED;
			StartBounceUp();
			activator->SetIsRevealed(true);
			break;
		}
		case QUESTIONBLOCK_STATE_BOUNCE_DOWN:
		{
			y0 = y;
			vy = -QUESTIONBLOCK_BOUNCE_SPEED;
			StartBounceDown();
			break;
		}
		case QUESTIONBLOCK_STATE_BOUNCE_COMPLETE:
		{
			vy = 0.0f;
			isHit = true;
			break;
		}
	}
}

