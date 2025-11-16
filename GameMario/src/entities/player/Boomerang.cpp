#include "entities/player/Boomerang.h"
#include "entities/player/Mario.h"
#include "core/PlayScene.h"

CBoomerang::CBoomerang(int id, float x, float y, int z, int originalChunkId) : CGameObject(id, x, y, z)
{
	this->originalChunkId = originalChunkId;
	this->ax = 0;
	this->ay = 0;
	x0 = x;
	y0 = y;
	SetState(BOOMERANG_STATE_IDLING);
}

void CBoomerang::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	left = x - BOOMERANG_BBOX_WIDTH / 2;
	top = y - BOOMERANG_BBOX_HEIGHT / 2;
	right = left + BOOMERANG_BBOX_WIDTH;
	bottom = top + BOOMERANG_BBOX_HEIGHT;
}

void CBoomerang::OnNoCollision(DWORD dt)
{
	//x += vx * dt;
	//y += vy * dt;
};

void CBoomerang::OnCollisionWith(LPCOLLISIONEVENT e)
{

}

void CBoomerang::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	//DebugOutTitle(L"[BOOMERANG] Update: vx = %f, vy = %f, ax = %f, ay = %f, state = %d\n", vx, vy, ax, ay, state);
	if (isSwingingUp)
		ay = ay;
	else if (isSwingingDown)
		ay -= BOOMERANG_ACCEL_DECREMENT_Y;

	
	if (isSwingingDown && y >= y00 - 32)
		if (nx < 0)
			ax -= BOOMERANG_ACCEL_DECREMENT_X;
		else if (nx > 0)
			ax += BOOMERANG_ACCEL_DECREMENT_X;
	else
		ax = 0;

	vy += ay * dt;
	vx += ax * dt;

	x += vx * dt;
	y += vy * dt;

	if ((isSwingingUp) && (vy > 0))
	{
		isSwingingUp = false;
		SetState(BOOMERANG_STATE_SWINGING_DOWN);
	}
	if ((isSwingingDown) && (y >= y00 + BOOMERANG_MAX_HEIGHT_DIFF_FROM_START))
	{
		y = y00 + BOOMERANG_MAX_HEIGHT_DIFF_FROM_START;
		isSwingingDown = false;
		SetState(BOOMERANG_STATE_HOMING);
	}

	CGameObject::Update(dt, coObjects);
	CCollision::GetInstance()->Process(this, dt, coObjects);
}

void CBoomerang::Render()
{
	if (!isVisible) return;

	int aniId = ID_ANI_BOOMERANG_SWINGING_RIGHT;
	aniId = (nx > 0) ? ID_ANI_BOOMERANG_SWINGING_RIGHT : ID_ANI_BOOMERANG_SWINGING_LEFT;
	if (isIdling)
		aniId = (nx > 0) ? ID_ANI_BOOMERANG_IDLING_RIGHT : ID_ANI_BOOMERANG_IDLING_LEFT;
	CAnimations::GetInstance()->Get(aniId)->Render(x, y);

	//RenderBoundingBox();
}

void CBoomerang::SetState(int state)
{
	CGameObject::SetState(state);
	switch (state)
	{
	case BOOMERANG_STATE_IDLING:
		vx = 0;
		vy = 0;
		ax = 0;
		ay = 0;
		isIdling = true;
		break;
	case BOOMERANG_STATE_SWINGING_UP:
		vx = (nx < 0) ? BOOMERANG_SPEED_X : -BOOMERANG_SPEED_X;
		vy = -0.07f;
		ax = 0;
		ay = BOOMERANG_GRAVITY;
		StartSwingingUp();
		break;
	case BOOMERANG_STATE_SWINGING_DOWN:
		vx = (nx < 0) ? BOOMERANG_SPEED_X : -BOOMERANG_SPEED_X;
		ax = 0;
		ay = BOOMERANG_GRAVITY * 2;
		StartSwingingDown();
		break;
	case BOOMERANG_STATE_HOMING:
		vx = (nx < 0) ? -BOOMERANG_SPEED_X : BOOMERANG_SPEED_X;
		vy = 0;
		ax = 0;
		ay = 0;
		StartHoming();
		break;
	}
}
