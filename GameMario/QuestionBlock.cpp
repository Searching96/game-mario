#include "QuestionBlock.h"
#include "QuestionBlock.h"
#include "Game.h"

CQuestionBlock::CQuestionBlock(float x, float y) : CGameObject(x, y)
{
	this->x = x;
	this->y = y;
	SetState(QUESTIONBLOCK_STATE_NOT_HIT);
}

void CQuestionBlock::Render()
{
	int aniId = ID_ANI_QUESTIONBLOCK;
	if (isHit)
	{
		aniId = ID_ANI_QUESTIONBLOCK_HIT;
	}

	CAnimations::GetInstance()->Get(aniId)->Render(x, y);
	RenderBoundingBox();
}

void CQuestionBlock::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	if (bounceUp == 1)
	{
		if (GetTickCount64() - bounceUpStart > QUESTIONBLOCK_BOUNCE_UP_TIME)
		{
			bounceUp = 0;
			SetState(QUESTIONBLOCK_STATE_BOUNCE_DOWN);
		}
	}
	else if (bounceDown == 1)
	{
		if (GetTickCount64() - bounceDownStart > QUESTIONBLOCK_BOUNCE_DOWN_TIME)
		{
			bounceDown = 0;
			SetState(QUESTIONBLOCK_STATE_BOUNCE_COMPLETE);
		}
	}
	x += vx * dt;
	y += vy * dt;
	CGameObject::Update(dt, coObjects);
	CCollision::GetInstance()->Process(this, dt, coObjects);
}

void CQuestionBlock::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x - QUESTIONBLOCK_BBOX_WIDTH / 2;
	t = y - QUESTIONBLOCK_BBOX_HEIGHT / 2;
	r = l + QUESTIONBLOCK_BBOX_WIDTH;
	b = t + QUESTIONBLOCK_BBOX_HEIGHT;
}

void CQuestionBlock::SetState(int state)
{
	CGameObject::SetState(state);
	switch (state)
	{
	case QUESTIONBLOCK_STATE_NOT_HIT:
		isHit = false;
		break;
	case QUESTIONBLOCK_STATE_BOUNCE_UP:
		isHit = true;
		vy = QUESTIONBLOCK_BOUNCE_SPEED;
		StartBounceUp();
		break;
	case QUESTIONBLOCK_STATE_BOUNCE_DOWN:
		vy = -QUESTIONBLOCK_BOUNCE_SPEED;
		StartBounceDown();
		break;
	case QUESTIONBLOCK_STATE_BOUNCE_COMPLETE:
		vy = 0.0f;
		break;
	}
}

