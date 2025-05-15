#include <algorithm>
#include "debug.h"

#include "Mario.h"
#include "Game.h"

#include "Goomba.h"
#include "Coin.h"
#include "Portal.h"
#include "QuestionBlock.h"
#include "LifeMushroom.h"
#include "Mushroom.h"
#include "SuperLeaf.h"
#include "PiranhaPlant.h"
#include "CoinQBlock.h"
#include "BuffQBlock.h"
#include "Koopa.h"
#include "WingedGoomba.h"
#include "FallPitch.h"
#include "Particle.h"
#include "LifeBrick.h"

#include "Collision.h"

#define DEPENDENT_ID				9999 // taken from PlayScene.cpp

CMario::CMario(int id, float x, float y, int z) : CGameObject(id, x, y, z)
{
	this->tailWhip = new CTailWhip(DEPENDENT_ID, x, y, z + 10);

	isSitting = false;
	maxVx = 0.0f;
	ax = 0.0f;
	ay = MARIO_GRAVITY;

	level = MARIO_LEVEL_SMALL;
	untouchable = 0;
	untouchableStart = -1;
	isOnPlatform = false;
	jumpCount = 0;
	frictionX = 0;
	powerUp = 0;
	powerUpStart = -1;
	tailUp = 0;
	tailUpStart = -1;
	powerDown = 0;
	powerDownStart = -1;
	tailDown = 0;
	tailDownStart = -1;
	isRendering = true;
	lastRenderTime = -1;
	isHovering = 0;
	hoveringStart = -1;
	isBraking = 0;
	vxBeforeBraking = 0;
	brakingStart = -1;
	isTailWhipping = 0;
	tailWhipStart = -1;
	isKicking = 0;
	kickStart = -1;
	isHoldingKoopa = 0;
	isMoving = 0;
	isRunning = 0;
	isJumpButtonHeld = 0;
	pMeter = 0.0f;
	pMeterMax = -1;

	SetState(MARIO_STATE_IDLE);
}


CMario::~CMario()
{
	delete tailWhip;
	tailWhip = nullptr;
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
			y -= 6; // RED ALERT
			SetLevel(MARIO_LEVEL_BIG);
			isChangingLevel = 0;
		}
		return;
	}
	if (tailUp == 1)
	{
		if (GetTickCount64() - tailUpStart > MARIO_TAIL_UP_TIME)
		{
			tailUp = 0;
			SetLevel(MARIO_LEVEL_TAIL);
			isChangingLevel = 0;
		}
		return;
	}
	if (powerDown == 1)
	{
		if (GetTickCount64() - powerDownStart > MARIO_POWER_DOWN_TIME)
		{
			powerDown = 0;
			SetLevel(MARIO_LEVEL_SMALL);
			StartUntouchable();
			isChangingLevel = 0;
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
			isChangingLevel = 0;
		}
		return;
	}
	if (isKicking == 1)
	{
		if (GetTickCount64() - kickStart > MARIO_KICK_TIME)
		{
			isKicking = 0;
		}
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

	//Power Meter calculation

	if (isRunning == 1 && isOnPlatform && abs(vx) >= MARIO_HALF_RUN_ACCEL_SPEED)
	{
		pMeter += dt / 1000.0f;
	}
	else if (isOnPlatform) //Not running => Depleting
	{
		pMeter -= dt / ((fabs(vx) < MARIO_WALKING_SPEED) ? 2500.0f : 3000.0f);
	}

	if (pMeter == 1 && pMeterMax == -1) //Trigger full pMeter
	{
		pMeterMax = GetTickCount64();

	}
	if (pMeterMax != -1 && GetTickCount64() - pMeterMax > MARIO_PMETER_MAX_TIME)
	{
		if (pMeter == 1.0f)
			pMeter = 0;
		pMeterMax = -1;
	}

	pMeter = max(0.0f, min(pMeter, 1.0f));

	// Friction when not moving and not jumping
	if (jumpCount < 1 && isHovering == 0 && isMoving == 0 && isBraking == 0)
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

	if (tailWhip != nullptr) {
		// Pass Mario's current position and facing direction (nx) to the whip
		// TailWhip's Update should use these to set its own x, y, nx
		tailWhip->UpdatePosition(this->x, this->y, this->nx); // Need to add UpdatePosition to TailWhip
		// Pass the collidable objects list for whip's collision checks
		tailWhip->Update(dt, coObjects);
	}

	//DebugOutTitle(L"vx=%f, ax=%f, mvx=%f, irn=%d, fx=%f, iop=%d, imv=%d\n",
	//	vx, ax, maxVx, isRunning, frictionX, isOnPlatform, isMoving);

	// Process collisions
	isOnPlatform = false;
	CCollision::GetInstance()->Process(this, dt, coObjects);
}

void CMario::HandleBraking(DWORD dt)
{
	if (isBraking)
	{
		float brakeForce = MARIO_DECELERATION_X;
		if (vxBeforeBraking > 0)
		{
			ax = -brakeForce; // Apply deceleration to the left
			vx += ax * dt;   // Update velocity based on deceleration
			if (vx <= 0.0f)  // Stop when velocity reaches zero or reverses
			{
				vx = 0.0f;    // Full stop at standstill
				isBraking = 0;
				nx = -1;      // Face opposite direction
				maxVx = -MARIO_MAX_WALKING_SPEED;
				ax = 0.0f;    // Reset acceleration
			}
		}
		else if (vxBeforeBraking < 0)
		{
			ax = brakeForce;  // Apply deceleration to the right
			vx += ax * dt;   // Update velocity based on deceleration
			if (vx >= 0.0f)  // Stop when velocity reaches zero or reverses
			{
				vx = 0.0f;    // Full stop at standstill
				isBraking = 0;
				nx = 1;       // Face opposite direction
				maxVx = MARIO_MAX_WALKING_SPEED;
				ax = 0.0f;    // Reset acceleration
			}
		}
	}
	else
	{
		ax = 0.0f; // Reset acceleration when not braking
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
		if (fabs(vx) > MARIO_MAX_WALKING_SPEED / 2)
		{
			vx = (nx > 0) ? MARIO_MAX_WALKING_SPEED / 2 : -MARIO_MAX_WALKING_SPEED / 2;
		}
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
		consecutiveEnemies = 0;
	}
	else
	{
		if (e->nx != 0 && e->obj->IsBlocking())
		{
			if (vx > MARIO_WALKING_SPEED)
				vx = MARIO_WALKING_SPEED;
			else if (vx < -MARIO_WALKING_SPEED)
				vx = -MARIO_WALKING_SPEED;
		}
	}

	if (dynamic_cast<CGoomba*>(e->obj))
		OnCollisionWithGoomba(e);
	else if (dynamic_cast<CCoin*>(e->obj))
		OnCollisionWithCoin(e);
	else if (dynamic_cast<CPortal*>(e->obj))
		OnCollisionWithPortal(e);
	else if (dynamic_cast<CLifeMushroom*>(e->obj))
		OnCollisionWithLifeMushroom(e);
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
	else if (dynamic_cast<CLifeBrick*>(e->obj))
		OnCollisionWithLifeBrick(e);
	else if (dynamic_cast<CFireball*>(e->obj))
		OnCollisionWithFireball(e);
	else if (dynamic_cast<CKoopa*>(e->obj))
		OnCollisionWithKoopa(e);
	else if (dynamic_cast<CWingedGoomba*>(e->obj))
		OnCollisionWithWingedGoomba(e);
	else if (dynamic_cast<CFallPitch*>(e->obj))
		OnCollisionWithFallPitch(e);
}

void CMario::OnCollisionWithGoomba(LPCOLLISIONEVENT e)
{
	CGoomba* g = dynamic_cast<CGoomba*>(e->obj);
	if (g->IsDead() == 1) return;

	// jump on top >> kill Goomba and deflect a bit 
	if (e->ny < 0)
	{
		if (g->GetState() != GOOMBA_STATE_DIE_ON_STOMP)
		{
			g->SetState(GOOMBA_STATE_DIE_ON_STOMP);
			vy = -MARIO_JUMP_DEFLECT_SPEED;
			CalculateScore(g);
		}
	}
	else // hit by Goomba
	{
		if (untouchable == 0)
		{
			if (g->GetState() != GOOMBA_STATE_DIE_ON_STOMP)
			{
				if (level == MARIO_LEVEL_BIG)
				{
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

				if (e->nx != 0 && this->isRunning == 1)
				{
					float eVx, eVy;
					e->obj->GetSpeed(eVx, eVy);
					e->obj->SetSpeed(-eVx, eVy);
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
		if (c->GetType() == 0) c->Activate();
	}
}

void CMario::OnCollisionWithPortal(LPCOLLISIONEVENT e)
{
	CPortal* p = (CPortal*)e->obj;
	CGame::GetInstance()->InitiateSwitchScene(p->GetSceneId());
}

void CMario::OnCollisionWithCoinQBlock(LPCOLLISIONEVENT e)
{
	CCoinQBlock* cqb = dynamic_cast<CCoinQBlock*>(e->obj);
	if (cqb)
	{
		if (e->ny > 0 && e->nx == 0 && !cqb->GetIsHit() && cqb->GetState() == QUESTIONBLOCK_STATE_NOT_HIT)
		{
			cqb->SetState(QUESTIONBLOCK_STATE_BOUNCE_UP);
		}
	}
}

void CMario::OnCollisionWithBuffQBlock(LPCOLLISIONEVENT e)
{
	CBuffQBlock* bqb = dynamic_cast<CBuffQBlock*>(e->obj);
	if (bqb)
	{
		if (e->ny > 0 && e->nx == 0 && !bqb->GetIsHit() && bqb->GetState() == QUESTIONBLOCK_STATE_NOT_HIT)
		{
			if (level == MARIO_LEVEL_SMALL)
			{
				CMushroom* mr = bqb->GetMushroom();
				if (mr)
				{
					float mX, mY;
					mr->GetPosition(mX, mY);
					if (x < mX)
						mr->SetCollisionNx(1);
					else
						mr->SetCollisionNx(-1);
				}
				bqb->SetToSpawn(0);
			}
			else
				bqb->SetToSpawn(1);
			bqb->SetState(QUESTIONBLOCK_STATE_BOUNCE_UP);
		}
	}
}

void CMario::OnCollisionWithLifeBrick(LPCOLLISIONEVENT e)
{
	CLifeBrick* lb = dynamic_cast<CLifeBrick*>(e->obj);
	if (lb)
	{
		if (e->ny > 0 && e->nx == 0 && !lb->GetIsHit() && lb->GetState() == QUESTIONBLOCK_STATE_NOT_HIT)
		{
			lb->SetState(QUESTIONBLOCK_STATE_BOUNCE_UP);
		}
	}
}

void CMario::OnCollisionWithLifeMushroom(LPCOLLISIONEVENT e)
{
	CLifeMushroom* lmr = dynamic_cast<CLifeMushroom*>(e->obj);

	if (lmr)
	{
		if (lmr->GetState() == MUSHROOM_STATE_MOVING)
		{
			lmr->Activate();
			CalculateScore(lmr);
			lmr->Delete();
		}
	}
}

void CMario::OnCollisionWithFallPitch(LPCOLLISIONEVENT e)
{
	this->SetState(MARIO_STATE_DIE);
}

void CMario::OnCollisionWithMushroom(LPCOLLISIONEVENT e)
{
	CMushroom* mushroom = dynamic_cast<CMushroom*>(e->obj);

	if (mushroom)
	{
		if (mushroom->GetState() == MUSHROOM_STATE_MOVING)
		{
			CalculateScore(mushroom);
			mushroom->Delete();

			if (level == MARIO_LEVEL_SMALL)
				this->SetState(MARIO_STATE_POWER_UP);
		}
	}
}

void CMario::OnCollisionWithSuperLeaf(LPCOLLISIONEVENT e)
{
	CSuperLeaf* sl = dynamic_cast<CSuperLeaf*>(e->obj);

	if (sl)
	{
		if (sl->GetState() == SUPERLEAF_STATE_FLOATING_RIGHT || sl->GetState() == SUPERLEAF_STATE_FLOATING_LEFT)
		{
			CalculateScore(sl);
			sl->Delete();
			if (level == MARIO_LEVEL_BIG)
				this->SetState(MARIO_STATE_TAIL_UP);
		}
	}
}

void CMario::OnCollisionWithPiranhaPlant(LPCOLLISIONEVENT e)
{
	CPiranhaPlant* pp = dynamic_cast<CPiranhaPlant*>(e->obj);
	if (untouchable == 0)
	{
		if (pp->GetState() != PIRANHA_PLANT_STATE_HIDDEN && pp->GetState() != PIRANHA_PLANT_STATE_DIED)
		{
			if (level == MARIO_LEVEL_TAIL)
			{
				SetState(MARIO_STATE_TAIL_DOWN);
				StartUntouchable();
			}
			else if (level > MARIO_LEVEL_SMALL)
			{
				SetState(MARIO_STATE_POWER_DOWN);
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
	CFireball* fb = dynamic_cast<CFireball*>(e->obj);
	if (untouchable == 0)
	{
		if (fb->GetState() != FIREBALL_STATE_STATIC)
		{
			if (level == MARIO_LEVEL_TAIL)
			{
				SetState(MARIO_STATE_TAIL_DOWN);
				StartUntouchable();
			}
			else if (level == MARIO_LEVEL_BIG)
			{
				SetState(MARIO_STATE_POWER_DOWN);
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

void CMario::OnCollisionWithKoopa(LPCOLLISIONEVENT e) {
	// Early exit if Mario is invulnerable

	CKoopa* k = dynamic_cast<CKoopa*>(e->obj);
	if (k->IsDefeated() == 1)
		return;

	// Jumped on top
	if (e->ny < 0) {
		if (k->IsHeld() == 0) CalculateScore(k);
		if (k->GetState() == KOOPA_STATE_WALKING_LEFT ||
			k->GetState() == KOOPA_STATE_WALKING_RIGHT) {
			k->StartShell();
			vy = -MARIO_JUMP_DEFLECT_SPEED;
			return;
		}
		if (k->GetState() == KOOPA_STATE_SHELL_DYNAMIC) {
			k->SetState(KOOPA_STATE_SHELL_STATIC);
			k->StartShell();
			vy = -MARIO_JUMP_DEFLECT_SPEED;
			return;
		}
	}

	//if (e->ny > 0) { //Shell fell on mario
	//	return; //Processed in Koopa.cpp
	//}
	if (k->GetState() == KOOPA_STATE_SHELL_STATIC) {
		// Kick the shell
		if (isRunning == 0)
		{
			k->SetSpeed(0, 0);
			StartKick();
			k->SetState(KOOPA_STATE_SHELL_DYNAMIC);
			k->SetSpeed((nx > 0) ? KOOPA_SHELL_SPEED : -KOOPA_SHELL_SPEED, 0);
			k->SetNx(nx);
			return;
		}
		else
		{
			k->SetState(KOOPA_STATE_BEING_HELD);
			k->SetSpeed(0, 0);
			this->SetIsHoldingKoopa(1);
		}
		return;
	}
	// Side collision with walking or moving shell
	if (k->GetState() == KOOPA_STATE_WALKING_LEFT ||
		k->GetState() == KOOPA_STATE_WALKING_RIGHT ||
		k->GetState() == KOOPA_STATE_SHELL_DYNAMIC) {
		if (untouchable) return;
		if (level == MARIO_LEVEL_TAIL)
		{
			SetState(MARIO_STATE_TAIL_DOWN);
			StartUntouchable();
		}
		else if (level > MARIO_LEVEL_SMALL)
		{
			SetState(MARIO_STATE_POWER_DOWN);
			StartUntouchable();
		}
		else
		{
			DebugOut(L">>> Mario DIE >>> \n");
			SetState(MARIO_STATE_DIE);
		}
	}
}

void CMario::OnCollisionWithWingedGoomba(LPCOLLISIONEVENT e)
{
	CWingedGoomba* wg = dynamic_cast<CWingedGoomba*>(e->obj);
	if (wg->IsDead() == 1) return;

	// jump on top >> kill Goomba and deflect a bit 
	if (e->ny < 0)
	{
		CalculateScore(wg);
		if (wg->IsWinged() == 1)
		{
			wg->SetIsWinged(0);
			wg->SetState(WINGED_GOOMBA_STATE_WALKING);
			vy = -MARIO_JUMP_DEFLECT_SPEED;
			return;
		}
		if (wg->GetState() != WINGED_GOOMBA_STATE_DIE_ON_STOMP)
		{
			wg->SetState(WINGED_GOOMBA_STATE_DIE_ON_STOMP);
			vy = -MARIO_JUMP_DEFLECT_SPEED;
		}
	}
	else // hit by Goomba
	{
		if (untouchable == 0)
		{
			if (wg->GetState() != WINGED_GOOMBA_STATE_DIE_ON_STOMP)
			{
				if (level == MARIO_LEVEL_BIG)
				{
					SetState(MARIO_STATE_POWER_DOWN);
				}
				else if (level == MARIO_LEVEL_TAIL)
				{
					SetState(MARIO_STATE_TAIL_DOWN);
				}
				else
				{
					SetState(MARIO_STATE_DIE);
					DebugOut(L">>> Mario DIE >>> \n");
				}

				if (e->nx != 0 && this->isRunning == 1 && wg->IsWinged() == 0)
				{
					float eVx, eVy;
					e->obj->GetSpeed(eVx, eVy);
					e->obj->SetSpeed(-eVx, eVy);
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
		if (pMeter == 1.0f)
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
	else if (isKicking == 1)
	{
		if (nx > 0) aniId = ID_ANI_MARIO_SMALL_KICK_RIGHT;
		else aniId = ID_ANI_MARIO_SMALL_KICK_LEFT;
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
		else if (fabs(vx) == MARIO_MAX_RUNNING_SPEED && pMeter == 1.0f)
			aniId = ID_ANI_MARIO_SMALL_RUNNING_RIGHT;
		else if (fabs(vx) > MARIO_HALF_RUN_ACCEL_SPEED && fabs(vx) <= MARIO_MAX_RUNNING_SPEED)
			aniId = ID_ANI_MARIO_SMALL_HALF_RUN_ACCEL_RIGHT;
	}
	else // vx < 0
	{
		if (isBraking)
			aniId = ID_ANI_MARIO_SMALL_BRACE_LEFT;
		else if (fabs(vx) <= MARIO_HALF_RUN_ACCEL_SPEED)
			aniId = ID_ANI_MARIO_SMALL_WALKING_LEFT;
		else if (fabs(vx) == MARIO_MAX_RUNNING_SPEED && pMeter == 1.0f)
			aniId = ID_ANI_MARIO_SMALL_RUNNING_LEFT;
		else if (fabs(vx) > MARIO_HALF_RUN_ACCEL_SPEED && fabs(vx) <= MARIO_MAX_RUNNING_SPEED)
			aniId = ID_ANI_MARIO_SMALL_HALF_RUN_ACCEL_LEFT;
	}

	if (powerUp)
	{
		if (nx > 0) aniId = ID_ANI_MARIO_SMALL_POWER_UP_RIGHT;
		else aniId = ID_ANI_MARIO_SMALL_POWER_UP_LEFT;
	}

	if (isHoldingKoopa)
	{
		if (vx == 0)
		{
			if (nx > 0) aniId = ID_ANI_MARIO_SMALL_HOLDING_KOOPA_IDLE_RIGHT;
			else aniId = ID_ANI_MARIO_SMALL_HOLDING_KOOPA_IDLE_LEFT;
		}
		else if (nx > 0)
		{
			if (fabs(vx) <= MARIO_RUNNING_SPEED)
				aniId = ID_ANI_MARIO_SMALL_HOLDING_KOOPA_WALKING_RIGHT;
			else if (fabs(vx) > MARIO_RUNNING_SPEED && fabs(vx) <= MARIO_MAX_RUNNING_SPEED)
				aniId = ID_ANI_MARIO_SMALL_HOLDING_KOOPA_RUNNING_RIGHT;
			else if (fabs(vx) == MARIO_MAX_RUNNING_SPEED && pMeter == 1.0f)
				aniId = ID_ANI_MARIO_SMALL_HOLDING_KOOPA_MAX_RUNNING_RIGHT;
		}
		else if (nx < 0)
		{
			if (fabs(vx) <= MARIO_RUNNING_SPEED)
				aniId = ID_ANI_MARIO_SMALL_HOLDING_KOOPA_WALKING_LEFT;
			else if (fabs(vx) > MARIO_RUNNING_SPEED && fabs(vx) <= MARIO_MAX_RUNNING_SPEED)
				aniId = ID_ANI_MARIO_SMALL_HOLDING_KOOPA_RUNNING_LEFT;
			else if (fabs(vx) == MARIO_MAX_RUNNING_SPEED && pMeter == 1.0f)
				aniId = ID_ANI_MARIO_SMALL_HOLDING_KOOPA_MAX_RUNNING_LEFT;
		}
	}

	if (aniId == -1) aniId = ID_ANI_MARIO_SMALL_IDLE_RIGHT;

	return aniId;
}


//
// Get animdation ID for big Mario
//
int CMario::GetAniIdBig()
{
	static int preAniId = -1;
	int aniId = -1;
	if (!isOnPlatform)
	{
		if (pMeter == 1.0f)
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
	else if (isKicking == 1)
	{
		if (nx > 0) aniId = ID_ANI_MARIO_KICK_RIGHT;
		else aniId = ID_ANI_MARIO_KICK_LEFT;
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
		else if (fabs(vx) == MARIO_MAX_RUNNING_SPEED && pMeter == 1.0f)
			aniId = ID_ANI_MARIO_RUNNING_RIGHT;
		else if (fabs(vx) > MARIO_HALF_RUN_ACCEL_SPEED && fabs(vx) <= MARIO_MAX_RUNNING_SPEED)
			aniId = ID_ANI_MARIO_HALF_RUN_ACCEL_RIGHT;
	}
	else // vx < 0
	{
		if (isBraking)
			aniId = ID_ANI_MARIO_BRACE_LEFT;
		else if (fabs(vx) <= MARIO_HALF_RUN_ACCEL_SPEED)
			aniId = ID_ANI_MARIO_WALKING_LEFT;
		else if (fabs(vx) == MARIO_MAX_RUNNING_SPEED && pMeter == 1.0f)
			aniId = ID_ANI_MARIO_RUNNING_LEFT;
		else if (fabs(vx) > MARIO_HALF_RUN_ACCEL_SPEED && fabs(vx) <= MARIO_MAX_RUNNING_SPEED)
			aniId = ID_ANI_MARIO_HALF_RUN_ACCEL_LEFT;
	}

	if (tailUp)
	{
		aniId = ID_ANI_MARIO_INVISIBLE;
		CParticle::GenerateParticleInChunk(this, 2);
	}
	if (powerDown)
	{
		if (nx > 0) aniId = ID_ANI_MARIO_POWER_DOWN_RIGHT;
		else aniId = ID_ANI_MARIO_POWER_DOWN_LEFT;
	}

	if (isHoldingKoopa)
	{
		if (vx == 0)
		{
			if (nx > 0) aniId = ID_ANI_MARIO_HOLDING_KOOPA_IDLE_RIGHT;
			else aniId = ID_ANI_MARIO_HOLDING_KOOPA_IDLE_LEFT;
		}
		else if (nx > 0)
		{
			if (fabs(vx) <= MARIO_RUNNING_SPEED)
				aniId = ID_ANI_MARIO_HOLDING_KOOPA_WALKING_RIGHT;
			else if (fabs(vx) > MARIO_RUNNING_SPEED && fabs(vx) <= MARIO_MAX_RUNNING_SPEED)
				aniId = ID_ANI_MARIO_HOLDING_KOOPA_RUNNING_RIGHT;
			else if (fabs(vx) == MARIO_MAX_RUNNING_SPEED && pMeter == 1.0f)
				aniId = ID_ANI_MARIO_HOLDING_KOOPA_MAX_RUNNING_RIGHT;
		}
		else if (nx < 0)
		{
			if (fabs(vx) <= MARIO_RUNNING_SPEED)
				aniId = ID_ANI_MARIO_HOLDING_KOOPA_WALKING_LEFT;
			else if (fabs(vx) > MARIO_RUNNING_SPEED && fabs(vx) <= MARIO_MAX_RUNNING_SPEED)
				aniId = ID_ANI_MARIO_HOLDING_KOOPA_RUNNING_LEFT;
			else if (fabs(vx) == MARIO_MAX_RUNNING_SPEED && pMeter == 1.0f)
				aniId = ID_ANI_MARIO_HOLDING_KOOPA_MAX_RUNNING_LEFT;
		}
	}

	if (preAniId != ID_ANI_MARIO_JUMP_RUN_RIGHT && preAniId != ID_ANI_MARIO_JUMP_RUN_LEFT)
		if (isOnPlatform == 0 && isHoldingKoopa == 0 && isChangingLevel == 0 && vy > 0)
			aniId = (nx > 0) ? ID_ANI_MARIO_FALLING_RIGHT : ID_ANI_MARIO_FALLING_LEFT;

	if (aniId == -1) aniId = ID_ANI_MARIO_IDLE_RIGHT;

	preAniId = aniId;

	return aniId;
}

//
// Get animation ID for tail Mario
//
int CMario::GetAniIdTail()
{
	static int preAniId = -1;
	int aniId = -1;
	if (!isOnPlatform)
	{
		if (pMeter == 1.0f)
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
	{
		if (isSitting)
		{
			if (nx > 0)
				aniId = ID_ANI_MARIO_TAIL_SIT_RIGHT;
			else
				aniId = ID_ANI_MARIO_TAIL_SIT_LEFT;
		}
		else
		{
			if (vx == 0)
			{
				if (nx > 0) aniId = ID_ANI_MARIO_TAIL_IDLE_RIGHT;
				else aniId = ID_ANI_MARIO_TAIL_IDLE_LEFT;
			}
			else if (vx > 0)
			{
				if (isBraking)
					aniId = ID_ANI_MARIO_TAIL_BRACE_RIGHT;
				else if (fabs(vx) <= MARIO_RUNNING_SPEED)
					aniId = ID_ANI_MARIO_TAIL_WALKING_RIGHT;
				else if (fabs(vx) == MARIO_MAX_RUNNING_SPEED && pMeter == 1.0f)
					aniId = ID_ANI_MARIO_TAIL_RUNNING_RIGHT;
				else if (fabs(vx) > MARIO_RUNNING_SPEED && fabs(vx) <= MARIO_MAX_RUNNING_SPEED)
					aniId = ID_ANI_MARIO_TAIL_HALF_RUN_ACCEL_RIGHT;
			}
			else // vx < 0
			{
				if (isBraking)
					aniId = ID_ANI_MARIO_TAIL_BRACE_LEFT;
				else if (fabs(vx) <= MARIO_RUNNING_SPEED)
					aniId = ID_ANI_MARIO_TAIL_WALKING_LEFT;
				else if (fabs(vx) == MARIO_MAX_RUNNING_SPEED && pMeter == 1.0f)
					aniId = ID_ANI_MARIO_TAIL_RUNNING_LEFT;
				else if (fabs(vx) > MARIO_RUNNING_SPEED && fabs(vx) <= MARIO_MAX_RUNNING_SPEED)
					aniId = ID_ANI_MARIO_TAIL_HALF_RUN_ACCEL_LEFT;
			}
		}
	}

	if (isHovering)
	{
		if (nx > 0) aniId = ID_ANI_MARIO_TAIL_HOVERING_RIGHT;
		else aniId = ID_ANI_MARIO_TAIL_HOVERING_LEFT;
	}

	if (tailDown)
	{
		aniId = ID_ANI_MARIO_INVISIBLE;
		CParticle::GenerateParticleInChunk(this, 2);
	}

	if (isTailWhipping == 1)
	{
		if (nx > 0) aniId = ID_ANI_MARIO_TAIL_WHIP_RIGHT;
		else aniId = ID_ANI_MARIO_TAIL_WHIP_LEFT;
	}

	if (isKicking == 1)
	{
		if (nx > 0) aniId = ID_ANI_MARIO_TAIL_KICK_RIGHT;
		else aniId = ID_ANI_MARIO_TAIL_KICK_LEFT;
	}

	if (jumpCount > 0)
		if (nx > 0) aniId = ID_ANI_MARIO_TAIL_MULTIJUMP_RIGHT;
		else aniId = ID_ANI_MARIO_TAIL_MULTIJUMP_LEFT;

	if (isHoldingKoopa)
	{
		if (vx == 0)
		{
			if (nx > 0) aniId = ID_ANI_MARIO_TAIL_HOLDING_KOOPA_IDLE_RIGHT;
			else aniId = ID_ANI_MARIO_TAIL_HOLDING_KOOPA_IDLE_LEFT;
		}
		else if (nx > 0)
		{
			if (fabs(vx) <= MARIO_RUNNING_SPEED)
				aniId = ID_ANI_MARIO_TAIL_HOLDING_KOOPA_WALKING_RIGHT;
			else if (fabs(vx) > MARIO_RUNNING_SPEED && fabs(vx) <= MARIO_MAX_RUNNING_SPEED)
				aniId = ID_ANI_MARIO_TAIL_HOLDING_KOOPA_RUNNING_RIGHT;
			else if (fabs(vx) == MARIO_MAX_RUNNING_SPEED && pMeter == 1.0f)
				aniId = ID_ANI_MARIO_TAIL_HOLDING_KOOPA_MAX_RUNNING_RIGHT;
		}
		else if (nx < 0)
		{
			if (fabs(vx) <= MARIO_RUNNING_SPEED)
				aniId = ID_ANI_MARIO_TAIL_HOLDING_KOOPA_WALKING_LEFT;
			else if (fabs(vx) > MARIO_RUNNING_SPEED && fabs(vx) <= MARIO_MAX_RUNNING_SPEED)
				aniId = ID_ANI_MARIO_TAIL_HOLDING_KOOPA_RUNNING_LEFT;
			else if (fabs(vx) == MARIO_MAX_RUNNING_SPEED && pMeter == 1.0f)
				aniId = ID_ANI_MARIO_TAIL_HOLDING_KOOPA_MAX_RUNNING_LEFT;
		}
	}

	if (preAniId != ID_ANI_MARIO_TAIL_JUMP_RUN_RIGHT && preAniId != ID_ANI_MARIO_TAIL_JUMP_RUN_LEFT)
		if (isOnPlatform == 0 && isHovering == 0 && isHoldingKoopa == 0 && isChangingLevel == 0 && jumpCount < 1 && vy > 0)
			aniId = (nx > 0) ? ID_ANI_MARIO_TAIL_FALLING_RIGHT : ID_ANI_MARIO_TAIL_FALLING_LEFT;

	if (aniId == -1) aniId = ID_ANI_MARIO_TAIL_IDLE_RIGHT;

	preAniId = aniId;

	return aniId;
}

void CMario::Render()
{
	if (untouchable)
	{
		ULONGLONG currentTime = GetTickCount64();
		if (lastRenderTime == (ULONGLONG)-1) lastRenderTime = currentTime;
		if (currentTime - lastRenderTime > MARIO_UNTOUCHABLE_RENDER_INTERVAL)
		{
			isRendering = !isRendering;
			lastRenderTime = currentTime;
		}
		bool isPaused = CGame::GetInstance()->IsPaused();
		if (!isRendering && !isPaused) return;
	}
	else
	{
		isRendering = true;
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

	if (tailWhip != nullptr)
	{
		tailWhip->Render();
	}

	//RenderBoundingBox();
}

// Modify the SetState method for improved braking and hovering handling
void CMario::SetState(int state)
{
	// DIE is the end state, cannot be changed! 
	if (this->state == MARIO_STATE_DIE) return;
	if (CGame::GetInstance()->IsPaused()) return;

	int previousState = this->state;

	switch (state)
	{
	case MARIO_STATE_TAIL_WHIP:
		if (level == MARIO_LEVEL_TAIL && tailWhip != nullptr && !tailWhip->IsActive())
		{
			StartTailWhip();
			if (nx > 0) tailWhip->SetState(TAIL_STATE_WHIPPING_RIGHT);
			else tailWhip->SetState(TAIL_STATE_WHIPPING_LEFT);
		}
		break;

	case MARIO_STATE_RUNNING_RIGHT:
		if (isSitting) break;
		if (isOnPlatform && vx < 0 && isHoldingKoopa == 0) // If Mario is moving left, set state to brake
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
			if (vx < 0)  // Đang đứng im hoặc đang đi sang trái trong không trung
			{
				//vx = -MARIO_RUNNING_SPEED / 5;
				ax = MARIO_DECELERATION_X;
			}
			else
				ax = MARIO_ACCEL_RUN_X;
		}
		else
		{
			ax = MARIO_ACCEL_RUN_X;
		}
		frictionX = 0;
		isMoving = 1;
		isRunning = 1;
		nx = 1;
		break;

	case MARIO_STATE_RUNNING_LEFT:
		if (isSitting) break;
		if (isOnPlatform && vx > 0 && isHoldingKoopa == 0) // If Mario is moving right, set state to brake
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
			if (vx > 0)  // Đang đứng im hoặc đang đi sang phải trong không trung
			{
				//vx = MARIO_RUNNING_SPEED / 5;
				ax = -MARIO_DECELERATION_X;
			}
			else
				ax = -MARIO_ACCEL_RUN_X;
		}
		else
		{
			ax = -MARIO_ACCEL_RUN_X;
		}
		frictionX = 0;
		nx = -1;
		isMoving = 1;
		isRunning = 1;
		break;

	case MARIO_STATE_WALKING_RIGHT:
		if (isSitting) break;
		if (isOnPlatform && vx < 0 && isHoldingKoopa == 0) // If Mario is moving left, set state to brake
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
			if (vx < 0)  // Đang đứng im hoặc đang đi sang trái trong không trung
			{
				ax = MARIO_DECELERATION_X;
			}
			else
				ax = MARIO_ACCEL_WALK_X;
		}
		else
		{
			ax = MARIO_ACCEL_WALK_X;
		}
		frictionX = 0;
		isMoving = 1;
		nx = 1;
		break;

	case MARIO_STATE_WALKING_LEFT:
		if (isSitting) break;
		if (isOnPlatform && vx > 0 && isHoldingKoopa == 0) // If Mario is moving right, set state to brake
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
			if (vx > 0)  // Đang đứng im hoặc đang đi sang phải trong không trung
			{
				ax = -MARIO_DECELERATION_X;
			}
			else
				ax = -MARIO_ACCEL_WALK_X;
		}
		else
		{
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
			if (pMeter == 1.0f)
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

			if (fabs(vx) == MARIO_MAX_RUNNING_SPEED && pMeter == 1.0f)
			{
				jumpCount++;
				vy = -MARIO_JUMP_SPEED_Y;
			}
			else if (jumpCount >= 1)
			{
				jumpCount++;
				vy = -MARIO_JUMP_SPEED_Y;
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
		if (isHoldingKoopa == 1) break;
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

	case MARIO_STATE_DIE_ON_FALLING:
		vy = 0;
		vx = 0;
		ax = 0;
		ay = 0;
		break;

	case MARIO_STATE_POWER_UP:
		StartPowerUp();
		break;

	case MARIO_STATE_TAIL_UP:
		StartTailUp();
		break;

	case MARIO_STATE_POWER_DOWN:
		y += 6; // RED ALERT
		//vx = 0;
		StartPowerDown();
		break;

	case MARIO_STATE_TAIL_DOWN:
		StartTailDown();
		//vx = 0;
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
		if (!isOnPlatform)
		{
			frictionX = MARIO_FRICTION_X * 1.75;
		}
		else if (fabs(vx) <= MARIO_MAX_WALKING_SPEED)
		{
			frictionX = MARIO_FRICTION_X * 3.75;
		}
		else
			frictionX = MARIO_FRICTION_X * 2.5;
		break;
	case MARIO_STATE_RELEASE_RUN:
		isRunning = 0;
		break;
	}

	CGameObject::SetState(state);
}

void CMario::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	if (level == MARIO_LEVEL_BIG)
	{
		if (isSitting)
		{
			left = x - MARIO_BIG_SITTING_BBOX_WIDTH / 2;
			top = y - MARIO_BIG_SITTING_BBOX_HEIGHT / 2;
			right = left + MARIO_BIG_SITTING_BBOX_WIDTH - 3;
			bottom = top + MARIO_BIG_SITTING_BBOX_HEIGHT;
		}
		else
		{
			left = x - MARIO_BIG_BBOX_WIDTH / 2;
			top = y - MARIO_BIG_BBOX_HEIGHT / 2;
			right = left + MARIO_BIG_BBOX_WIDTH - 3;
			bottom = top + MARIO_BIG_BBOX_HEIGHT;
		}
	}
	else if (level == MARIO_LEVEL_TAIL)
	{
		if (isSitting)
		{
			left = x - MARIO_TAIL_SITTING_BBOX_WIDTH / 2 - 1;
			top = y - MARIO_TAIL_SITTING_BBOX_HEIGHT / 2;
			right = left + MARIO_TAIL_SITTING_BBOX_WIDTH + 1;
			bottom = top + MARIO_TAIL_SITTING_BBOX_HEIGHT;
		}
		else
		{
			left = x - MARIO_TAIL_BBOX_WIDTH / 2 - 1;
			top = y - MARIO_TAIL_BBOX_HEIGHT / 2;
			right = left + MARIO_TAIL_BBOX_WIDTH + 1;
			bottom = top + MARIO_TAIL_BBOX_HEIGHT;
		}
	}
	else
	{
		left = x - MARIO_SMALL_BBOX_WIDTH / 2;
		top = y - MARIO_SMALL_BBOX_HEIGHT / 2;
		right = left + MARIO_SMALL_BBOX_WIDTH - 4;
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

void CMario::StartTailWhip() {
	isTailWhipping = 1;
	tailWhipStart = GetTickCount64();
}

CTailWhip* CMario::GetActiveTailWhip() {
	if (tailWhip != nullptr && tailWhip->IsActive()) {
		return tailWhip;
	}
	return nullptr;
}

int EnemiesToPoints(int enemies)
{
	switch (enemies)
	{
	case 2:
		return 200;
	case 3:
		return 400;
	case 4:
		return 800;
	case 5:
		return 1000;
	case 6:
		return 2000;
	case 7:
		return 4000;
	case 8:
		return 8000;
	case 1:
	default:
		return 100;
	}
}

void CMario::CalculateScore(LPGAMEOBJECT obj)
{
	if (obj == nullptr || dynamic_cast<CMario*>(obj)) return;
	int point = 0;
	int type = 0;
	if (dynamic_cast<CGoomba*>(obj)
		|| dynamic_cast<CKoopa*>(obj)
		|| dynamic_cast<CWingedGoomba*>(obj)
		|| dynamic_cast<CPiranhaPlant*>(obj))
	{
		consecutiveEnemies += 1;
		point = EnemiesToPoints(consecutiveEnemies);
	}
	else if (dynamic_cast<CLifeMushroom*>(obj))
	{
		type = 1;
	}
	else point = 1000;
	CGame::GetInstance()->GetGameState()->AddScore(point);
	CParticle::GenerateParticleInChunk(obj, type, point);
}