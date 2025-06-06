#pragma once
#include "GameObject.h"

#define BOOMERANG_GRAVITY								0.0001f
#define BOOMERANG_SPEED_X								0.1f

#define BOOMERANG_ACCEL_DECREMENT_Y						0.000001f
#define BOOMERANG_ACCEL_DECREMENT_X						0.000022f

#define BOOMERANG_BBOX_WIDTH							16
#define BOOMERANG_BBOX_HEIGHT							16

#define BOOMERANG_DIE_TIMEOUT							700

#define BOOMERANG_STATE_IDLING							0
#define BOOMERANG_STATE_SWINGING_UP						100
#define BOOMERANG_STATE_SWINGING_DOWN					200
#define BOOMERANG_STATE_HOMING							300

#define BOOMERANG_SWING_UP_OFFSET						48.0f

#define ID_ANI_BOOMERANG_SWINGING_RIGHT					200000
#define ID_ANI_BOOMERANG_SWINGING_LEFT					200010
#define ID_ANI_BOOMERANG_IDLING_RIGHT					201000
#define ID_ANI_BOOMERANG_IDLING_LEFT					201010

#define BOOMERANG_MAX_HEIGHT_DIFF_FROM_START			10.0f


class CBoomerang : public CGameObject
{
protected:
	float ax;
	float ay;

	float x0;
	float y0;

	float y00 = -1.0f;
	float x00 = -1.0f;

	ULONGLONG dieStart = -1;
	ULONGLONG dieOnStompStart = -1;
	ULONGLONG dieOnWhipStart = -1;

	bool isSwingingUp = false;
	bool isSwingingDown = false;
	bool isHoming = false;
	bool isIdling = false;
	bool isVisible = false;

	int originalChunkId;

	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();

	virtual int IsCollidable() { return 1; }
	virtual int IsBlocking() { return 0; }

	virtual void OnNoCollision(DWORD dt);
	virtual void OnCollisionWith(LPCOLLISIONEVENT e);

	void StartSwingingUp() { x00 = x; y00 = y; isSwingingUp = true; isIdling = false; }
	void StartSwingingDown() { isSwingingDown = true; }
	void StartHoming() { isHoming = true; }

public:
	CBoomerang(int id, float x, float y, int z, int originalChunkId);
	virtual void SetState(int state);
	//int IsDead() { return isDead; }
	//int IsDefeated() { return isDefeated; }
	//void SetIsDefeated(int defeated) { isDefeated = defeated; }
	void GetOriginalPosition(float& x0, float& y0) { x0 = this->x0; y0 = this->y0; }
	int GetOriginalChunkId() { return originalChunkId; }
	bool IsIdling() { return isIdling; }
	bool IsVisible() { return isVisible; }
	void SetIsVisible(bool isVisible) { this->isVisible = isVisible; }
	void SetNx(int nx) { this->nx = nx; }
	int GetNx() { return nx; }
};