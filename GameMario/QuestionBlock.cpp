#include "QuestionBlock.h"
#include "QuestionBlock.h"
#include "Game.h"

CQuestionBlock::CQuestionBlock(float x, float y) : CGameObject(x, y)
{
	this->x = x;
	this->y = y;
	SetState(QUESTION_BLOCK_STATE_NOT_HIT);
}

void CQuestionBlock::Render()
{
	int aniId = ID_ANI_QUESTION_BLOCK;
	if (state == QUESTION_BLOCK_STATE_HIT)
	{
		aniId = ID_ANI_QUESTION_BLOCK_HIT;
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
	l = x - QUESTION_BLOCK_BBOX_WIDTH / 2;
	t = y - QUESTION_BLOCK_BBOX_HEIGHT / 2;
	r = l + QUESTION_BLOCK_BBOX_WIDTH;
	b = t + QUESTION_BLOCK_BBOX_HEIGHT;
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
