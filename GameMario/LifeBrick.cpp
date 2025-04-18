#include "LifeBrick.h"
#include "Mushroom.h"
#include "SuperLeaf.h"
#include "Game.h"

CLifeBrick::CLifeBrick(float x, float y, CLifeMushroom* mushroom) : CQuestionBlock(x, y)
{
	this->mushroom = mushroom;
	SetState(QUESTIONBLOCK_STATE_NOT_HIT);
}

void CLifeBrick::Render()
{
	int aniId = ID_SPRITE_BRICK;
	if (isHit)
	{
		aniId = ID_ANI_QUESTIONBLOCK_HIT;
		CAnimations::GetInstance()->Get(aniId)->Render(x, y);
		return;
	}
	CSprites::GetInstance()->Get(aniId)->Draw(x, y);


	//RenderBoundingBox();
}

void CLifeBrick::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
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

void CLifeBrick::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x - QUESTIONBLOCK_BBOX_WIDTH / 2;
	t = y - QUESTIONBLOCK_BBOX_HEIGHT / 2;
	r = l + QUESTIONBLOCK_BBOX_WIDTH;
	b = t + QUESTIONBLOCK_BBOX_HEIGHT;
}

void CLifeBrick::SetState(int state)
{
	CGameObject::SetState(state);
	switch (state)
	{
	case QUESTIONBLOCK_STATE_NOT_HIT:
		isHit = false;
		break;
	case QUESTIONBLOCK_STATE_BOUNCE_UP:
		isHit = true;
		y0 = y;
		vy = QUESTIONBLOCK_BOUNCE_SPEED;
		StartBounceUp();
		break;
	case QUESTIONBLOCK_STATE_BOUNCE_DOWN:
		y0 = y;
		vy = -QUESTIONBLOCK_BOUNCE_SPEED;
		StartBounceDown();
		break;
	case QUESTIONBLOCK_STATE_BOUNCE_COMPLETE:
		vy = 0.0f;
		mushroom->SetVisible(1);
		mushroom->SetState(MUSHROOM_STATE_RISE);
		break;
	}
}

