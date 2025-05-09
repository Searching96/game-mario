#include "Koopa.h"

CKoopa::CKoopa(int id, float x, float y, int z, int originalChunkId) : CGameObject(id, x, y, z)
{
	this->originalChunkId = originalChunkId;
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

	if (e->ny < 0) {
		vy = 0;
	}
	
	if (e->nx != 0 && e->obj->IsBlocking()) {
		if (state == KOOPA_STATE_SHELL_DYNAMIC) {
			vx = -vx;
			nx = -nx;
		}
		else
		{
			this->SetState((vx > 0) ? KOOPA_STATE_WALKING_LEFT : KOOPA_STATE_WALKING_RIGHT);
		}
	}

	if (dynamic_cast<CCoinQBlock*>(e->obj) || dynamic_cast<CBuffQBlock*>(e->obj)) {
		OnCollisionWithQuestionBlock(e);
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

bool CKoopa::IsPlatformEdge(float checkDistance, vector<LPGAMEOBJECT>& possibleGrounds)
{
	if (possibleGrounds.size() == 0) return false;
	float verticalTolerance = 2.0f;
	float l, t, r, b; // Koopa's current bounding box
	GetBoundingBox(l, t, r, b);
	float koopaBottomY = b;
	float direction = (state == KOOPA_STATE_WALKING_LEFT) ? -1.0f : 1.0f;

	bool isOnAnyPlatformInList = false;

	for (const auto& ground : possibleGrounds)
	{
		if (ground == nullptr || ground->IsDeleted()) continue;

		float groundL, groundT, groundR, groundB;
		ground->GetBoundingBox(groundL, groundT, groundR, groundB);

		bool verticallyOnThisGround = std::abs(koopaBottomY - groundT) <= verticalTolerance;
		bool horizontallyOverlapping = (l < groundR && r > groundL);

		if (verticallyOnThisGround && horizontallyOverlapping)
		{
			isOnAnyPlatformInList = true;

			float projectedX = x + direction * checkDistance;

			//DebugOut(L"[INFO] On platform L=%f,R=%f. Koopa current L=%f,R=%f. Projected X=%f dir=%f dist=%f\n", groundL, groundR, l, r, projectedX, direction, checkDistance);


			if (direction < 0) // Walking left
			{
				if (projectedX <= groundL + 0.001f)
				{
					//DebugOut(L"[INFO] Edge detected (walking left): projectedX=%f <= groundL=%f\n", projectedX, groundL);
					return true;
				}
			}
			else // Walking right
			{
				if (projectedX >= groundR - 0.001f)
				{
					//DebugOut(L"[INFO] Edge detected (walking right): projectedX=%f >= groundR=%f\n", projectedX, groundR);
					return true;
				}
			}
		}
	}

	if (isOnAnyPlatformInList) {
		// DebugOut(L"[INFO] On platform, but no edge detected for any supporting platform.\n");
		return false; // No edge found
	}
	else {
		// DebugOut(L"[INFO] No supporting platform found in IsPlatformEdge for Koopa at Y_bottom=%f\n", koopaBottomY);
		return false;
	}
}

void CKoopa::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	if (beingHeld == 1)
	{
		vx = 0;
		vy = 0;

		vector<CChunk*> chunk_list = dynamic_cast<CPlayScene*>(CGame::GetInstance()->GetCurrentScene())->GetChunks();


		for (auto chunk : chunk_list)
		{
			if (!chunk->IsLoaded()) continue;
			for (auto obj : chunk->GetObjects())
			{
				if (obj == this) chunk->RemoveObject(this);
			}
			if (x < chunk->GetEndX() && x > chunk->GetStartX())
			{
				chunk->AddObject(this);
			}
		}
	}

	if (state == KOOPA_STATE_SHELL_DYNAMIC)
	{
		vx = (nx > 0) ? KOOPA_SHELL_SPEED : -KOOPA_SHELL_SPEED;
	}

	vector<LPGAMEOBJECT> potentialGrounds;
	float koopa_l_bbox, koopa_t_bbox, koopa_r_bbox, koopa_b_bbox;
	GetBoundingBox(koopa_l_bbox, koopa_t_bbox, koopa_r_bbox, koopa_b_bbox);

	// A tolerance for how close the Koopa's bottom needs to be to the platform's top.
	// This should match or be slightly larger than `verticalTolerance` in IsPlatformEdge.
	float vertical_check_tolerance = 2.5f; // e.g., 2.0f used in IsPlatformEdge + 0.5f buffer

	for (const auto& obj : *coObjects) {
		if (obj == this || obj->IsDeleted() || !obj->IsBlocking())
			continue;

		float obj_L, obj_T, obj_R, obj_B;
		obj->GetBoundingBox(obj_L, obj_T, obj_R, obj_B);

		// Condition 1: Horizontal Bounding Box Overlap
		bool horizontal_overlap = (koopa_l_bbox < obj_R && koopa_r_bbox > obj_L);

		// Condition 2: Platform's top surface is near Koopa's bottom.
		// This ensures we only consider objects that could actually be ground.
		bool vertical_candidate = (std::abs(koopa_b_bbox - obj_T) <= vertical_check_tolerance);
		// And ensure Koopa is generally above the platform, not under it
		// (koopa_t_bbox < obj_B is a loose check, more accurately obj_T should be below koopa_b_bbox)
		bool koopa_is_above_platform_top = koopa_b_bbox >= obj_T - vertical_check_tolerance;


		if (horizontal_overlap && vertical_candidate && koopa_is_above_platform_top)
		{
			potentialGrounds.push_back(obj);
		}
	}

	//DebugOutTitle(L"Being held: %d, vx=%f, vy=%f\n", beingHeld, vx, vy);

	if ((isDead == 1) && (GetTickCount64() - dieStart > KOOPA_DIE_TIMEOUT))
	{
		LPCHUNK chunk = ((LPPLAYSCENE)CGame::GetInstance()->GetCurrentScene())->GetChunk(originalChunkId);
		chunk->SetIsObjectDeleted(this->GetId(), true);
		isDeleted = true;
		return;
	}

	CMario* player = (CMario*)((LPPLAYSCENE)CGame::GetInstance()->GetCurrentScene())->GetPlayer();

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
		if (player->GetIsRunning() == 1 && beingHeld == 1)
		{
			if (player->GetLevel() == MARIO_LEVEL_TAIL)
				player->SetState(MARIO_STATE_TAIL_DOWN);
			else if (player->GetLevel() == MARIO_LEVEL_BIG)
				player->SetState(MARIO_STATE_POWER_DOWN);
			else if (player->GetLevel() == MARIO_LEVEL_SMALL)
				player->SetState(MARIO_STATE_DIE);

			beingHeld = 0;
			player->SetIsHoldingKoopa(0);

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
					player->SetIsHoldingKoopa(0);
					player->StartKick();
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
				e->obj->SetState(WINGED_GOOMBA_STATE_DIE_ON_HELD_KOOPA);
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
			player->SetIsHoldingKoopa(0);
			beingHeld = 0;
			vy += ay * dt;
			y += vy * dt;
			return;
		}

		if (player->GetIsRunning() == 0)
		{
			this->SetState(KOOPA_STATE_SHELL_DYNAMIC);
			this->SetNx(player->GetNx());
			this->SetSpeed((nx > 0) ? KOOPA_SHELL_SPEED : - KOOPA_SHELL_SPEED, 0);
			beingHeld = 0;
			player->SetIsHoldingKoopa(0);
			player->StartKick();

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
					player->SetIsHoldingKoopa(0);
					player->StartKick();
					break;
				}
			}

			for (size_t i = 0; i < coEvents.size(); i++) delete coEvents[i];
			return;
		}

		float mX, mY;
		player->GetPosition(mX, mY);
		float mNx = player->GetNx();
		if (player->GetLevel() == MARIO_LEVEL_BIG || player->GetLevel() == MARIO_LEVEL_TAIL)
		{
			if (mNx > 0)
				x = mX + 10;
			else
				x = mX - 10;
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
	if (isDead == 1) return;

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
			vx = (player->GetNx() > 0) ? 0.2f : -0.2f;
			vy = -0.35f;
			ax = 0;
			ay = KOOPA_GRAVITY;
			isDead = 1;
			break;
		}
		case KOOPA_STATE_DIE_ON_COLLIDE_WITH_TERRAIN:
			dieStart = GetTickCount64();
			vx = 0;
			vy = -0.4f;
			ax = 0;
			ay = KOOPA_GRAVITY;
			isDead = 1;
			break;
		case KOOPA_STATE_SHELL_STATIC:
			vx = 0;
			vy = 0;
			ax = 0;
			break;
		case KOOPA_STATE_SHELL_DYNAMIC:
			isKicked = true;
			//vy = 0;
			//ax = 0;
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

	//DebugOut(L"[INFO] Koopa: state=%d\n", state);
}