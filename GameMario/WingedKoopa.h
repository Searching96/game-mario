#pragma once  
#include "GameObject.h"  

#define WINGED_KOOPA_GRAVITY 0.002f  
#define WINGED_KOOPA_WALKING_SPEED 0.02f  
#define WINGED_KOOPA_SHELL_SPEED 0.25f  
#define WINGED_KOOPA_SHELL_DEFLECT_SPEED 0.25f  
#define WINGED_KOOPA_BOUNCING_SPEED_Y -0.4f
#define WINGED_KOOPA_BOUNCING_SPEED_X 0.08f

#define WINGED_KOOPA_TEXTURE_WIDTH 16  
#define WINGED_KOOPA_TEXTURE_HEIGHT 26  

#define WINGED_KOOPA_BBOX_WIDTH 14  
#define WINGED_KOOPA_BBOX_HEIGHT 14  

#define WINGED_KOOPA_SHELL_TIMEOUT 7000  
#define WINGED_KOOPA_SHELL_ALERT_TIMEOUT 5000  
#define WINGED_KOOPA_DIE_TIMEOUT 700  
#define WINGED_KOOPA_TURNAROUND_TIMEOUT 200  

#define WINGED_KOOPA_STATE_MOVING_LEFT 100  
#define WINGED_KOOPA_STATE_MOVING_RIGHT 110  
#define WINGED_KOOPA_STATE_SHELL_STATIC 200  
#define WINGED_KOOPA_STATE_SHELL_DYNAMIC 300  
#define WINGED_KOOPA_STATE_BEING_HELD 400  
#define WINGED_KOOPA_STATE_DIE_ON_COLLIDE_WITH_ENEMY 500  
#define WINGED_KOOPA_STATE_DIE_ON_COLLIDE_WITH_TERRAIN 510  

#define ID_ANI_WINGED_KOOPA_MOVING_LEFT 180010  
#define ID_ANI_WINGED_KOOPA_MOVING_RIGHT 180020  
#define ID_ANI_WINGED_KOOPA_SHELL_STATIC_1 180100  
#define ID_ANI_WINGED_KOOPA_SHELL_STATIC_2 180101  
#define ID_ANI_WINGED_KOOPA_SHELL_DYNAMIC 180200  
#define ID_ANI_WINGED_KOOPA_SHELL_STATIC_REVERSE_1 180300  
#define ID_ANI_WINGED_KOOPA_SHELL_STATIC_REVERSE_2 180301  
#define ID_ANI_WINGED_KOOPA_SHELL_DYNAMIC_REVERSE 180400  

class CWingedKoopa : public CGameObject  
{  
protected:  
	float ax;  
	float ay;  

	float x0;  
	float y0;  

	bool isReversed = false;  
	bool isKicked = false;  
	bool isFlying = false; 
	bool isWinged;

	ULONGLONG shellStart = -1;  
	ULONGLONG dieStart = -1;  
	ULONGLONG lastTurnAroundTime = -1;  
	ULONGLONG flapStart = -1;

	bool isHeld = 0;  
	bool isDead = 0;  
	bool isDefeated = 0;  

	int originalChunkId;
	int wingState = 0; // 0: flap, 1: fold

	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);  
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);  
	virtual void Render();  

	virtual int IsCollidable() { return (isDead == 0 && isDefeated == 0); };  
	virtual int IsBlocking() { return 0; }  
	virtual void OnNoCollision(DWORD dt);  

	virtual void OnCollisionWith(LPCOLLISIONEVENT e);  
	void OnCollisionWithQuestionBlock(LPCOLLISIONEVENT e);  
	void OnCollisionWithGoomba(LPCOLLISIONEVENT e);  
	void OnCollisionWithWingedGoomba(LPCOLLISIONEVENT e);  
	void OnCollisionWithPiranhaPlant(LPCOLLISIONEVENT e);  
	void OnCollisionWithLifeBrick(LPCOLLISIONEVENT e);  
	void OnCollisionWithCoinBrick(LPCOLLISIONEVENT e);  
	void OnCollisionWithActivatorBrick(LPCOLLISIONEVENT e);
	void OnCollisionWithWingedKoopa(LPCOLLISIONEVENT e);

	bool CheckCollisionWithTerrain(DWORD dt, vector<LPGAMEOBJECT>* coObjects);

public:  
	CWingedKoopa(int id, float x, float y, int z, int originalChunkId, int initialNx, bool isWinged);  
	virtual void SetState(int state);  
	void StartShell();  
	bool IsHeld() { return isHeld; }  
	void SetIsHeld(int beingHeld) { this->isHeld = beingHeld; }  
	void SetIsFlying(bool isFlying) { this->isFlying = isFlying; }  
	void SetIsReversed(bool isReversed) { this->isReversed = isReversed; }  
	void SetNx(int nx) { this->nx = nx; }  
	void GetOriginalPosition(float& x0, float& y0) { x0 = this->x0; y0 = this->y0; }  
	bool IsFlying() { return isFlying; }  
	bool IsDefeated() { return isDefeated; }  
	bool IsDead() { return isDead; }
	bool IsWinged() { return isWinged; }
	void SetIsDefeated(bool isDefeated) { this->isDefeated = isDefeated; }  
	void SetIsWinged(bool isWinged) { this->isWinged = isWinged; }
	int GetOriginalChunkId() { return originalChunkId; }
	int GetNx() { return nx; }
};
