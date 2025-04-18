#include "Koopa.h"
#include "debug.h"

CKoopa::CKoopa(float x, float y) :CGameObject(x, y)
{
	this->ax = 0;
	this->ay = KOOPA_GRAVITY;
	shell_start = -1;
	SetState(KOOPA_STATE_WALKING_LEFT);
}

void CKoopa::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	if (state == KOOPA_STATE_WALKING_LEFT || state == KOOPA_STATE_WALKING_RIGHT)
	{
		left = x - KOOPA_BBOX_WIDTH / 2;
		right = left + KOOPA_BBOX_WIDTH;

		// Set bottom at the sprite's bottom
		bottom = y + KOOPA_TEXTURE_HEIGHT / 2;

		// Set top 16 pixels up from bottom
		top = bottom - KOOPA_BBOX_HEIGHT;
	}
	else // Shell states
	{
		left = x - KOOPA_BBOX_WIDTH / 2;
		top = y - KOOPA_BBOX_HEIGHT / 2;
		right = left + KOOPA_BBOX_WIDTH;
		bottom = top + KOOPA_BBOX_HEIGHT;
	}
}

void CKoopa::OnNoCollision(DWORD dt)
{
	x += vx * dt;
	y += vy * dt;
}

void CKoopa::OnCollisionWith(LPCOLLISIONEVENT e) {
	// Skip collision with non-blocking objects or self
	if (!e->obj->IsBlocking() || e->obj == this) return;

	// Ground check and tracking
	if (e->ny < 0) { // Collision from above (standing on something)
		vy = 0;
		if (ground == nullptr
			|| dynamic_cast<CPlatform*>(e->obj)
			|| dynamic_cast<CBox*>(e->obj))
			ground = e->obj;
	}// Remember what we're standing on

	// Handle other collisions
	if (e->nx != 0) {
		// Wall collision while walking
		if (state == KOOPA_STATE_WALKING_LEFT || state == KOOPA_STATE_WALKING_RIGHT) {
			SetState(state == KOOPA_STATE_WALKING_LEFT ?
				KOOPA_STATE_WALKING_RIGHT :
				KOOPA_STATE_WALKING_LEFT);
		}
		// Shell bounce
		else if (state == KOOPA_STATE_SHELL_DYNAMIC) {
			vx = -vx; // Reverse direction
		}
	}
}

bool CKoopa::IsPlatformEdge(float checkDistance)
{
	if (ground == nullptr) return false;

	float groundL, groundT, groundR, groundB;
	ground->GetBoundingBox(groundL, groundT, groundR, groundB);

	// Check if there's ground ahead in walking direction
	float checkX = (state == KOOPA_STATE_WALKING_LEFT)
		? x - KOOPA_BBOX_WIDTH / 2 - checkDistance
		: x + KOOPA_BBOX_WIDTH / 2 + checkDistance;

	return (checkX < groundL || checkX > groundR);
}

void CKoopa::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	vy += ay * dt;
	vx += ax * dt;

	if (state == KOOPA_STATE_WALKING_LEFT)
	{
		if (IsPlatformEdge(5.0f))
			SetState(KOOPA_STATE_WALKING_RIGHT);
	}
	else if (state == KOOPA_STATE_WALKING_RIGHT)
	{
		if (IsPlatformEdge(5.0f))
			SetState(KOOPA_STATE_WALKING_LEFT);
	}

	if (state == KOOPA_STATE_SHELL_STATIC && !isKicked && (GetTickCount64() - shell_start > KOOPA_SHELL_TIMEOUT))
	{
		SetState(KOOPA_STATE_WALKING_LEFT);
		return;
	}

	//DebugOut(L"[INFO] Koopa: vx=%f, ax=%f, vy=%f, ay=%f\n", vx, ax, vy, ay);
	//DebugOut(L"[INFO] Koopa: x=%f, y=%f\n", x, y);

	CGameObject::Update(dt, coObjects);
	CCollision::GetInstance()->Process(this, dt, coObjects);
}

void CKoopa::StartShell()
{
	shell_start = GetTickCount64();
	isKicked = false;
	SetState(KOOPA_STATE_SHELL_STATIC);
}


void CKoopa::Render()
{
	int aniId;
	if (state == KOOPA_STATE_SHELL_STATIC)
	{
		if (!isKicked && GetTickCount64() - shell_start > KOOPA_SHELL_ALERT_TIMEOUT)
		{
			if (isReversed)
				aniId = ID_ANI_KOOPA_SHELL_STATIC_REVERSE_2;
			else
				aniId = ID_ANI_KOOPA_SHELL_STATIC_2;
		}
		else
		{
			if (isReversed)
				aniId = ID_ANI_KOOPA_SHELL_STATIC_REVERSE_1;
			else
				aniId = ID_ANI_KOOPA_SHELL_STATIC_1;
		}
	}
	else if (state == KOOPA_STATE_SHELL_DYNAMIC)
	{
		if (isReversed)
			aniId = ID_ANI_KOOPA_SHELL_DYNAMIC_REVERSE;
		else
			aniId = ID_ANI_KOOPA_SHELL_DYNAMIC;
	}
	else if (state == KOOPA_STATE_WALKING_LEFT)
		aniId = ID_ANI_KOOPA_WALKING_LEFT;
	else aniId = ID_ANI_KOOPA_WALKING_RIGHT;

	CAnimations::GetInstance()->Get(aniId)->Render(x, y);
	RenderBoundingBox();
}
void CKoopa::SetState(int state)
{
	// If transitioning from shell to walking state
	if ((this->state == KOOPA_STATE_SHELL_STATIC || this->state == KOOPA_STATE_SHELL_DYNAMIC) &&
		(state == KOOPA_STATE_WALKING_LEFT || state == KOOPA_STATE_WALKING_RIGHT))
	{
		// Adjust y position to account for hitbox difference
		// In shell: hitbox is centered (height/2 above and below y)
		// In walking: hitbox extends from bottom up
		float heightDifference = (KOOPA_TEXTURE_HEIGHT - KOOPA_BBOX_HEIGHT) / 2;
		y -= heightDifference; // Move Koopa up to compensate
	}

	switch (state)
	{
	case KOOPA_STATE_SHELL_STATIC:
		vx = 0;
		vy = 0;
		ax = 0;
		break;
	case KOOPA_STATE_SHELL_DYNAMIC:
		isKicked = true;
		vy = 0;
		ax = 0;
		break;
	case KOOPA_STATE_WALKING_LEFT:
		isKicked = false;
		vx = -KOOPA_WALKING_SPEED;
		break;
	case KOOPA_STATE_WALKING_RIGHT:
		isKicked = false;
		vx = KOOPA_WALKING_SPEED;
		break;
	}
	CGameObject::SetState(state);
}