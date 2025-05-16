#pragma once

#include "GameObject.h"
#include "Animation.h"
#include "Animations.h"
#include "Particle.h"

#define ID_ANI_TAIL_WHIP			20000

#define TAIL_WHIP_BBOX_WIDTH		20
#define TAIL_WHIP_BBOX_HEIGHT		8

#define TAIL_STATE_WHIPPING_LEFT	100
#define TAIL_STATE_WHIPPING_RIGHT	200
#define TAIL_STATE_NOT_WHIPPING		300

#define TAIL_WHIP_LEFT_TIME			200
#define TAIL_WHIP_RIGHT_TIME		200

class CTailWhip : public CGameObject
{
protected:
	int whippingLeft = 0;
	int whippingRight = 0;
	int notWhipping = 1;
	ULONGLONG whipLeftStart = -1;
	ULONGLONG whipRightStart = -1;
	int whipSpin = 0;

	void OnCollisionWithGoomba(LPCOLLISIONEVENT e);
	void OnCollisionWithWingedGoomba(LPCOLLISIONEVENT e);
	void OnCollisionWithKoopa(LPCOLLISIONEVENT e);
	void OnCollisionWithBuffQBlock(LPCOLLISIONEVENT e);
	void OnCollisionWithCoinQBlock(LPCOLLISIONEVENT e);
	void OnCollisionWithLifeBrick(LPCOLLISIONEVENT e);
	void OnCollisionWithPiranhaPlant(LPCOLLISIONEVENT e);
	void OnCollisionWithCoinBrick(LPCOLLISIONEVENT e);
	void OnCollisionWithActivatorBrick(LPCOLLISIONEVENT e);

public:
	CTailWhip(int id, float x, float y, int z);
	virtual ~CTailWhip();
	void Render();
	void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	void GetBoundingBox(float& l, float& t, float& r, float& b);
	int IsCollidable() { return (state != TAIL_STATE_NOT_WHIPPING); };
	int IsBlocking() { return 0; }
	void SetState(int state);
	void SetNx(int nx) { this->nx = nx; }
	//void StartWhippingLeft() { whippingLeft = 1; whipLeftStart = GetTickCount64(); }
	//void StartWhippingRight() { whippingRight = 1; whipRightStart = GetTickCount64(); }
	void OnNoCollision(DWORD dt) {};
	void OnCollisionWith(LPCOLLISIONEVENT e);
	bool IsActive() const { return notWhipping == 0; }
	void UpdatePosition(float marioX, float marioY, int marioNx);
};