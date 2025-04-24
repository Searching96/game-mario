#include "Koopa.h"

CKoopa::CKoopa(float x, float y) :CGameObject(x, y)
{
	this->ax = 0;
	this->ay = KOOPA_GRAVITY;
	shellStart = -1;
	SetState(KOOPA_STATE_WALKING_LEFT);
}

void CKoopa::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	if (state == KOOPA_STATE_WALKING_LEFT || state == KOOPA_STATE_WALKING_RIGHT)
	{
		left = x - KOOPA_BBOX_WIDTH / 2;
		right = left + KOOPA_BBOX_WIDTH;
		bottom = y + KOOPA_TEXTURE_HEIGHT / 2;
		top = bottom - KOOPA_BBOX_HEIGHT;
	}
	else // Shell states (including being held, dynamic, static)
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
	if (state == KOOPA_STATE_DIE_ON_COLLIDE_WITH_ENEMY) return;
	if (!e->obj->IsBlocking() || e->obj == this) return;

	if (e->ny < 0) { // Collision from above (standing on something)
		vy = 0;
	}

	if (dynamic_cast<CMario*>(e->obj)) {
		OnCollisionWithMario(e);
	}
	else if (dynamic_cast<CCoinQBlock*>(e->obj) || dynamic_cast<CBuffQBlock*>(e->obj)) {
		OnCollisionWithQuestionBlock(e);
	}
}

void CKoopa::OnCollisionWithMario(LPCOLLISIONEVENT e) {
	if (e->ny < 0) {
		vy = -KOOPA_SHELL_DEFLECT_SPEED;
		vx = e->nx > 0 ? -0.2f : 0.2f;
		isFlying = true;
	}
}

void CKoopa::OnCollisionWithQuestionBlock(LPCOLLISIONEVENT e) {
	if (state == KOOPA_STATE_SHELL_DYNAMIC) {
		if (CCoinQBlock* cqb = dynamic_cast<CCoinQBlock*>(e->obj)) {
			if (e->nx != 0 && cqb->GetState() == QUESTIONBLOCK_STATE_NOT_HIT)
				cqb->SetState(QUESTIONBLOCK_STATE_BOUNCE_UP);
		}
		else if (CBuffQBlock* bqb = dynamic_cast<CBuffQBlock*>(e->obj)) {
			if (e->nx != 0 && bqb->GetState() == QUESTIONBLOCK_STATE_NOT_HIT) {
				CMario* player = (CMario*)(((LPPLAYSCENE)CGame::GetInstance()->GetCurrentScene())->GetPlayer());
				if (player->GetLevel() == MARIO_LEVEL_SMALL) {
					CMushroom* mushroom = bqb->GetMushroom();
					if (mushroom) {
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
	}
}

bool CKoopa::IsPlatformEdge(float checkDistance, vector<LPGAMEOBJECT>& possibleGrounds) {
	float verticalTolerance = 5.0f;
	float koopaL, koopaT, koopaR, koopaB;
	GetBoundingBox(koopaL, koopaT, koopaR, koopaB);
	float koopaBottom = koopaB;
	float koopaWidth = koopaR - koopaL;

	// Define two check points slightly offset from the corners
	float edgeOffset = koopaWidth * 0.4f; // Check near the corners, adjust as needed

	float checkX_Outer, checkX_Inner;
	if (state == KOOPA_STATE_WALKING_LEFT) {
		checkX_Outer = koopaL - checkDistance;         // Point ahead of the outer edge
		checkX_Inner = koopaL + edgeOffset - checkDistance; // Point ahead of an inner part
	}
	else { // Walking right
		checkX_Outer = koopaR + checkDistance;         // Point ahead of the outer edge
		checkX_Inner = koopaR - edgeOffset + checkDistance; // Point ahead of an inner part
	}

	bool groundFoundOuter = false;
	bool groundFoundInner = false;


	for (const auto& ground : possibleGrounds) {
		if (ground == nullptr || ground->IsDeleted()) continue;

		float groundL, groundT, groundR, groundB;
		ground->GetBoundingBox(groundL, groundT, groundR, groundB);


		// Check outer point
		if (!groundFoundOuter && checkX_Outer >= groundL && checkX_Outer <= groundR) {
			if (abs(koopaBottom - groundT) <= verticalTolerance) {
				groundFoundOuter = true;
			}
		}

		// Check inner point
		if (!groundFoundInner && checkX_Inner >= groundL && checkX_Inner <= groundR) {
			if (abs(koopaBottom - groundT) <= verticalTolerance) {
				groundFoundInner = true;
			}
		}

		// If both points have found ground, no need to check further
		if (groundFoundOuter && groundFoundInner) break;
	}

	// It's an edge if EITHER the inner OR outer check point fails to find ground
	return !(groundFoundOuter && groundFoundInner);
}
void CKoopa::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	if (beingHeld == 1)
	{
		vx = 0;
		vy = 0;
	}

	// Add blocking objects as potential ground
	vector<LPGAMEOBJECT> potentialGrounds;
	for (const auto& obj : *coObjects) {
		if (obj != this && !obj->IsDeleted() && obj->IsBlocking()) { 
			potentialGrounds.push_back(obj);
		}
	}

	DebugOutTitle(L"Being held: %d, vx=%f, vy=%f\n", beingHeld, vx, vy);

	if ((state == KOOPA_STATE_DIE_ON_COLLIDE_WITH_ENEMY) && (GetTickCount64() - dieStart > KOOPA_DIE_TIMEOUT))
	{
		isDeleted = true;
		return;
	}

	CMario* mario = (CMario*)((LPPLAYSCENE)CGame::GetInstance()->GetCurrentScene())->GetPlayer();

	if (isFlying && vy == 0)
	{
		vx = 0;
		isFlying = false;
		return;
	}

	// thoat khoi mai
	if ((state == KOOPA_STATE_SHELL_STATIC || state == KOOPA_STATE_BEING_HELD)
		&& !isKicked && (GetTickCount64() - shellStart > KOOPA_SHELL_TIMEOUT))
	{
		if (mario->GetIsRunning() == 1 && beingHeld == 1)
		{
			if (mario->GetLevel() == MARIO_LEVEL_TAIL)
				mario->SetState(MARIO_STATE_TAIL_DOWN);
			else if (mario->GetLevel() == MARIO_LEVEL_BIG)
				mario->SetState(MARIO_STATE_POWER_DOWN);
			else if (mario->GetLevel() == MARIO_LEVEL_SMALL)
				mario->SetState(MARIO_STATE_DIE);

			beingHeld = 0;
			mario->SetIsHoldingKoopa(0);

			// Check for collision with blocking objects
			vector<LPCOLLISIONEVENT> coEvents;
			vector<LPCOLLISIONEVENT> coEventsResult;

			coEvents.clear();
			CCollision::GetInstance()->Scan(this, dt, coObjects, coEvents);

			for (int i = 0; i < coEvents.size(); i++)
			{
				LPCOLLISIONEVENT e = coEvents[i];
				if (e->obj->IsBlocking())
				{
					this->SetState(KOOPA_STATE_DIE_ON_COLLIDE_WITH_TERRAIN);
					beingHeld = 0;
					mario->SetIsHoldingKoopa(0);
					mario->StartKick();
					break;
				}
			}

			for (int i = 0; i < coEvents.size(); i++) delete coEvents[i];
			return;
		}
		this->SetPosition(x, y - 2);
		SetState(KOOPA_STATE_WALKING_LEFT);
	}

	if (beingHeld == 1)
	{
		// Kiểm tra overlap với Goomba
		vector<LPCOLLISIONEVENT> coEvents;
		coEvents.clear();
		CCollision::GetInstance()->Scan(this, dt, coObjects, coEvents);

		int isKilledOnCollideWithEnemy = 0;
		for (size_t i = 0; i < coEvents.size(); i++)
		{
			LPCOLLISIONEVENT e = coEvents[i];
			if (dynamic_cast<CGoomba*>(e->obj) || dynamic_cast<CWingedGoomba*>(e->obj))
			{
				this->SetState(KOOPA_STATE_DIE_ON_COLLIDE_WITH_ENEMY);
				e->obj->SetState(GOOMBA_STATE_DIE_ON_TAIL_WHIP);
				isKilledOnCollideWithEnemy = 1;
			}
			else if (dynamic_cast<CPiranhaPlant*>(e->obj))
			{
				this->SetState(KOOPA_STATE_DIE_ON_COLLIDE_WITH_ENEMY);
				e->obj->SetState(PIRANHA_PLANT_STATE_DIED);
				isKilledOnCollideWithEnemy = 1;
			}
		}

		for (size_t i = 0; i < coEvents.size(); i++) delete coEvents[i];
		if (isKilledOnCollideWithEnemy == 1)
		{
			mario->SetIsHoldingKoopa(0);
			beingHeld = 0;
			vy += ay * dt;
			y += vy * dt;
			return;
		}

		if (mario->GetIsRunning() == 0)
		{
			float mNx;
			mario->GetNx(mNx);
			this->SetState(KOOPA_STATE_SHELL_DYNAMIC);
			this->SetSpeed((mNx > 0) ? KOOPA_SHELL_SPEED : - KOOPA_SHELL_SPEED, 0);
			beingHeld = 0;
			mario->SetIsHoldingKoopa(0);

			// Kiểm tra va chạm với các đối tượng có IsBlocking = 1
			vector<LPCOLLISIONEVENT> coEvents;
			vector<LPCOLLISIONEVENT> coEventsResult;

			coEvents.clear();
			CCollision::GetInstance()->Scan(this, dt, coObjects, coEvents);

			for (size_t i = 0; i < coEvents.size(); i++)
			{
				LPCOLLISIONEVENT e = coEvents[i];
				if (e->obj->IsBlocking())
				{
					SetState(KOOPA_STATE_DIE_ON_COLLIDE_WITH_TERRAIN);
					beingHeld = 0;
					mario->SetIsHoldingKoopa(0);
					mario->StartKick();
					break;
				}
			}

			for (size_t i = 0; i < coEvents.size(); i++) delete coEvents[i];
			return;
		}

		float mX, mY;
		mario->GetPosition(mX, mY);
		float mNx;
		mario->GetNx(mNx);
		if (mario->GetLevel() == MARIO_LEVEL_BIG || mario->GetLevel() == MARIO_LEVEL_TAIL)
		{
			if (mNx > 0)
				x = mX + 8;
			else
				x = mX - 8;
			y = mY + 1;
		}
		else
		{
			if (mNx > 0)
				x = mX + 6;
			else
				x = mX - 6;
			y = mY - 1;
		}
		return;
	}

	vy += ay * dt;
	vx += ax * dt;

	if (state == KOOPA_STATE_WALKING_LEFT) {
		if (IsPlatformEdge(0.1f, potentialGrounds)) {
			SetState(KOOPA_STATE_WALKING_RIGHT);
		}
	}
	else if (state == KOOPA_STATE_WALKING_RIGHT) {
		if (IsPlatformEdge(0.1f, potentialGrounds)) {
			SetState(KOOPA_STATE_WALKING_LEFT);
		}
	}

	if (state == KOOPA_STATE_SHELL_DYNAMIC && vx < 0.000000001f && vy < 0.000000001f) SetState(KOOPA_STATE_SHELL_STATIC);

	//DebugOut(L"[INFO] Koopa: vx=%f, ax=%f, vy=%f, ay=%f\n", vx, ax, vy, ay);
	//float l, t, r, b;
	//GetBoundingBox(l, t, r, b);
	//DebugOut(L"[INFO] Bounding box: left=%f, top=%f, right=%f, bottom=%f\n", l, t, r, b);
	//DebugOut(L"[INFO] Koopa: x=%f, y=%f\n", x, y);

	CGameObject::Update(dt, coObjects);
	CCollision::GetInstance()->Process(this, dt, coObjects);
}

void CKoopa::StartShell()
{
	shellStart = GetTickCount64();
	isKicked = false;
	SetState(KOOPA_STATE_SHELL_STATIC);
}


void CKoopa::Render()
{
	int aniId;
	if (state == KOOPA_STATE_SHELL_STATIC || state == KOOPA_STATE_BEING_HELD)
	{
		if (!isKicked && GetTickCount64() - shellStart > KOOPA_SHELL_ALERT_TIMEOUT)
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

	if (state == KOOPA_STATE_DIE_ON_COLLIDE_WITH_ENEMY || state == KOOPA_STATE_DIE_ON_COLLIDE_WITH_TERRAIN)
	{
		aniId = ID_ANI_KOOPA_SHELL_STATIC_REVERSE_1;
	}

	CAnimations::GetInstance()->Get(aniId)->Render(x, y);
	//RenderBoundingBox();
}
void CKoopa::SetState(int state)
{
	if (this->state == KOOPA_STATE_DIE_ON_COLLIDE_WITH_ENEMY) return;

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
		case KOOPA_STATE_DIE_ON_COLLIDE_WITH_ENEMY:
		{
			dieStart = GetTickCount64();
			CMario* player = (CMario*)((LPPLAYSCENE)CGame::GetInstance()->GetCurrentScene())->GetPlayer();
			float mNx;
			player->GetNx(mNx);
			vx = (mNx > 0) ? 0.2f : -0.2f;
			vy = -0.4f;
			ax = 0;
			ay = KOOPA_GRAVITY;
			break;
		}
		case KOOPA_STATE_DIE_ON_COLLIDE_WITH_TERRAIN:
			dieStart = GetTickCount64();
			vx = 0;
			vy = -0.4f;
			ax = 0;
			ay = KOOPA_GRAVITY;
			break;
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
		case KOOPA_STATE_BEING_HELD:
			beingHeld = 1;
			isFlying = false;
			break;
	}
	CGameObject::SetState(state);

	DebugOut(L"[INFO] Koopa: state=%d\n", state);
}