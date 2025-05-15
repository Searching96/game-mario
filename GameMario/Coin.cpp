#include "Coin.h"
#include "Game.h"
#include "PlayScene.h"
#include "QuestionBlock.h"
#include "Particle.h"

CCoin::CCoin(int id, float x, float y, int z, int originalChunkId, int type) : CGameObject(id, x, y, z)
{
	this->type = type;
	this->originalChunkId = originalChunkId;
	if (type == 0) this->SetState(COIN_STATE_STATIC);
	else this->SetState(COIN_STATE_DYNAMIC);
}

void CCoin::Render()
{
	if (isVisible == 0)
		return;
	int aniId;
	if (type == 1)
		aniId = ID_ANI_COIN_DYNAMIC;
	else aniId = ID_ANI_COIN_STATIC;
	CAnimations* animations = CAnimations::GetInstance();
	animations->Get(aniId)->Render(x, y);

	//RenderBoundingBox();
}

void CCoin::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	if (bounceUp == 1)
	{
		if (fabs(y - y0) >= COIN_BOUNCE_UP_OFFSET)
		{
			y = y0 - COIN_BOUNCE_UP_OFFSET;
			bounceUp = 0;
			SetState(COIN_STATE_BOUNCE_DOWN);
		}
	}
	if (bounceDown == 1)
	{
		if (fabs(y - y0) >= COIN_BOUNCE_DOWN_OFFSET)
		{
			y = y0 + COIN_BOUNCE_DOWN_OFFSET;
			bounceDown = 0;
			SetState(COIN_STATE_BOUNCE_COMPLETE);
		}
	}
	x += vx * dt;
	y += vy * dt;
}

void CCoin::Activate()
{
	if (type == 0)
	{
		this->SetState(COIN_STATE_BOUNCE_COMPLETE);
		LPCHUNK chunk = ((LPPLAYSCENE)CGame::GetInstance()->GetCurrentScene())->GetChunk(originalChunkId);
		chunk->SetIsObjectConsumed(this->GetId(), true);
	}
	else this->SetState(COIN_STATE_BOUNCE_UP);
}

void CCoin::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x - COIN_BBOX_WIDTH / 2;
	t = y - COIN_BBOX_HEIGHT / 2;
	r = l + COIN_BBOX_WIDTH;
	b = t + COIN_BBOX_HEIGHT;
}

void CCoin::SetState(int state)
{
	switch (state)
	{
	case COIN_STATE_STATIC:
		isVisible = 1;
		break;
	case COIN_STATE_DYNAMIC:
		isVisible = 0;
		break;
	case COIN_STATE_BOUNCE_UP:
		y0 = y;
		vx = 0.0f;
		vy = COIN_BOUNCE_SPEED;
		isVisible = 1;
		StartBounceUp();
		break;
	case COIN_STATE_BOUNCE_DOWN:
		y0 = y;
		vx = 0.0f;
		vy = -COIN_BOUNCE_SPEED;
		StartBounceDown();
		break;
	case COIN_STATE_BOUNCE_COMPLETE:
		CGame::GetInstance()->GetGameState()->AddScore(100);
		CParticle::GenerateParticleInChunk(this, 0, 100);
		CGame::GetInstance()->GetGameState()->AddCoin();
		this->Delete();
		break;
	}
	CGameObject::SetState(state);
}
