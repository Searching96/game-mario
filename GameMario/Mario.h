#pragma once
#include "GameObject.h"

#include "Animation.h"
#include "Animations.h"

#include "debug.h"

#define MARIO_WALKING_SPEED		0.1f
#define MARIO_RUNNING_SPEED		0.2f

#define MARIO_ACCEL_WALK_X	0.0005f
#define MARIO_ACCEL_RUN_X	0.0007f

#define MARIO_JUMP_SPEED_Y		0.5f
#define MARIO_JUMP_RUN_SPEED_Y	0.6f

#define MARIO_GRAVITY			0.002f

#define MARIO_JUMP_DEFLECT_SPEED  0.4f

#define MARIO_STATE_DIE				-10
#define MARIO_STATE_IDLE			0
#define MARIO_STATE_WALKING_RIGHT	100
#define MARIO_STATE_WALKING_LEFT	200

#define MARIO_STATE_JUMP			300
#define MARIO_STATE_RELEASE_JUMP    301

#define MARIO_STATE_RUNNING_RIGHT	400
#define MARIO_STATE_RUNNING_LEFT	500

#define MARIO_STATE_SIT				600
#define MARIO_STATE_SIT_RELEASE		601

#define MARIO_STATE_POWER_UP		700


#pragma region ANIMATION_ID
// BIG MARIO
#define ID_ANI_MARIO_IDLE_RIGHT 0
#define ID_ANI_MARIO_IDLE_LEFT 10

#define ID_ANI_MARIO_WALKING_RIGHT 100
#define ID_ANI_MARIO_WALKING_LEFT 110

#define ID_ANI_MARIO_RUNNING_RIGHT 200
#define ID_ANI_MARIO_RUNNING_LEFT 210

#define ID_ANI_MARIO_JUMP_WALK_RIGHT 300
#define ID_ANI_MARIO_JUMP_WALK_LEFT 310

#define ID_ANI_MARIO_JUMP_RUN_RIGHT 400
#define ID_ANI_MARIO_JUMP_RUN_LEFT 410

#define ID_ANI_MARIO_SIT_RIGHT 500
#define ID_ANI_MARIO_SIT_LEFT 510

#define ID_ANI_MARIO_BRACE_RIGHT 600
#define ID_ANI_MARIO_BRACE_LEFT 610

#define ID_ANI_MARIO_DIE 4000

// SMALL MARIO
#define ID_ANI_MARIO_SMALL_IDLE_RIGHT 2000
#define ID_ANI_MARIO_SMALL_IDLE_LEFT 2010

#define ID_ANI_MARIO_SMALL_WALKING_RIGHT 2100
#define ID_ANI_MARIO_SMALL_WALKING_LEFT 2110

#define ID_ANI_MARIO_SMALL_RUNNING_RIGHT 2200
#define ID_ANI_MARIO_SMALL_RUNNING_LEFT 2210

#define ID_ANI_MARIO_SMALL_BRACE_RIGHT 2300
#define ID_ANI_MARIO_SMALL_BRACE_LEFT 2310

#define ID_ANI_MARIO_SMALL_JUMP_WALK_RIGHT 2400
#define ID_ANI_MARIO_SMALL_JUMP_WALK_LEFT 2410

#define ID_ANI_MARIO_SMALL_JUMP_RUN_RIGHT 2500
#define ID_ANI_MARIO_SMALL_JUMP_RUN_LEFT 2510

#define ID_ANI_MARIO_SMALL_POWER_UP_RIGHT 2600
#define ID_ANI_MARIO_SMALL_POWER_UP_LEFT 2610

// TAIL MARIO
#define ID_ANI_MARIO_TAIL_IDLE_RIGHT 6000
#define ID_ANI_MARIO_TAIL_IDLE_LEFT 6010

#define ID_ANI_MARIO_TAIL_WALKING_RIGHT 6100
#define ID_ANI_MARIO_TAIL_WALKING_LEFT 6110

#define ID_ANI_MARIO_TAIL_RUNNING_RIGHT 6200
#define ID_ANI_MARIO_TAIL_RUNNING_LEFT 6210

#define ID_ANI_MARIO_TAIL_JUMP_WALK_RIGHT 6300
#define ID_ANI_MARIO_TAIL_JUMP_WALK_LEFT 6310

#define ID_ANI_MARIO_TAIL_JUMP_RUN_RIGHT 6400
#define ID_ANI_MARIO_TAIL_JUMP_RUN_LEFT 6410

#define ID_ANI_MARIO_TAIL_SIT_RIGHT 6500
#define ID_ANI_MARIO_TAIL_SIT_LEFT 6510

#define ID_ANI_MARIO_TAIL_BRACE_RIGHT 6600
#define ID_ANI_MARIO_TAIL_BRACE_LEFT 6610


#pragma endregion

#define GROUND_Y 160.0f




#define	MARIO_LEVEL_SMALL	1
#define	MARIO_LEVEL_BIG		2
#define MARIO_LEVEL_TAIL	3

#define MARIO_BIG_BBOX_WIDTH  14
#define MARIO_BIG_BBOX_HEIGHT 24
#define MARIO_BIG_SITTING_BBOX_WIDTH  14
#define MARIO_BIG_SITTING_BBOX_HEIGHT 16

#define MARIO_SIT_HEIGHT_ADJUST ((MARIO_BIG_BBOX_HEIGHT-MARIO_BIG_SITTING_BBOX_HEIGHT)/2)

#define MARIO_SMALL_BBOX_WIDTH  13
#define MARIO_SMALL_BBOX_HEIGHT 12


#define MARIO_UNTOUCHABLE_TIME 2500
#define MARIO_POWER_UP_TIME 2000

class CMario : public CGameObject
{
	BOOLEAN isSitting;
	float maxVx;
	float ax;				// acceleration on x 
	float ay;				// acceleration on y 

	int level; 
	int untouchable; 
	ULONGLONG untouchable_start;
	BOOLEAN isOnPlatform;
	int coin; 

	int powerUp = 0;
	ULONGLONG powerUpStart = -1;

	void OnCollisionWithGoomba(LPCOLLISIONEVENT e);
	void OnCollisionWithCoin(LPCOLLISIONEVENT e);
	void OnCollisionWithPortal(LPCOLLISIONEVENT e);
	void OnCollisionWithQuestionBlock(LPCOLLISIONEVENT e);
	void OnCollisionWithMushroom(LPCOLLISIONEVENT e);
	void OnCollisionWithSuperLeaf(LPCOLLISIONEVENT e);

	int GetAniIdBig();
	int GetAniIdSmall();
	int GetAniIdTail();

public:
	CMario(float x, float y) : CGameObject(x, y)
	{
		isSitting = false;
		maxVx = 0.0f;
		ax = 0.0f;
		ay = MARIO_GRAVITY; 

		level = MARIO_LEVEL_SMALL;
		untouchable = 0;
		untouchable_start = -1;
		isOnPlatform = false;
		coin = 0;
	}
	void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	void Render();
	void SetState(int state);

	int IsCollidable()
	{ 
		return (state != MARIO_STATE_DIE); 
	}

	int IsBlocking() { return (state != MARIO_STATE_DIE && untouchable==0); }

	void OnNoCollision(DWORD dt);
	void OnCollisionWith(LPCOLLISIONEVENT e);

	void SetLevel(int l);
	void StartUntouchable() { untouchable = 1; untouchable_start = GetTickCount64(); }

	void GetBoundingBox(float& left, float& top, float& right, float& bottom);

	void StartPowerUp() { powerUp = 1; powerUpStart = GetTickCount64(); }
};