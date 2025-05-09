#include "WingedGoomba.h"
#include "Mario.h"
#include "Koopa.h"
#include "PlayScene.h"

#include "debug.h"

CWingedGoomba::CWingedGoomba(int id, float x, float y, int z) : CGameObject(id, x, y, z)
{
	this->ax = 0;
	this->ay = WINGED_GOOMBA_GRAVITY;
	SetState(WINGED_GOOMBA_STATE_TRACKING);
}

void CWingedGoomba::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	if (state == WINGED_GOOMBA_STATE_DIE_ON_STOMP)
	{
		left = x - WINGED_GOOMBA_BBOX_WIDTH / 2;
		top = y - WINGED_GOOMBA_BBOX_HEIGHT_DIE / 2;
		right = left + WINGED_GOOMBA_BBOX_WIDTH;
		bottom = top + WINGED_GOOMBA_BBOX_HEIGHT_DIE;
	}
	else
	{
		left = x - WINGED_GOOMBA_BBOX_WIDTH / 2;
		top = y - WINGED_GOOMBA_BBOX_HEIGHT / 2;
		right = left + WINGED_GOOMBA_BBOX_WIDTH;
		bottom = top + WINGED_GOOMBA_BBOX_HEIGHT;
	}
}

void CWingedGoomba::OnNoCollision(DWORD dt)
{
	x += vx * dt;
	y += vy * dt;
};

void CWingedGoomba::OnCollisionWith(LPCOLLISIONEVENT e)
{
	if (e->obj->IsBlocking())
	{
		if (e->ny != 0) // Vertical collision with blocking object
		{
			// Handle landing/bouncing logic based on current state
			if (isBouncing == 1)
			{
				if (e->ny < 0)
				{
					vy = -WINGED_GOOMBA_BOUNCE_SPEED;
					bounceCount++;
				}
				else {
					vy = 0;
				}
			}
			else if (isFlying == 1)
			{
				if (e->ny < 0) // Landed while flying
				{
					vy = 0;
					isFlying = 0;
					SetState(WINGED_GOOMBA_STATE_TRACKING); // Or WINGED_GOOMBA_STATE_WALKING?
				}
				else
				{
					vy = 0;
				}
			}
			else
			{
				if (e->ny < 0)
				{
					vy = 0;
				}
				else
				{
					vy = 0;
				}
			}
		}
		else if (e->nx != 0)
		{
			vx = -vx;
			nx = -nx;
		}
	}

	if (dynamic_cast<CGoomba*>(e->obj) || dynamic_cast<CWingedGoomba*>(e->obj))
	{
		do
		{
			if (this->isWinged == 1)
				break;

			if (CWingedGoomba* wg = dynamic_cast<CWingedGoomba*>(e->obj))
				if (wg->GetIsWinged() == 1)
					break;

			if (e->nx != 0)
			{
				vx = -vx; // Bounce off another Goomba
				float eVx, eVy;
				e->obj->GetSpeed(eVx, eVy);
				e->obj->SetSpeed(-eVx, eVy);
			}
		} while (false);
	}

	if (dynamic_cast<CKoopa*>(e->obj))
		OnCollisionWithKoopa(e);
}

void CWingedGoomba::OnCollisionWithKoopa(LPCOLLISIONEVENT e)
{
	if (e->obj->GetState() == KOOPA_STATE_SHELL_DYNAMIC)
	{
		if (this->isDead == 0)
		{
			this->SetState(WINGED_GOOMBA_STATE_DIE_ON_TAIL_WHIP);
		}
	}
	else
	{
		if (e->nx != 0 && isWinged == 0)
		{
			vx = -vx;
			nx = -nx;
			CKoopa* k = dynamic_cast<CKoopa*>(e->obj);
			float eVx, eVy;
			k->GetSpeed(eVx, eVy);
			k->SetState(eVx > 0 ? KOOPA_STATE_WALKING_LEFT : KOOPA_STATE_WALKING_RIGHT);
		}
	}
}

void CWingedGoomba::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{

	CMario* player = (CMario*)((LPPLAYSCENE)CGame::GetInstance()->GetCurrentScene())->GetPlayer();
	float mX, mY;
	player->GetPosition(mX, mY);
	if (fabs(x - mX) >= 200)
		return;

	if (isTracking)
	{
		if (GetTickCount64() - trackingStart > WINGED_GOOMBA_TRACKING_TIME)
		{
			isTracking = 0;
			SetState(WINGED_GOOMBA_STATE_BOUNCING);
		}
	}
	if (isBouncing)
	{

		if (bounceCount == WINGED_GOOMBA_MAX_BOUNCE_COUNT)
		{
			isBouncing = 0;
			bounceCount = 0;
			SetState(WINGED_GOOMBA_STATE_FLYING);
		}
	}

	vy += ay * dt;
	vx += ax * dt;

	if (isDead == 1 && (GetTickCount64() - dieStart > GOOMBA_DIE_TIMEOUT))
	{
		isDeleted = true;
		return;
	}

	if (isWinged == 1) {
		int flapInterval = 500;
		if (isBouncing == 1) flapInterval = 1000;
		else if (isFlying == 1) flapInterval = 500;
		if (GetTickCount64() - flapStart > flapInterval) {
			flapStart = GetTickCount64();
			wingState = 1 - wingState;
		}
		DebugOut(L"[WINGED_GOOMBA] wingState=%d, flapInterval=%d, timeSinceFlap=%llu\n",
			wingState, flapInterval, GetTickCount64() - flapStart);
	}



	CGameObject::Update(dt, coObjects);
	CCollision::GetInstance()->Process(this, dt, coObjects);
}

void CWingedGoomba::Render()
{
	int aniId = ID_ANI_WINGED_GOOMBA_WALKING;
	if (state == WINGED_GOOMBA_STATE_DIE_ON_STOMP)
	{
		aniId = ID_ANI_WINGED_GOOMBA_DIE_ON_STOMP;
	}
	else if (state == WINGED_GOOMBA_STATE_DIE_ON_TAIL_WHIP)
	{
		aniId = ID_ANI_WINGED_GOOMBA_DIE_ON_TAIL_WHIP;
	}



	CAnimations::GetInstance()->Get(aniId)->Render(x, y);
	if (isWinged == 1)
	{
		if (wingState == 0)
		{
			CAnimations::GetInstance()->Get(ID_ANI_WING_FLAP_LEFT)->Render(x - GOOMBA_BBOX_WIDTH / 2 + 1, y - GOOMBA_BBOX_HEIGHT / 2);
			CAnimations::GetInstance()->Get(ID_ANI_WING_FLAP_RIGHT)->Render(x + GOOMBA_BBOX_WIDTH / 2, y - GOOMBA_BBOX_HEIGHT / 2);
		}
		else
		{
			CAnimations::GetInstance()->Get(ID_ANI_WING_CLOSE_LEFT)->Render(x - GOOMBA_BBOX_WIDTH / 2 + 1, y - GOOMBA_BBOX_HEIGHT / 2 + 2);
			CAnimations::GetInstance()->Get(ID_ANI_WING_CLOSE_RIGHT)->Render(x + GOOMBA_BBOX_WIDTH / 2, y - GOOMBA_BBOX_HEIGHT / 2 + 2);
		}
	}

	//RenderBoundingBox();
}

void CWingedGoomba::SetState(int state)
{
	if (isDead == 1) return;

	CGameObject::SetState(state);
	switch (state)
	{
	case WINGED_GOOMBA_STATE_DIE_ON_STOMP:
		dieStart = GetTickCount64();
		y += (WINGED_GOOMBA_BBOX_HEIGHT - WINGED_GOOMBA_BBOX_HEIGHT_DIE) / 2 - 3;
		vx = 0;
		vy = 0;
		ay = 0;
		isDead = 1;
		break;
	case WINGED_GOOMBA_STATE_DIE_ON_TAIL_WHIP:
		dieStart = GetTickCount64();
		vy = -0.5f;
		ay = WINGED_GOOMBA_GRAVITY;
		isDead = 1;
		break;
	case WINGED_GOOMBA_STATE_DIE_ON_HELD_KOOPA:
		dieStart = GetTickCount64();
		vy = -0.35f;
		ay = WINGED_GOOMBA_GRAVITY;
		isDead = 1;
		break;
	case WINGED_GOOMBA_STATE_WALKING:
		vx = -WINGED_GOOMBA_WALKING_SPEED;
		vy = 0;
		break;
	case WINGED_GOOMBA_STATE_TRACKING:
	{
		if (isWinged == 0) break;
		CMario* player = (CMario*)((LPPLAYSCENE)CGame::GetInstance()->GetCurrentScene())->GetPlayer();
		float mX, mY;
		player->GetPosition(mX, mY);
		vx = (x < mX) ? WINGED_GOOMBA_WALKING_SPEED : -WINGED_GOOMBA_WALKING_SPEED;
		nx = (x < mX) ? 1 : -1;
		vy = 0;
		ax = 0;
		ay = WINGED_GOOMBA_GRAVITY;
		wingState = 1;
		StartTracking();
		break;
	}
	case WINGED_GOOMBA_STATE_BOUNCING:
		if (isWinged == 0) break;
		vx = (nx > 0) ? WINGED_GOOMBA_WALKING_SPEED : -WINGED_GOOMBA_WALKING_SPEED;
		ay = WINGED_GOOMBA_GRAVITY;
		vy = -WINGED_GOOMBA_BOUNCE_SPEED;
		wingState = 0;
		flapStart = GetTickCount64();
		StartBouncing();
		break;
	case WINGED_GOOMBA_STATE_FLYING:
		if (isWinged == 0) break;
		vx = (nx > 0) ? WINGED_GOOMBA_WALKING_SPEED : -WINGED_GOOMBA_WALKING_SPEED;
		vy = -0.35f;
		ax = 0;
		ay = WINGED_GOOMBA_GRAVITY;
		flapStart = GetTickCount64();
		StartFlying();
		break;
	}

	//DebugOut(L"[WINGED_GOOMBA] SetState: %i\n", state);
}
