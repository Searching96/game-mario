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
	if (state == QUESTIONBLOCK_STATE_HIT)
	{
		aniId = ID_ANI_QUESTIONBLOCK_HIT;
	}

	CAnimations::GetInstance()->Get(aniId)->Render(x, y);
	RenderBoundingBox();
}

void CQuestionBlock::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
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
}

//void CQuestionBlock::OnCollisionWith(LPCOLLISIONEVENT e)
//{
//	if (e->ny > 0 && state == QUESTION_BLOCK_STATE_NOT_HIT)
//	{
//		SetState(QUESTION_BLOCK_STATE_HIT);
//
//		CGame* game = CGame::GetInstance();
//		LPGAMEOBJECT star = new CStar(x, y - STAR_BBOX_HEIGHT / 2);
//		objects.push_back(star);
//	}
//}
