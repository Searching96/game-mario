#include <algorithm>
#include "debug.h"

#include "Mario.h"
#include "Game.h"

#include "Goomba.h"
#include "Coin.h"
#include "Portal.h"
#include "QuestionBlock.h"
#include "Mushroom.h"
#include "SuperLeaf.h"
#include "PiranhaPlant.h"
#include "CoinQBlock.h"
#include "BuffQBlock.h"
#include "Koopa.h"

#include "Collision.h"

CMario::CMario(float x, float y, CTailWhip* tailWhip) : CGameObject(x, y)
{
	this->tailWhip = tailWhip;
	isSitting = false;
	maxVx = 0.0f;
	ax = 0.0f;
	ay = MARIO_GRAVITY;

	level = MARIO_LEVEL_SMALL;
	untouchable = 0;
	untouchableStart = -1;
	isOnPlatform = false;
	coin = 0;
	jumpCount = 0;
	frictionX = 0;
}

// In CMario::Update method, modify the hover handling
void CMario::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	// Track previous jump count to apply consistent jump impulse
	static int lastJumpCount = 0;
	int prevJumpCount = lastJumpCount;

	if (isTailWhipping == 1)
	{
		if (GetTickCount64() - tailWhipStart > MARIO_TAIL_WHIP_TIME)
		{
			isTailWhipping = 0;
		}
	}

	if (powerUp == 1)
	{
		if (GetTickCount64() - powerUpStart > MARIO_POWER_UP_TIME)
		{
			powerUp = 0;
			SetLevel(MARIO_LEVEL_BIG);
		}
		return;
	}
	if (tailUp == 1)
	{
		if (GetTickCount64() - tailUpStart > MARIO_TAIL_UP_TIME)
		{
			tailUp = 0;
			SetLevel(MARIO_LEVEL_TAIL);
		}
		return;
	}
	if (powerDown == 1)
	{
		if (GetTickCount64() - powerDownStart > MARIO_POWER_DOWN_TIME)
		{
			powerDown = 0;
			y += 6; // RED ALERT
			SetLevel(MARIO_LEVEL_SMALL);
			StartUntouchable();
		}
		return;
	}
	if (tailDown == 1)
	{
		if (GetTickCount64() - tailDownStart > MARIO_TAIL_DOWN_TIME)
		{
			tailDown = 0;
			SetLevel(MARIO_LEVEL_BIG);
			StartUntouchable();
		}
		return;
	}

	HandleUntouchable(dt);
	HandleHovering(dt);
	
	// Multi-jump gravity scaling and horizontal cap
	if (jumpCount > 0)
	{
		// Scale gravity when ascending
		ay = (vy < 0) ? MARIO_GRAVITY * 0.25f : MARIO_GRAVITY * 0.4;

		// Cap horizontal speed during multi-jump
		float multiJumpMaxVx = MARIO_MAX_RUNNING_SPEED * 0.75f;
		if (vx > multiJumpMaxVx) vx = multiJumpMaxVx;
		else if (vx < -multiJumpMaxVx) vx = -multiJumpMaxVx;
	}
	else if (!isOnPlatform && isJumpButtonHeld && vy < 0)
	{
		ay = MARIO_GRAVITY * 0.225f; // Reduced gravity during ascent
	}
	else if (!isHovering)
	{
		// Normal gravity
		ay = MARIO_GRAVITY;
	}

	// Apply acceleration and gravity
	vx += ax * dt;
	vy += ay * dt;

	// Friction when not moving and not jumping
	if (jumpCount < 1 && !isHovering && isMoving == 0)
	{
		if (vx > 0)
			vx = max(0.0f, vx - frictionX * dt);
		else if (vx < 0)
			vx = min(0.0f, vx + frictionX * dt);
	}

	// Handle braking
	HandleBraking(dt);

	// Cap horizontal velocity to maxVx
	float absMaxVx = fabs(maxVx);
	if (vx > absMaxVx) vx = absMaxVx;
	else if (vx < -absMaxVx) vx = -absMaxVx;

	// Apply jump impulse on button press: full first jump, reduced constant for multijumps
	if (jumpCount > prevJumpCount)
	{
		const float multiJumpScale = 0.75f; // 75% of full jump speed for all subsequent jumps
		float impulse = MARIO_JUMP_SPEED_Y * multiJumpScale;
		vy = -impulse;
	}

	// Reset jump count when landing
	if (isOnPlatform)
		jumpCount = 0;

	// Update lastJumpCount for next frame
	lastJumpCount = jumpCount;

	// Cap vertical falling speed
	if (vy > MARIO_MAX_FALLING_SPEED) vy = MARIO_MAX_FALLING_SPEED;
	if (vy < MARIO_MAX_JUMP_SPEED) vy = MARIO_MAX_JUMP_SPEED;

	//if (nx > 0)
	//	tailWhip->SetPosition(x + 6, y + 6);
	//else
	//	tailWhip->SetPosition(x - 6, y + 6);

	DebugOutTitle(L"vx=%f, ax=%f, vy=%f, ay=%f, jc=%d, fx=%f, iop=%d, imv=%d\n",
		vx, ax, vy, ay, jumpCount, frictionX, isOnPlatform, isMoving);

	// Process collisions
	CCollision::GetInstance()->Process(this, dt, coObjects);
}

void CMario::HandleBraking(DWORD dt)
{
	if (isBraking)
	{
		float brakeForce = MARIO_DECELERATION_X;
		if (vxBeforeBraking > 0)
			vx = max(0.0f, vx - brakeForce * dt);
		else
			vx = min(0.0f, vx + brakeForce * dt);

		if (GetTickCount64() - brakingStart > MARIO_BRAKE_TIME || fabs(vx) < 0.1f)
		{
			isBraking = 0;
			if (vxBeforeBraking > 0)
			{
				nx = -1;
				vx = -MARIO_WALKING_SPEED * 0.5f;
				maxVx = -MARIO_MAX_WALKING_SPEED;
			}
			else
			{
				nx = 1;
				vx = MARIO_WALKING_SPEED * 0.5f;
				maxVx = MARIO_MAX_WALKING_SPEED;
			}
		}
	}
}

void CMario::HandleUntouchable(DWORD dt)
{
	if (GetTickCount64() - untouchableStart > MARIO_UNTOUCHABLE_TIME)
	{
		untouchableStart = 0;
		untouchable = 0;
	}
}

void CMario::HandleHovering(DWORD dt)
{
	if (isHovering)
	{
		ay = MARIO_GRAVITY * 0.3f;
		if (vy > 0) vy = MARIO_HOVER_SPEED_Y;
		if (GetTickCount64() - hoveringStart > MARIO_HOVER_TIME || isOnPlatform)
		{
			isHovering = 0;
			ay = MARIO_GRAVITY;
			vx = 0;
		}
	}
}

void CMario::OnNoCollision(DWORD dt)
{
	x += vx * dt;
	y += vy * dt;
	isOnPlatform = false;
}

void CMario::OnCollisionWith(LPCOLLISIONEVENT e)
{
	if (e->ny != 0 && e->obj->IsBlocking())
	{
		vy = 0;
		if (e->ny < 0) isOnPlatform = true;
	}
	else
		if (e->nx != 0 && e->obj->IsBlocking())
		{
			vx = 0;
		}

	if (dynamic_cast<CGoomba*>(e->obj))
		OnCollisionWithGoomba(e);
	//else if (dynamic_cast<CCoin*>(e->obj))
	//	OnCollisionWithCoin(e);
	else if (dynamic_cast<CPortal*>(e->obj))
		OnCollisionWithPortal(e);
	else if (dynamic_cast<CQuestionBlock*>(e->obj))
		OnCollisionWithQuestionBlock(e);
	else if (dynamic_cast<CMushroom*>(e->obj))
		OnCollisionWithMushroom(e);
	else if (dynamic_cast<CSuperLeaf*>(e->obj))
		OnCollisionWithSuperLeaf(e);
	else if (dynamic_cast<CPiranhaPlant*>(e->obj))
		OnCollisionWithPiranhaPlant(e);
	else if (dynamic_cast<CCoinQBlock*>(e->obj))
		OnCollisionWithCoinQBlock(e);
	else if (dynamic_cast<CBuffQBlock*>(e->obj))
		OnCollisionWithBuffQBlock(e);
	else if (dynamic_cast<CFireball*>(e->obj))
		OnCollisionWithFireball(e);
	else if (dynamic_cast<CKoopa*>(e->obj))
		OnCollisionWithKoopa(e);
}

void CMario::OnCollisionWithGoomba(LPCOLLISIONEVENT e)
{
	CGoomba* goomba = dynamic_cast<CGoomba*>(e->obj);

	// jump on top >> kill Goomba and deflect a bit 
	if (e->ny < 0)
	{
		if (goomba->GetState() != GOOMBA_STATE_DIE)
		{
			goomba->SetState(GOOMBA_STATE_DIE);
			vy = -MARIO_JUMP_DEFLECT_SPEED;
		}
	}
	else // hit by Goomba
	{
		if (untouchable == 0)
		{
			if (goomba->GetState() != GOOMBA_STATE_DIE)
			{
				if (level == MARIO_LEVEL_BIG)
				{
					y += 6; // RED ALERT
					SetState(MARIO_STATE_POWER_DOWN);
				}
				else if (level == MARIO_LEVEL_TAIL)
				{
					SetState(MARIO_STATE_TAIL_DOWN);
				}
				else
				{
					DebugOut(L">>> Mario DIE >>> \n");
					SetState(MARIO_STATE_DIE);
				}
			}
		}
	}
}

void CMario::OnCollisionWithCoin(LPCOLLISIONEVENT e)
{
	CCoin* c = dynamic_cast<CCoin*>(e->obj);

	if (c)
	{
		if (c->GetState() == COIN_STATE_STATIC)
			c->Delete();
		else if (e->ny > 0 && e->nx == 0 && c->GetState() == COIN_STATE_DYNAMIC)
			c->SetState(COIN_STATE_BOUNCE_UP);

		coin++;
	}
}

void CMario::OnCollisionWithPortal(LPCOLLISIONEVENT e)
{
	CPortal* p = (CPortal*)e->obj;
	CGame::GetInstance()->InitiateSwitchScene(p->GetSceneId());
}

void CMario::OnCollisionWithQuestionBlock(LPCOLLISIONEVENT e)
{
	CQuestionBlock* qb = dynamic_cast<CQuestionBlock*>(e->obj);

	if (qb)
		if (e->ny > 0 && e->nx == 0 && qb->GetState() == QUESTIONBLOCK_STATE_NOT_HIT)
			qb->SetState(QUESTIONBLOCK_STATE_BOUNCE_UP);
}


void CMario::OnCollisionWithCoinQBlock(LPCOLLISIONEVENT e)
{
	CCoinQBlock* cqb = dynamic_cast<CCoinQBlock*>(e->obj);
	if (cqb)
		if (e->ny > 0 && e->nx == 0 && cqb->GetState() == QUESTIONBLOCK_STATE_NOT_HIT)
			cqb->SetState(QUESTIONBLOCK_STATE_BOUNCE_UP);
}

void CMario::OnCollisionWithBuffQBlock(LPCOLLISIONEVENT e)
{
	CBuffQBlock* bqb = dynamic_cast<CBuffQBlock*>(e->obj);
	if (bqb)
		if (e->ny > 0 && e->nx == 0 && bqb->GetState() == QUESTIONBLOCK_STATE_NOT_HIT)
		{
			if (level == MARIO_LEVEL_SMALL)
			{
				CMushroom* mushroom = bqb->GetMushroom();
				if (mushroom)
				{
					float mX, mY;
					mushroom->GetPosition(mX, mY);
					if (x < mX)
						mushroom->SetCollisionNx(1);
					else
						mushroom->SetCollisionNx(-1);
				}
				bqb->SetToSpawn(0);
			}
			else
				bqb->SetToSpawn(1);
			bqb->SetState(QUESTIONBLOCK_STATE_BOUNCE_UP);
		}
}

void CMario::OnCollisionWithMushroom(LPCOLLISIONEVENT e)
{
	CMushroom* mushroom = dynamic_cast<CMushroom*>(e->obj);

	if (mushroom)
	{
		if (mushroom->GetState() == MUSHROOM_STATE_MOVING)
		{
			mushroom->Delete();
			if (level == MARIO_LEVEL_SMALL)
				this->SetState(MARIO_STATE_POWER_UP);
		}
	}
}

void CMario::OnCollisionWithSuperLeaf(LPCOLLISIONEVENT e)
{
	CSuperLeaf* superleaf = dynamic_cast<CSuperLeaf*>(e->obj);

	if (superleaf)
	{
		if (superleaf->GetState() == SUPERLEAF_STATE_FLOATING_RIGHT || superleaf->GetState() == SUPERLEAF_STATE_FLOATING_LEFT)
		{
			superleaf->Delete();
			if (level == MARIO_LEVEL_BIG)
				this->SetState(MARIO_STATE_TAIL_UP);
		}
	}
}

void CMario::OnCollisionWithPiranhaPlant(LPCOLLISIONEVENT e)
{
	CPiranhaPlant* plant = dynamic_cast<CPiranhaPlant*>(e->obj);
	if (untouchable == 0)
	{
		if (plant->GetState() != PIRANHA_PLANT_STATE_HIDDEN)
		{
			if (level > MARIO_LEVEL_SMALL)
			{
				level = MARIO_LEVEL_SMALL;
				StartUntouchable();
			}
			else
			{
				DebugOut(L">>> Mario DIE >>> \n");
				SetState(MARIO_STATE_DIE);
			}
		}
	}
}

void CMario::OnCollisionWithFireball(LPCOLLISIONEVENT e)
{
	CFireball* fireball = dynamic_cast<CFireball*>(e->obj);
	if (untouchable == 0)
	{
		if (fireball->GetState() != FIREBALL_STATE_STATIC)
		{
			if (level > MARIO_LEVEL_SMALL)
			{
				level = MARIO_LEVEL_SMALL;
				StartUntouchable();
			}
			else
			{
				DebugOut(L">>> Mario DIE >>> \n");
				SetState(MARIO_STATE_DIE);
			}
		}
	}
}

void CMario::OnCollisionWithKoopa(LPCOLLISIONEVENT e)
{
	CKoopa* koopa = dynamic_cast<CKoopa*>(e->obj);

	// jump on top >> Koopa hide in shell and deflect a bit 
	if (e->ny < 0)
	{
		if (koopa->GetState() != KOOPA_STATE_SHELL_STATIC 
		&& koopa->GetState() != KOOPA_STATE_SHELL_DYNAMIC)
		{
			koopa->SetState(KOOPA_STATE_SHELL_STATIC);
			vy = -MARIO_JUMP_DEFLECT_SPEED;
		}
	}
	else // hit by Koopa
	{
		if (untouchable == 0)
		{
			if (koopa->GetState() == KOOPA_STATE_WALKING_LEFT
			|| koopa->GetState() == KOOPA_STATE_WALKING_RIGHT)
			{
				if (level > MARIO_LEVEL_SMALL)
				{
					level = MARIO_LEVEL_SMALL;
					StartUntouchable();
				}
				else
				{
					DebugOut(L">>> Mario DIE >>> \n");
					SetState(MARIO_STATE_DIE);
				}
			}
		}
	}
}

//
// Get animation ID for small Mario
//
int CMario::GetAniIdSmall()
{
	int aniId = -1;
	if (!isOnPlatform)
	{
		if (fabs(vx) == MARIO_MAX_RUNNING_SPEED)
		{
			if (nx >= 0)
				aniId = ID_ANI_MARIO_SMALL_JUMP_RUN_RIGHT;
			else
				aniId = ID_ANI_MARIO_SMALL_JUMP_RUN_LEFT;
		}
		else
		{
			if (nx >= 0)
				aniId = ID_ANI_MARIO_SMALL_JUMP_WALK_RIGHT;
			else
				aniId = ID_ANI_MARIO_SMALL_JUMP_WALK_LEFT;
		}
	}
	else if (isSitting)
	{
		if (nx > 0)
			aniId = ID_ANI_MARIO_SIT_RIGHT;
		else
			aniId = ID_ANI_MARIO_SIT_LEFT;
	}
	else if (vx == 0)
	{
		if (nx > 0) aniId = ID_ANI_MARIO_SMALL_IDLE_RIGHT;
		else aniId = ID_ANI_MARIO_SMALL_IDLE_LEFT;
	}
	else if (vx > 0)
	{
		if (isBraking)
			aniId = ID_ANI_MARIO_SMALL_BRACE_RIGHT;
		else if (fabs(vx) <= MARIO_HALF_RUN_ACCEL_SPEED)
			aniId = ID_ANI_MARIO_SMALL_WALKING_RIGHT;
		else if (fabs(vx) > MARIO_HALF_RUN_ACCEL_SPEED && fabs(vx) < MARIO_MAX_RUNNING_SPEED)
			aniId = ID_ANI_MARIO_SMALL_HALF_RUN_ACCEL_RIGHT;
		else if (fabs(vx) == MARIO_MAX_RUNNING_SPEED)
			aniId = ID_ANI_MARIO_SMALL_RUNNING_RIGHT;
	}
	else // vx < 0
	{
		if (isBraking)
			aniId = ID_ANI_MARIO_SMALL_BRACE_LEFT;
		else if (fabs(vx) <= MARIO_HALF_RUN_ACCEL_SPEED)
			aniId = ID_ANI_MARIO_SMALL_WALKING_LEFT;
		else if (fabs(vx) > MARIO_HALF_RUN_ACCEL_SPEED && fabs(vx) < MARIO_MAX_RUNNING_SPEED)
			aniId = ID_ANI_MARIO_SMALL_HALF_RUN_ACCEL_LEFT;
		else if (fabs(vx) == MARIO_MAX_RUNNING_SPEED)
			aniId = ID_ANI_MARIO_SMALL_RUNNING_LEFT;
	}

	if (powerUp)
	{
		if (nx > 0) aniId = ID_ANI_MARIO_SMALL_POWER_UP_RIGHT;
		else aniId = ID_ANI_MARIO_SMALL_POWER_UP_LEFT;
	}

	if (aniId == -1) aniId = ID_ANI_MARIO_SMALL_IDLE_RIGHT;

	return aniId;
}


//
// Get animdation ID for big Mario
//
int CMario::GetAniIdBig()
{
	int aniId = -1;
	if (!isOnPlatform)
	{
		if (fabs(vx) == MARIO_MAX_RUNNING_SPEED)
		{
			if (nx >= 0)
				aniId = ID_ANI_MARIO_JUMP_RUN_RIGHT;
			else
				aniId = ID_ANI_MARIO_JUMP_RUN_LEFT;
		}
		else
		{
			if (nx >= 0)
				aniId = ID_ANI_MARIO_JUMP_WALK_RIGHT;
			else
				aniId = ID_ANI_MARIO_JUMP_WALK_LEFT;
		}
	}
	else if (isSitting)
	{
		if (nx > 0)
			aniId = ID_ANI_MARIO_SIT_RIGHT;
		else
			aniId = ID_ANI_MARIO_SIT_LEFT;
	}
	else if (vx == 0)
	{
		if (nx > 0) aniId = ID_ANI_MARIO_IDLE_RIGHT;
		else aniId = ID_ANI_MARIO_IDLE_LEFT;
	}
	else if (vx > 0)
	{
		if (isBraking)
			aniId = ID_ANI_MARIO_BRACE_RIGHT;
		else if (fabs(vx) <= MARIO_HALF_RUN_ACCEL_SPEED)
			aniId = ID_ANI_MARIO_WALKING_RIGHT;
		else if (fabs(vx) > MARIO_HALF_RUN_ACCEL_SPEED && fabs(vx) < MARIO_MAX_RUNNING_SPEED)
			aniId = ID_ANI_MARIO_HALF_RUN_ACCEL_RIGHT;
		else if (fabs(vx) == MARIO_MAX_RUNNING_SPEED)
			aniId = ID_ANI_MARIO_RUNNING_RIGHT;
	}
	else // vx < 0
	{
		if (isBraking)
			aniId = ID_ANI_MARIO_BRACE_LEFT;
		else if (fabs(vx) <= MARIO_HALF_RUN_ACCEL_SPEED)
			aniId = ID_ANI_MARIO_WALKING_LEFT;
		else if (fabs(vx) > MARIO_HALF_RUN_ACCEL_SPEED && fabs(vx) < MARIO_MAX_RUNNING_SPEED)
			aniId = ID_ANI_MARIO_HALF_RUN_ACCEL_LEFT;
		else if (fabs(vx) == MARIO_MAX_RUNNING_SPEED)
			aniId = ID_ANI_MARIO_RUNNING_LEFT;
	}

	if (tailUp)
	{
		aniId = ID_ANI_MARIO_TAIL_UP;
	}
	if (powerDown)
	{
		if (nx > 0) aniId = ID_ANI_MARIO_POWER_DOWN_RIGHT;
		else aniId = ID_ANI_MARIO_POWER_DOWN_LEFT;
	}

	if (aniId == -1) aniId = ID_ANI_MARIO_IDLE_RIGHT;

	return aniId;
}

//
// Get animation ID for tail Mario
//
int CMario::GetAniIdTail()
{
	int aniId = -1;
	if (!isOnPlatform)
	{
		if (fabs(vx) == MARIO_MAX_RUNNING_SPEED)
		{
			if (nx >= 0)
				aniId = ID_ANI_MARIO_TAIL_JUMP_RUN_RIGHT;
			else
				aniId = ID_ANI_MARIO_TAIL_JUMP_RUN_LEFT;
		}
		else
		{
			if (nx >= 0)
				aniId = ID_ANI_MARIO_TAIL_JUMP_WALK_RIGHT;
			else
				aniId = ID_ANI_MARIO_TAIL_JUMP_WALK_LEFT;
		}
	}
	else
		if (isSitting)
		{
			if (nx > 0)
				aniId = ID_ANI_MARIO_TAIL_SIT_RIGHT;
			else
				aniId = ID_ANI_MARIO_TAIL_SIT_LEFT;
		}
		else
			if (vx == 0)
			{
				if (nx > 0) aniId = ID_ANI_MARIO_TAIL_IDLE_RIGHT;
				else aniId = ID_ANI_MARIO_TAIL_IDLE_LEFT;
			}
			else if (vx > 0)
			{
				if (isBraking)
					aniId = ID_ANI_MARIO_TAIL_BRACE_RIGHT;
				else if (fabs(vx) <= MARIO_HALF_RUN_ACCEL_SPEED)
					aniId = ID_ANI_MARIO_TAIL_WALKING_RIGHT;
				else if (fabs(vx) > MARIO_HALF_RUN_ACCEL_SPEED && fabs(vx) < MARIO_MAX_RUNNING_SPEED)
					aniId = ID_ANI_MARIO_TAIL_HALF_RUN_ACCEL_RIGHT;
				else if (fabs(vx) == MARIO_MAX_RUNNING_SPEED)
					aniId = ID_ANI_MARIO_TAIL_RUNNING_RIGHT;
			}
			else // vx < 0
			{
				if (isBraking)
					aniId = ID_ANI_MARIO_TAIL_BRACE_LEFT;
				else if (fabs(vx) <= MARIO_HALF_RUN_ACCEL_SPEED)
					aniId = ID_ANI_MARIO_TAIL_WALKING_LEFT;
				else if (fabs(vx) > MARIO_HALF_RUN_ACCEL_SPEED && fabs(vx) < MARIO_MAX_RUNNING_SPEED)
					aniId = ID_ANI_MARIO_TAIL_HALF_RUN_ACCEL_LEFT;
				else if (fabs(vx) == MARIO_MAX_RUNNING_SPEED)
					aniId = ID_ANI_MARIO_TAIL_RUNNING_LEFT;
			}

	if (jumpCount > 0)
		if (nx > 0)
			aniId = ID_ANI_MARIO_TAIL_MULTIJUMP_RIGHT;
		else
			aniId = ID_ANI_MARIO_TAIL_MULTIJUMP_LEFT;

	if (isHovering)
	{
		if (nx > 0)
			aniId = ID_ANI_MARIO_TAIL_HOVERING_RIGHT;
		else
			aniId = ID_ANI_MARIO_TAIL_HOVERING_LEFT;
		tailWagged = 1;
	}

	if (tailDown)
	{
		aniId = ID_ANI_MARIO_TAIL_DOWN;
	}

	if (isTailWhipping == 1)
	{
		if (nx > 0)
			aniId = ID_ANI_MARIO_TAIL_WHIP_RIGHT;
		else
			aniId = ID_ANI_MARIO_TAIL_WHIP_LEFT;
	}
		
	if (aniId == -1) aniId = ID_ANI_MARIO_TAIL_IDLE_RIGHT;

	return aniId;
}

void CMario::Render()
{
	if (untouchable)
	{
		ULONGLONG currentTime = GetTickCount64();
		if (currentTime - lastRenderTime > MARIO_UNTOUCHABLE_RENDER_INTERVAL)
		{
			isRendering = !isRendering;
			lastRenderTime = currentTime;
		}

		if (!isRendering)
			return;
	}

	CAnimations* animations = CAnimations::GetInstance();
	int aniId = -1;

	if (state == MARIO_STATE_DIE)
		aniId = ID_ANI_MARIO_DIE;
	else if (level == MARIO_LEVEL_BIG)
		aniId = GetAniIdBig();
	else if (level == MARIO_LEVEL_SMALL)
		aniId = GetAniIdSmall();
	else if (level == MARIO_LEVEL_TAIL)
		aniId = GetAniIdTail();

	animations->Get(aniId)->Render(x, y);

	//RenderBoundingBox();
	
	//DebugOutTitle(L"Coins: %d", coin);
}

// Modify the SetState method for improved braking and hovering handling
void CMario::SetState(int state)
{
	// DIE is the end state, cannot be changed! 
	if (this->state == MARIO_STATE_DIE) return;

	if (this->state == MARIO_STATE_POWER_UP && (GetTickCount64() - powerUpStart <= MARIO_POWER_UP_TIME))
		return;
	if (this->state == MARIO_STATE_TAIL_UP && (GetTickCount64() - tailUpStart <= MARIO_TAIL_UP_TIME))
		return;
	if (this->state == MARIO_STATE_POWER_DOWN && (GetTickCount64() - powerUpStart <= MARIO_POWER_DOWN_TIME))
		return;
	if (this->state == MARIO_STATE_TAIL_DOWN && (GetTickCount64() - tailUpStart <= MARIO_TAIL_DOWN_TIME))
		return;

	int previousState = this->state;

	switch (state)
	{
	case MARIO_STATE_TAIL_WHIP:
		if (nx > 0)
			tailWhip->SetState(TAIL_STATE_WHIPPING_RIGHT);
		else
			tailWhip->SetState(TAIL_STATE_WHIPPING_LEFT);
		StartTailWhip();
		break;

	case MARIO_STATE_RUNNING_RIGHT:
		if (isSitting) break;
		if (isOnPlatform && vx < 0) // If Mario is moving left, set state to brake
		{
			if (isOnPlatform)
			{
				SetState(MARIO_STATE_BRAKE);
				break;
			}
		}
		if (isBraking) break;
		maxVx = MARIO_MAX_RUNNING_SPEED;

		// Kiểm tra rõ ràng hơn cho trạng thái nhảy và chuyển hướng
		if (!isOnPlatform)
		{
			if (vx <= 0)  // Đang đứng im hoặc đang đi sang trái trong không trung
			{
				vx = MARIO_MAX_RUNNING_SPEED / SPEED_DIVISOR;
				ax = 0.0f;  // Đặt ax thành 0 một cách rõ ràng
				//DebugOut(L"Air direction change - to right, ax set to 0\n");
			}
		}
		else
		{
			if (fabs(vx) < fabs(MARIO_RUNNING_SPEED)) vx = MARIO_RUNNING_SPEED;
			ax = MARIO_ACCEL_RUN_X;
		}
		frictionX = 0;
		isMoving = 1;
		nx = 1;
		break;

	case MARIO_STATE_RUNNING_LEFT:
		if (isSitting) break;
		if (isOnPlatform && vx > 0) // If Mario is moving right, set state to brake
		{
			if (isOnPlatform)
			{
				SetState(MARIO_STATE_BRAKE);
				break;
			}
		}
		maxVx = -MARIO_MAX_RUNNING_SPEED;

		// Kiểm tra rõ ràng hơn cho trạng thái nhảy và chuyển hướng
		if (!isOnPlatform)
		{
			if (vx >= 0)  // Đang đứng im hoặc đang đi sang phải trong không trung
			{
				vx = -MARIO_MAX_RUNNING_SPEED / SPEED_DIVISOR;
				ax = 0.0f;  // Đặt ax thành 0 một cách rõ ràng
				DebugOut(L"Air direction change - to left, ax set to 0\n");
			}
		}
		else
		{
			if (fabs(vx) < fabs(MARIO_RUNNING_SPEED)) vx = -MARIO_RUNNING_SPEED;
			ax = -MARIO_ACCEL_RUN_X;
		}
		frictionX = 0;
		nx = -1;
		isMoving = 1;
		break;

	case MARIO_STATE_WALKING_RIGHT:
		if (isSitting) break;
		if (isOnPlatform && vx < 0) // If Mario is moving left, set state to brake
		{
			if (isOnPlatform)
			{
				SetState(MARIO_STATE_BRAKE);
				break;
			}
		}
		maxVx = MARIO_MAX_WALKING_SPEED;

		// Kiểm tra rõ ràng hơn cho trạng thái nhảy và chuyển hướng
		if (!isOnPlatform)
		{
			if (vx <= 0)  // Đang đứng im hoặc đang đi sang trái trong không trung
			{
				vx = MARIO_MAX_WALKING_SPEED / SPEED_DIVISOR;
				ax = 0.0f;  // Đặt ax thành 0 một cách rõ ràng
				DebugOut(L"Air direction change - to right, ax set to 0\n");
			}
		}
		else
		{
			if (fabs(vx) < fabs(MARIO_WALKING_SPEED)) vx = MARIO_WALKING_SPEED;
			ax = MARIO_ACCEL_WALK_X;
		}
		frictionX = 0;
		isMoving = 1;
		nx = 1;
		break;

	case MARIO_STATE_WALKING_LEFT:
		if (isSitting) break;
		if (isOnPlatform && vx > 0) // If Mario is moving right, set state to brake
		{
			if (isOnPlatform)
			{
				SetState(MARIO_STATE_BRAKE);
				break;
			}
		}
		maxVx = -MARIO_MAX_WALKING_SPEED;

		// Kiểm tra rõ ràng hơn cho trạng thái nhảy và chuyển hướng
		if (!isOnPlatform)
		{
			if (vx >= 0)  // Đang đứng im hoặc đang đi sang phải trong không trung
			{
				vx = -MARIO_MAX_WALKING_SPEED / SPEED_DIVISOR;
				ax = 0.0f;  // Đặt ax thành 0 một cách rõ ràng
				//DebugOut(L"Air direction change - to left, ax set to 0\n");
			}
		}
		else
		{
			if (fabs(vx) < fabs(MARIO_WALKING_SPEED)) vx = -MARIO_WALKING_SPEED;
			ax = -MARIO_ACCEL_WALK_X;
		}
		frictionX = 0;
		isMoving = 1;
		nx = -1;
		break;

	case MARIO_STATE_JUMP:
		if (isSitting) break;
		if (isOnPlatform)
		{
			if (fabs(vx) == MARIO_MAX_RUNNING_SPEED)
				vy = -MARIO_JUMP_RUN_SPEED_Y;
			else
				vy = -MARIO_JUMP_SPEED_Y;
			isJumpButtonHeld = 1;
		}
		else if (level == MARIO_LEVEL_TAIL)
		{
			if (jumpCount >= MAX_JUMP_COUNT)
			{
				SetState(MARIO_STATE_HOVER);
				break;
			}

			if (fabs(vx) == MARIO_MAX_RUNNING_SPEED)
			{
				jumpCount++;
				vy = -MARIO_JUMP_SPEED_Y;
			}
			else if (jumpCount >= 1)
			{
				jumpCount++;
				vy = -MARIO_JUMP_SPEED_Y / 2.0f;
				//vx = (nx > 0) ? MARIO_MAX_WALKING_SPEED : -MARIO_MAX_WALKING_SPEED;
				maxVx = (nx > 0) ? MARIO_MAX_WALKING_SPEED : -MARIO_MAX_WALKING_SPEED;
			}
			else if (jumpCount == 0)
			{
				SetState(MARIO_STATE_HOVER);
			}
		}
		break;

	case MARIO_STATE_RELEASE_JUMP:
		if (vy < 0) vy += MARIO_JUMP_SPEED_Y / 2;
		isJumpButtonHeld = 0;
		break;

	case MARIO_STATE_SIT:
		if (isMoving == 1) break; 
		if (isOnPlatform && level != MARIO_LEVEL_SMALL)
		{
			state = MARIO_STATE_IDLE;
			isSitting = true;
			vx = 0; vy = 0.0f;
			y += MARIO_SIT_HEIGHT_ADJUST;
		}
		break;

	case MARIO_STATE_SIT_RELEASE:
		if (isSitting)
		{
			isSitting = false;
			state = MARIO_STATE_IDLE;
			y -= MARIO_SIT_HEIGHT_ADJUST;
		}
		break;

	case MARIO_STATE_IDLE:
		ax = 0.0f;
		//vx = 0.0f;
		break;

	case MARIO_STATE_DIE:
		vy = -MARIO_JUMP_DEFLECT_SPEED;
		vx = 0;
		ax = 0;
		break;

	case MARIO_STATE_POWER_UP:
		StartPowerUp();
		break;

	case MARIO_STATE_TAIL_UP:
		StartTailUp();
		break;

	case MARIO_STATE_POWER_DOWN:
		StartPowerDown();
		break;

	case MARIO_STATE_TAIL_DOWN:
		StartTailDown();
		break;

	case MARIO_STATE_BRAKE:
		StartBraking();
		// Modified braking behavior - don't force a specific velocity
		// Let the braking friction in Update() handle slowing down
		ax = 0.0f; // Stop acceleration while braking
		break;

	case MARIO_STATE_HOVER:
		// Modified hovering - smoother transition
		if (vy > 0) {  // Only hover when falling
			vy = MARIO_HOVER_SPEED_Y;
		}
		// Maintain current horizontal velocity but apply slight deceleration
		ax = 0.0f;
		maxVx = (nx > 0) ? MARIO_MAX_WALKING_SPEED : -MARIO_MAX_WALKING_SPEED;
		// Keep some horizontal momentum but don't force a speed
		StartHovering();
		break;

	case MARIO_STATE_RELEASE_MOVE:
		isMoving = 0;
		frictionX = MARIO_FRICTION_X;
		break;
	}

	DebugOut(L"SetState: %d\n", state);
	CGameObject::SetState(state);
}

void CMario::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	if (level == MARIO_LEVEL_BIG || level == MARIO_LEVEL_TAIL)
	{
		if (isSitting)
		{
			left = x - MARIO_BIG_SITTING_BBOX_WIDTH / 2;
			top = y - MARIO_BIG_SITTING_BBOX_HEIGHT / 2;
			right = left + MARIO_BIG_SITTING_BBOX_WIDTH;
			bottom = top + MARIO_BIG_SITTING_BBOX_HEIGHT;
		}
		else
		{
			left = x - MARIO_BIG_BBOX_WIDTH / 2;
			top = y - MARIO_BIG_BBOX_HEIGHT / 2;
			right = left + MARIO_BIG_BBOX_WIDTH;
			bottom = top + MARIO_BIG_BBOX_HEIGHT;
		}
	}
	else
	{
		left = x - MARIO_SMALL_BBOX_WIDTH / 2;
		top = y - MARIO_SMALL_BBOX_HEIGHT / 2;
		right = left + MARIO_SMALL_BBOX_WIDTH;
		bottom = top + MARIO_SMALL_BBOX_HEIGHT;
	}
}

void CMario::SetLevel(int l)
{
	level = l;

	// Adjust position to avoid falling off platform
	if (this->level == MARIO_LEVEL_SMALL)
	{
		y -= (MARIO_BIG_BBOX_HEIGHT - MARIO_SMALL_BBOX_HEIGHT) / 2;
	}
}


// Modify the StartBraking function
void CMario::StartBraking()
{
	if (isBraking) return; // Don't restart braking if already braking

	isBraking = 1;
	vxBeforeBraking = vx; // Store velocity before braking
	brakingStart = GetTickCount64();

	// When braking, we want to face the opposite direction
	// This is for animation purposes
	if (vx > 0) {
		nx = -1; // Face left when braking from right movement
	}
	else if (vx < 0) {
		nx = 1;  // Face right when braking from left movement
	}
}