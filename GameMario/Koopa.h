#pragma once
#include "GameObject.h"
#include "Platform.h"
#include "Box.h"
#include "BuffQBlock.h"
#include "CoinQBlock.h"
#include "Mario.h"
#include "PlayScene.h"
#include "debug.h"

#define KOOPA_GRAVITY 0.002f
#define KOOPA_WALKING_SPEED 0.02f
#define KOOPA_SHELL_SPEED 0.25f

#define KOOPA_TEXTURE_WIDTH 16
#define KOOPA_TEXTURE_HEIGHT 26

#define KOOPA_BBOX_WIDTH 16
#define KOOPA_BBOX_HEIGHT 16

#define KOOPA_SHELL_TIMEOUT 7000
#define KOOPA_SHELL_ALERT_TIMEOUT 5000

#define KOOPA_STATE_WALKING_LEFT 100
#define KOOPA_STATE_WALKING_RIGHT 101
#define KOOPA_STATE_SHELL_STATIC 200
#define KOOPA_STATE_SHELL_DYNAMIC 300
#define KOOPA_STATE_BEING_HELD 400

#define ID_ANI_KOOPA_WALKING_LEFT 150010
#define ID_ANI_KOOPA_WALKING_RIGHT 150020
#define ID_ANI_KOOPA_SHELL_STATIC_1 150100
#define ID_ANI_KOOPA_SHELL_STATIC_2 150101
#define ID_ANI_KOOPA_SHELL_DYNAMIC 150200
#define ID_ANI_KOOPA_SHELL_STATIC_REVERSE_1 150300
#define ID_ANI_KOOPA_SHELL_STATIC_REVERSE_2 150301
#define ID_ANI_KOOPA_SHELL_DYNAMIC_REVERSE 150400

class CKoopa :
    public CGameObject
{
protected:
	float ax;
	float ay;

	bool isReversed = false;
	bool isKicked = false;

	LPGAMEOBJECT ground;

	ULONGLONG shell_start;

	int beingHeld = 0;

	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();
	bool IsPlatformEdge(float checkDistance);

	virtual int IsCollidable() { return 1; };
	virtual int IsBlocking() { return 0; }
	virtual void OnNoCollision(DWORD dt);

	virtual void OnCollisionWith(LPCOLLISIONEVENT e);

public:
	CKoopa(float x, float y);
	virtual void SetState(int state);
	void StartShell();
	int GetBeingHeld() { return beingHeld; }
	void SetBeingHeld(int beingHeld) { this->beingHeld = beingHeld; }
};

