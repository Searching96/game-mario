#pragma once
#include "GameObject.h"

#include "Animation.h"
#include "Animations.h"
#include "TailWhip.h"

#include "debug.h"

class CPortal;

#define MARIO_MAX_WALKING_SPEED			0.09f
#define MARIO_MAX_RUNNING_SPEED			0.185f
#define MARIO_MAX_FALLING_SPEED			0.25f
#define MARIO_MAX_JUMP_SPEED			-0.65f

#define MARIO_WALKING_SPEED				0.08f
#define MARIO_RUNNING_SPEED				0.165f
#define MARIO_INSTANT_BRAKING_SPEED		0.04f
#define MARIO_DESCEND_SPEED				0.01f

#define MARIO_ACCEL_RUN_X				0.00025f
#define MARIO_ACCEL_WALK_X				0.00020f
#define MARIO_DECELERATION_X			0.0003f
#define MARIO_FRICTION_X				0.0001f
#define MARIO_JUMP_GRAVITY				0.00005f

#define MARIO_JUMP_SPEED_Y				0.3f
#define MARIO_JUMP_RUN_SPEED_Y			0.35f
#define MARIO_HOVER_SPEED_Y				0.05f

#define MARIO_GRAVITY					0.00225f

#define MARIO_JUMP_DEFLECT_SPEED		0.3f
#define MARIO_DIE_BOUNCING_SPEED		0.5f

#define MARIO_STATE_DIE_ON_BEING_KILLED	-20
#define MARIO_STATE_DIE_ON_FALLING		-10
#define MARIO_STATE_IDLE				0
#define MARIO_STATE_WALKING_RIGHT		100
#define MARIO_STATE_WALKING_LEFT		200

#define MARIO_STATE_JUMP				300
#define MARIO_STATE_RELEASE_JUMP		301

#define MARIO_STATE_RUNNING_RIGHT		400
#define MARIO_STATE_RUNNING_LEFT		500

#define MARIO_STATE_SIT					600
#define MARIO_STATE_SIT_RELEASE			601

#define MARIO_STATE_POWER_UP			700
#define MARIO_STATE_POWER_DOWN			701

#define MARIO_STATE_TAIL_UP				800
#define MARIO_STATE_TAIL_DOWN			801

#define MARIO_STATE_HOVER				900
#define MARIO_STATE_BRAKE				1000

#define MARIO_STATE_RELEASE_MOVE		1100
#define MARIO_STATE_RELEASE_RUN			1101

#define MARIO_STATE_TAIL_WHIP			1200

#define MARIO_STATE_HOLDING_KOOPA		1300

#define MARIO_STATE_TELEPORTING			1400

#pragma region ANIMATION_ID
// BIG MARIO
#define ID_ANI_MARIO_IDLE_RIGHT 0
#define ID_ANI_MARIO_IDLE_LEFT 10

#define ID_ANI_MARIO_WALKING_RIGHT 100
#define ID_ANI_MARIO_WALKING_LEFT 110

#define ID_ANI_MARIO_MAX_RUNNING_RIGHT 200
#define ID_ANI_MARIO_MAX_RUNNING_LEFT 210

#define ID_ANI_MARIO_JUMP_WALK_RIGHT 300
#define ID_ANI_MARIO_JUMP_WALK_LEFT 310

#define ID_ANI_MARIO_JUMP_RUN_RIGHT 400
#define ID_ANI_MARIO_JUMP_RUN_LEFT 410

#define ID_ANI_MARIO_SIT_RIGHT 500
#define ID_ANI_MARIO_SIT_LEFT 510

#define ID_ANI_MARIO_BRACE_RIGHT 600
#define ID_ANI_MARIO_BRACE_LEFT 610

#define ID_ANI_MARIO_RUNNING_RIGHT 700
#define ID_ANI_MARIO_RUNNING_LEFT 710

#define ID_ANI_MARIO_POWER_DOWN_RIGHT 900
#define ID_ANI_MARIO_POWER_DOWN_LEFT 910

#define ID_ANI_MARIO_KICK_RIGHT 1000
#define ID_ANI_MARIO_KICK_LEFT 1010

#define ID_ANI_MARIO_HOLDING_KOOPA_IDLE_RIGHT 1100
#define ID_ANI_MARIO_HOLDING_KOOPA_IDLE_LEFT 1110

#define ID_ANI_MARIO_HOLDING_KOOPA_WALKING_RIGHT 1200
#define ID_ANI_MARIO_HOLDING_KOOPA_WALKING_LEFT 1210

#define ID_ANI_MARIO_HOLDING_KOOPA_RUNNING_RIGHT 1300
#define ID_ANI_MARIO_HOLDING_KOOPA_RUNNING_LEFT 1310

#define ID_ANI_MARIO_HOLDING_KOOPA_MAX_RUNNING_RIGHT 1400
#define ID_ANI_MARIO_HOLDING_KOOPA_MAX_RUNNING_LEFT 1410

#define ID_ANI_MARIO_FALLING_RIGHT 1500
#define ID_ANI_MARIO_FALLING_LEFT 1510

#define ID_ANI_MARIO_DIE 4000

// SMALL MARIO
#define ID_ANI_MARIO_SMALL_IDLE_RIGHT 2000
#define ID_ANI_MARIO_SMALL_IDLE_LEFT 2010

#define ID_ANI_MARIO_SMALL_WALKING_RIGHT 2100
#define ID_ANI_MARIO_SMALL_WALKING_LEFT 2110

#define ID_ANI_MARIO_SMALL_MAX_RUNNING_RIGHT 2200
#define ID_ANI_MARIO_SMALL_MAX_RUNNING_LEFT 2210

#define ID_ANI_MARIO_SMALL_BRACE_RIGHT 2300
#define ID_ANI_MARIO_SMALL_BRACE_LEFT 2310

#define ID_ANI_MARIO_SMALL_JUMP_WALK_RIGHT 2400
#define ID_ANI_MARIO_SMALL_JUMP_WALK_LEFT 2410

#define ID_ANI_MARIO_SMALL_JUMP_RUN_RIGHT 2500
#define ID_ANI_MARIO_SMALL_JUMP_RUN_LEFT 2510

#define ID_ANI_MARIO_SMALL_POWER_UP_RIGHT 2600
#define ID_ANI_MARIO_SMALL_POWER_UP_LEFT 2610

#define ID_ANI_MARIO_SMALL_RUNNING_RIGHT 2700
#define ID_ANI_MARIO_SMALL_RUNNING_LEFT 2710

#define ID_ANI_MARIO_SMALL_KICK_RIGHT 2800
#define ID_ANI_MARIO_SMALL_KICK_LEFT 2810

#define ID_ANI_MARIO_SMALL_HOLDING_KOOPA_IDLE_RIGHT 2900
#define ID_ANI_MARIO_SMALL_HOLDING_KOOPA_IDLE_LEFT 2910

#define ID_ANI_MARIO_SMALL_HOLDING_KOOPA_WALKING_RIGHT 3000
#define ID_ANI_MARIO_SMALL_HOLDING_KOOPA_WALKING_LEFT 3010

#define ID_ANI_MARIO_SMALL_HOLDING_KOOPA_RUNNING_RIGHT 3100
#define ID_ANI_MARIO_SMALL_HOLDING_KOOPA_RUNNING_LEFT 3110

#define ID_ANI_MARIO_SMALL_HOLDING_KOOPA_MAX_RUNNING_RIGHT 3200
#define ID_ANI_MARIO_SMALL_HOLDING_KOOPA_MAX_RUNNING_LEFT 3210

// TAIL MARIO
#define ID_ANI_MARIO_TAIL_IDLE_RIGHT 6000
#define ID_ANI_MARIO_TAIL_IDLE_LEFT 6010

#define ID_ANI_MARIO_TAIL_WALKING_RIGHT 6100
#define ID_ANI_MARIO_TAIL_WALKING_LEFT 6110

#define ID_ANI_MARIO_TAIL_MAX_RUNNING_RIGHT 6200
#define ID_ANI_MARIO_TAIL_MAX_RUNNING_LEFT 6210

#define ID_ANI_MARIO_TAIL_JUMP_WALK_RIGHT 6300
#define ID_ANI_MARIO_TAIL_JUMP_WALK_LEFT 6310

#define ID_ANI_MARIO_TAIL_JUMP_RUN_RIGHT 6400
#define ID_ANI_MARIO_TAIL_JUMP_RUN_LEFT 6410

#define ID_ANI_MARIO_TAIL_SIT_RIGHT 6500
#define ID_ANI_MARIO_TAIL_SIT_LEFT 6510

#define ID_ANI_MARIO_TAIL_BRACE_RIGHT 6600
#define ID_ANI_MARIO_TAIL_BRACE_LEFT 6610

#define ID_ANI_MARIO_TAIL_MULTIJUMP_RIGHT 6700
#define ID_ANI_MARIO_TAIL_MULTIJUMP_LEFT 6710

#define ID_ANI_MARIO_TAIL_RUNNING_RIGHT 6800
#define ID_ANI_MARIO_TAIL_RUNNING_LEFT 6810

#define ID_ANI_MARIO_TAIL_HOVERING_RIGHT 6900
#define ID_ANI_MARIO_TAIL_HOVERING_LEFT 6910

#define ID_ANI_MARIO_TAIL_WHIP_RIGHT 7000
#define ID_ANI_MARIO_TAIL_WHIP_LEFT 7010

#define ID_ANI_MARIO_TAIL_KICK_RIGHT 7100
#define ID_ANI_MARIO_TAIL_KICK_LEFT 7110

#define ID_ANI_MARIO_TAIL_HOLDING_KOOPA_IDLE_RIGHT 7200
#define ID_ANI_MARIO_TAIL_HOLDING_KOOPA_IDLE_LEFT 7210

#define ID_ANI_MARIO_TAIL_HOLDING_KOOPA_WALKING_RIGHT 7300
#define ID_ANI_MARIO_TAIL_HOLDING_KOOPA_WALKING_LEFT 7310

#define ID_ANI_MARIO_TAIL_HOLDING_KOOPA_RUNNING_RIGHT 7400
#define ID_ANI_MARIO_TAIL_HOLDING_KOOPA_RUNNING_LEFT 7410

#define ID_ANI_MARIO_TAIL_HOLDING_KOOPA_MAX_RUNNING_RIGHT 7500
#define ID_ANI_MARIO_TAIL_HOLDING_KOOPA_MAX_RUNNING_LEFT 7510

#define ID_ANI_MARIO_TAIL_FALLING_RIGHT 7600
#define ID_ANI_MARIO_TAIL_FALLING_LEFT 7610

#define ID_ANI_MARIO_TAIL_ENTERING_PIPE 7900

#define ID_ANI_MARIO_INVISIBLE 8001

#pragma endregion

#define GROUND_Y									160.0f

#define	MARIO_LEVEL_SMALL							1
#define	MARIO_LEVEL_BIG								2
#define MARIO_LEVEL_TAIL							3

#define MARIO_BIG_BBOX_WIDTH						14
#define MARIO_BIG_BBOX_HEIGHT						24
#define MARIO_BIG_SITTING_BBOX_WIDTH				14
#define MARIO_BIG_SITTING_BBOX_HEIGHT				14
#define MARIO_TAIL_BBOX_WIDTH						14
#define MARIO_TAIL_BBOX_HEIGHT						24
#define MARIO_TAIL_SITTING_BBOX_WIDTH				14
#define MARIO_TAIL_SITTING_BBOX_HEIGHT				14

#define MARIO_SIT_HEIGHT_ADJUST						((MARIO_BIG_BBOX_HEIGHT-MARIO_BIG_SITTING_BBOX_HEIGHT)/2)

#define MARIO_SMALL_BBOX_WIDTH						13
#define MARIO_SMALL_BBOX_HEIGHT						12

#define MARIO_UNTOUCHABLE_TIMEOUT					2500
#define MARIO_POWER_UP_TIMEOUT						1000
#define MARIO_POWER_DOWN_TIMEOUT					MARIO_POWER_UP_TIMEOUT
#define MARIO_TAIL_UP_TIMEOUT						400
#define MARIO_TAIL_DOWN_TIMEOUT						MARIO_TAIL_UP_TIMEOUT
#define MARIO_HOVER_TIMEOUT							500
#define MARIO_BRAKE_TIMEOUT							500
#define MARIO_KICK_TIMEOUT							150
#define MARIO_UNTOUCHABLE_RENDER_INTERVAL			75
#define MARIO_TAIL_WHIP_TIMEOUT						305
#define MARIO_PMETER_MAX_TIMEOUT					4000
#define MARIO_DEATH_ANI_TIMEOUT						1500

#define MAX_JUMP_COUNT 10000
#define SPEED_DIVISOR 3.0f

class CMario : public CGameObject
{
	CTailWhip* tailWhip = nullptr;

	// --- State Variables ---
	BOOLEAN isSitting;
	float maxVx;
	float ax;
	float ay;

	bool hasReachedPlatformAfterHover = true;

	float frictionX;
	
	int level;
	bool untouchable;
	ULONGLONG untouchableStart;
	BOOLEAN isOnPlatform;
	float pMeter = 0.0f;
	ULONGLONG pMeterMax = -1;
	int consecutiveEnemies;

	bool isTeleporting = false;
	bool isEnteringPortal = false;
	int exitDirection = 0; // 0 for down, 1 for up
	float entranceY;
	float targetX;
	float exitY;
	float yLevel;
	float offsetX = 0;
	float offsetY = 0;

	bool powerUp = 0;
	ULONGLONG powerUpStart = -1;
	bool tailUp = 0;
	ULONGLONG tailUpStart = -1;
	bool powerDown = 0;
	ULONGLONG powerDownStart = -1;
	bool tailDown = 0;
	ULONGLONG tailDownStart = -1;
	bool isHovering = 0;
	ULONGLONG hoveringStart = -1;
	bool isBraking = 0;
	float vxBeforeBraking = 0;
	ULONGLONG brakingStart = -1;
	bool isTailWhipping = 0;
	ULONGLONG tailWhipStart = -1;
	bool isKicking = 0;
	ULONGLONG kickStart = -1;

	bool isRendering = true;
	ULONGLONG lastRenderTime = -1;

	bool isHoldingKoopa = 0;
	int jumpCount = 0;
	bool isMoving = 0;
	bool isRunning = 0;
	bool isJumpButtonHeld = 0;
	bool isChangingLevel = false;
	bool isHoldingUpKey = false;
	bool isSkywalking = false;

	void OnCollisionWithGoomba(LPCOLLISIONEVENT e);
	void OnCollisionWithCoin(LPCOLLISIONEVENT e);
	void OnCollisionWithPortal(LPCOLLISIONEVENT e);
	void OnCollisionWithMushroom(LPCOLLISIONEVENT e);
	void OnCollisionWithSuperLeaf(LPCOLLISIONEVENT e);
	void OnCollisionWithPiranhaPlant(LPCOLLISIONEVENT e);
	void OnCollisionWithCoinQBlock(LPCOLLISIONEVENT e);
	void OnCollisionWithBuffQBlock(LPCOLLISIONEVENT e);
	void OnCollisionWithFireball(LPCOLLISIONEVENT e);
	void OnCollisionWithKoopa(LPCOLLISIONEVENT e);
	void OnCollisionWithWingedGoomba(LPCOLLISIONEVENT e);
	void OnCollisionWithLifeMushroom(LPCOLLISIONEVENT e);
	void OnCollisionWithFallPitch(LPCOLLISIONEVENT e);
	void OnCollisionWithLifeBrick(LPCOLLISIONEVENT e);
	void OnCollisionWithCoinBrick(LPCOLLISIONEVENT e);
	void OnCollisionWithActivator(LPCOLLISIONEVENT e);
	void OnCollisionWithActivatorBrick(LPCOLLISIONEVENT e);
	void OnCollisionWithWingedKoopa(LPCOLLISIONEVENT e);

	int GetAniIdBig();
	int GetAniIdSmall();
	int GetAniIdTail();

	void HandleBraking(DWORD dt);
	void HandleUntouchable(DWORD dt);
	void HandleHovering(DWORD dt);

public:
	CMario(int id, float x, float y, int z);
	virtual ~CMario();

	void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	void Render();
	void SetState(int state);

	int IsCollidable() { return (state != MARIO_STATE_DIE_ON_BEING_KILLED && state != MARIO_STATE_DIE_ON_FALLING && isTeleporting == 0); }
	int IsBlocking() { return 0; }

	void OnNoCollision(DWORD dt);
	void OnCollisionWith(LPCOLLISIONEVENT e);

	void CalculateScore(LPGAMEOBJECT obj);
	void Teleport(CPortal* portal);

	void GetBoundingBox(float& left, float& top, float& right, float& bottom);

	void StartUntouchable() { untouchable = 1; untouchableStart = GetTickCount64(); }
	void StartPowerUp() { powerUp = 1; powerUpStart = GetTickCount64(); isChangingLevel = true; }
	void StartTailUp() { tailUp = 1; tailUpStart = GetTickCount64(); isChangingLevel = true; }
	void StartPowerDown() { powerDown = 1; powerDownStart = GetTickCount64(); isChangingLevel = true; }
	void StartTailDown() { tailDown = 1; tailDownStart = GetTickCount64(); isChangingLevel = true; }
	void StartKick() { isKicking = 1; kickStart = GetTickCount64(); }
	void StartBraking();
	void StartHovering() { isHovering = 1; hoveringStart = GetTickCount64(); hasReachedPlatformAfterHover = false; }
	void StartTailWhip();

	int GetJumpCount() const { return jumpCount; }
	int GetLevel() const { return level; }

	bool IsPowerUp() const { return powerUp; }
	bool IsTailUp() const { return tailUp; }
	bool IsPowerDown() const { return powerDown; }
	bool IsTailDown() const { return tailDown; }
	bool IsHovering() const { return isHovering; }
	bool IsFlying() { return !isOnPlatform && (jumpCount > 1 || isHovering == 1); }
	bool IsRunning() const { return isRunning; }

	bool GetIsTeleporting() const { return isTeleporting; }
	bool GetIsEnteringPortal() const { return isEnteringPortal; }
	float GetYLevel() const { return yLevel; }

	void ToggleSkywalk() { isSkywalking = !isSkywalking; }
	int GetNx() { return nx; }
	float GetPMeter() const { return pMeter; }
	//bool MaxPMeter() const { return fabs(vx) == MARIO_MAX_RUNNING_SPEED; }
	BOOLEAN IsOnPlatform() const { return isOnPlatform; }
	BOOLEAN IsChangingLevel() const { return isChangingLevel; }
	CTailWhip* GetTailWhip() const { return tailWhip; }
	CTailWhip* GetActiveTailWhip();

	void SetLevel(int l);
	void SetIsRunning(bool isRunning) { this->isRunning = isRunning; }
	void SetIsHoldingKoopa(bool isHoldingKoopa) { this->isHoldingKoopa = isHoldingKoopa; }
	void SetIsJumpButtonHeld(bool held) { this->isJumpButtonHeld = held; }
	void SetIsHoldingUpKey(bool isHoldingUpKey) { this->isHoldingUpKey = isHoldingUpKey; }
};