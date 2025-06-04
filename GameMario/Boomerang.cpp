#include "Boomerang.h"
#include "Mario.h"
#include "PlayScene.h"

CBoomerang::CBoomerang(int id, float x, float y, int z, int originalChunkId) : CGameObject(id, x, y, z)
{
	this->originalChunkId = originalChunkId;
	this->ax = 0;
	this->ay = 0;
	x0 = x;
	y0 = y;
	SetState(BOOMERANG_STATE_SWINGING_UP);
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
	// Debug out vx, vy, ax, ay, state
	DebugOutTitle(L"[BOOMERANG] Update: vx = %f, vy = %f, ax = %f, ay = %f, state = %d\n", vx, vy, ax, ay, state);

	if (isSwingingUp)
		ay += BOOMERANG_ACCEL_DECREMENT_Y;
	else if (isHoming)
		ay = 0;
	
	if (isSwingingDown)
		ax -= BOOMERANG_ACCEL_DECREMENT_X;
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
	if ((isSwingingDown) && (y >= y00))
	{
		y = y00;
		isSwingingDown = false;
		SetState(BOOMERANG_STATE_HOMING);
	}
	if ((isHoming) && (x <= x00))
	{
		x = x00;
		isHoming = false;
		SetState(BOOMERANG_STATE_SWINGING_UP);
	}

	CGameObject::Update(dt, coObjects);
	CCollision::GetInstance()->Process(this, dt, coObjects);
}

void CBoomerang::Render()
{
	int aniId = ID_ANI_BOOMERANG_SWINGING;
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
		break;
	case BOOMERANG_STATE_SWINGING_UP:
		vx = BOOMERANG_SPEED_X;
		vy = -0.09f;
		ax = 0;
		ay = BOOMERANG_GRAVITY;
		StartSwingingUp();
		break;
	case BOOMERANG_STATE_SWINGING_DOWN:
		vx = BOOMERANG_SPEED_X;
		ax = 0;
		ay = BOOMERANG_GRAVITY * 2;
		StartSwingingDown();
		break;
	case BOOMERANG_STATE_HOMING:
		vx = -BOOMERANG_SPEED_X;
		vy = 0;
		ax = 0;
		ay = 0;
		StartHoming();
		break;
	}
}
