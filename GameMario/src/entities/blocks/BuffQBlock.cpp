#include "entities/blocks/BuffQBlock.h"
#include "entities/items/Mushroom.h"
#include "entities/items/SuperLeaf.h"
#include "core/Game.h"
#include "world/Chunk.h"
#include "core/PlayScene.h"

CBuffQBlock::CBuffQBlock(int id, float x, float y, int z, int originalChunkId, CMushroom* mushroom, CSuperLeaf* superleaf) : CGameObject(id, x, y, z)
{
	this->originalChunkId = originalChunkId;
	this->mushroom = mushroom;
	this->superleaf = superleaf;
	SetState(QUESTIONBLOCK_STATE_NOT_HIT);
}

void CBuffQBlock::Render()
{
	int aniId = ID_ANI_QUESTIONBLOCK;
	if (isHit || state != QUESTIONBLOCK_STATE_NOT_HIT)
	{
		aniId = ID_ANI_QUESTIONBLOCK_HIT;
	}

	CAnimations::GetInstance()->Get(aniId)->Render(x, y);

	//RenderBoundingBox();
}

void CBuffQBlock::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
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

void CBuffQBlock::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x - QUESTIONBLOCK_BBOX_WIDTH / 2;
	t = y - QUESTIONBLOCK_BBOX_HEIGHT / 2;
	r = l + QUESTIONBLOCK_BBOX_WIDTH;
	b = t + QUESTIONBLOCK_BBOX_HEIGHT;
}

void CBuffQBlock::SetState(int state)
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
		if (toSpawn == 1)
		{
			superleaf->SetVisible(1);
			superleaf->SetState(SUPERLEAF_STATE_BOUNCE_UP);
		}
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
		if (toSpawn == 0)
		{
			mushroom->SetVisible(1);
			mushroom->SetState(MUSHROOM_STATE_RISE);
		}
		isHit = true;
		break;
	}
	CGameObject::SetState(state);
}

