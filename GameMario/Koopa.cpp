#include "Koopa.h"
#include "Particle.h"

CKoopa::CKoopa(int id, float x, float y, int z, int originalChunkId) : CGameObject(id, x, y, z)
{
	this->originalChunkId = originalChunkId;
	this->ax = 0;
	this->ay = KOOPA_GRAVITY;
	x0 = x;
	y0 = y;
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

	/*if (e->obj->IsBlocking())
		OnCollisionWithTerrain(e);*/
	if (dynamic_cast<CCoinQBlock*>(e->obj) || dynamic_cast<CBuffQBlock*>(e->obj))
		OnCollisionWithQuestionBlock(e);
	else if (dynamic_cast<CGoomba*>(e->obj))
		OnCollisionWithGoomba(e);
	else if (dynamic_cast<CWingedGoomba*>(e->obj))
		OnCollisionWithWingedGoomba(e);
	else if (dynamic_cast<CPiranhaPlant*>(e->obj))
		OnCollisionWithPiranhaPlant(e);
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

void CKoopa::OnCollisionWithGoomba(LPCOLLISIONEVENT e)
{
	CGoomba* g = dynamic_cast<CGoomba*>(e->obj);
	if (this->state == KOOPA_STATE_SHELL_DYNAMIC)
	{
		if (g->IsDead() == 0)
		{
			g->SetState(GOOMBA_STATE_DIE_ON_TAIL_WHIP);
		}
	}
	else if (this->state != KOOPA_STATE_SHELL_STATIC)
	{
		if (e->nx != 0)
		{
			this->SetState((vx > 0) ? KOOPA_STATE_WALKING_LEFT : KOOPA_STATE_WALKING_RIGHT);
			float eVx, eVy;
			g->GetSpeed(eVx, eVy);
			g->SetSpeed(-eVx, eVy);
		}
	}
}

void CKoopa::OnCollisionWithWingedGoomba(LPCOLLISIONEVENT e)
{
	CWingedGoomba* wg = dynamic_cast<CWingedGoomba*>(e->obj);
	if (this->state == KOOPA_STATE_SHELL_DYNAMIC)
	{
		if (wg->IsDead() == 0)
		{
			wg->SetState(WINGED_GOOMBA_STATE_DIE_ON_TAIL_WHIP);
		}
	}
	else if (this->state != KOOPA_STATE_SHELL_STATIC)
	{
		if (e->nx != 0 && wg->IsWinged() == 0)
		{
			this->SetState((vx > 0) ? KOOPA_STATE_WALKING_LEFT : KOOPA_STATE_WALKING_RIGHT);
			float eVx, eVy;
			wg->GetSpeed(eVx, eVy);
			wg->SetSpeed(-eVx, eVy);
		}
	}
}

void CKoopa::OnCollisionWithPiranhaPlant(LPCOLLISIONEVENT e)
{
	CPiranhaPlant* pp = dynamic_cast<CPiranhaPlant*>(e->obj);
	if (this->state == KOOPA_STATE_SHELL_DYNAMIC)
	{
		if (pp->GetState() != PIRANHA_PLANT_STATE_DIED)
		{
			pp->SetState(PIRANHA_PLANT_STATE_DIED);
		}
	}
}

void CKoopa::OnCollisionWithTerrain(LPCOLLISIONEVENT e)
{
	CMario* player = (CMario*)((LPPLAYSCENE)CGame::GetInstance()->GetCurrentScene())->GetPlayer();
	this->SetState(KOOPA_STATE_DIE_ON_COLLIDE_WITH_TERRAIN);
	isHeld = 0;
	player->SetIsHoldingKoopa(0);
	player->StartKick();
}

bool CKoopa::IsPlatformEdge(float checkDistance, vector<LPGAMEOBJECT>& possibleGrounds)
{
	if (possibleGrounds.size() == 0) 
		return false;
	
	float verticalTolerance = 2.0f;
	float l, t, r, b; // Koopa's current bounding box
	GetBoundingBox(l, t, r, b);
	float koopaBottomY = b;
	float direction = (state == KOOPA_STATE_WALKING_LEFT) ? -1.0f : 1.0f;

	bool isOnAnyPlatformInList = false;

	for (const auto& ground : possibleGrounds)
	{
		if (ground == nullptr || ground->IsDeleted()) 
			continue;

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
    if (isDefeated == 1)
        return;

    CMario* player = (CMario*)((LPPLAYSCENE)CGame::GetInstance()->GetCurrentScene())->GetPlayer();

    if (CheckAndDeleteIfDied()) // Handles timeout death and marks for deletion
        return;

    // Specific logic if Koopa is being held (initial setup: vx,vy=0 and chunk updates)
    if (isHeld == 1)
    {
        UpdateChunkMembershipWhileHeld();
    }

    // Set horizontal speed if Koopa is in dynamic shell state
    SetSpeedIfShellDynamic();

    // Find potential ground objects for platform edge detection
    vector<LPGAMEOBJECT> potentialGrounds;
    FindPotentialGrounds(coObjects, potentialGrounds);

    // Handle landing logic for flying Koopas
    if (HandleFlyingLogic())
        return; // Koopa landed, state changed, update cycle might end here for this frame.

    // Handle Koopa recovering from shell state (timeout)
    if (HandleShellRecoveryLogic(dt, coObjects, player))
        return; // Returns true if Mario was holding Koopa while running, Koopa recovered, and Update should end.

    // Comprehensive logic if Koopa is currently being held by Mario
    if (isHeld == 1)
    {
        ProcessInteractionsWhileHeld(dt, coObjects, player); // Handles enemy collisions, kicking, position updates while held
        return; // All paths within ProcessInteractionsWhileHeld originally led to an early return from Update
    }

    // --- The following logic executes if Koopa is NOT being held (and previous blocks didn't cause an early return) ---

    // Apply basic physics (gravity, acceleration)
    ApplyPhysics(dt);

    // Handle walking behavior and turning at platform edges
    HandleWalkingAndEdgeDetection(potentialGrounds);

    // Check if a dynamic shell should become static (if it stopped moving)
    // CheckShellStateTransition();

    CGameObject::Update(dt, coObjects);
    CCollision::GetInstance()->Process(this, dt, coObjects);
}


bool CKoopa::CheckAndDeleteIfDied()
{
    if ((isDead == 1) && (GetTickCount64() - dieStart > KOOPA_DIE_TIMEOUT))
    {
        LPCHUNK chunk = ((LPPLAYSCENE)CGame::GetInstance()->GetCurrentScene())->GetChunk(originalChunkId);
        if (chunk) // Safety check
        {
            chunk->SetIsObjectDeleted(this->GetId(), true);
        }
        isDeleted = true; // Mark for deletion by the scene or game
        return true;    // Indicate that the object is now deleted
    }
    return false; // Not deleted
}

void CKoopa::UpdateChunkMembershipWhileHeld()
{
    vx = 0; // Koopa doesn't move on its own while held
    vy = 0;

    vector<CChunk*> chunk_list = dynamic_cast<CPlayScene*>(CGame::GetInstance()->GetCurrentScene())->GetChunks();

    // Remove 'this' Koopa from all chunks it might currently be in.
    // This assumes CChunk::RemoveObject can handle being called even if the object isn't in that specific chunk.
    for (auto chunk : chunk_list)
    {
        if (!chunk->IsLoaded()) continue;
        chunk->RemoveObject(this); // Assuming this removes 'this' if present from the chunk's object list.
        // The original code did not explicitly call RemoveEnemy here.
    }

    // Add 'this' Koopa to the chunk it currently falls into based on its x position.
    for (auto chunk : chunk_list)
    {
        if (!chunk->IsLoaded()) continue;
        if (x < chunk->GetEndX() && x > chunk->GetStartX())
        {
            chunk->AddObject(this);
            chunk->AddEnemy(this); // As per the original snippet
            break; // Koopa should primarily be in one chunk based on its x-coordinate.
        }
    }
}

void CKoopa::SetSpeedIfShellDynamic()
{
    if (state == KOOPA_STATE_SHELL_DYNAMIC)
    {
        vx = (nx > 0) ? KOOPA_SHELL_SPEED : -KOOPA_SHELL_SPEED;
    }
}

void CKoopa::FindPotentialGrounds(vector<LPGAMEOBJECT>* coObjects, vector<LPGAMEOBJECT>& outPotentialGrounds)
{
    outPotentialGrounds.clear(); // Ensure the output vector is empty

    float koopa_l_bbox, koopa_t_bbox, koopa_r_bbox, koopa_b_bbox;
    GetBoundingBox(koopa_l_bbox, koopa_t_bbox, koopa_r_bbox, koopa_b_bbox);

    float vertical_check_tolerance = 2.5f;

    for (const auto& obj : *coObjects) {
        if (obj == this || obj->IsDeleted() || !obj->IsBlocking())
            continue;

        float obj_L, obj_T, obj_R, obj_B;
        obj->GetBoundingBox(obj_L, obj_T, obj_R, obj_B);

        bool horizontal_overlap = (koopa_l_bbox < obj_R && koopa_r_bbox > obj_L);
        bool vertical_candidate = (std::abs(koopa_b_bbox - obj_T) <= vertical_check_tolerance);
        bool koopa_is_above_platform_top = koopa_b_bbox >= obj_T - vertical_check_tolerance;

        if (horizontal_overlap && vertical_candidate && koopa_is_above_platform_top)
        {
            outPotentialGrounds.push_back(obj);
        }
    }
}

bool CKoopa::HandleFlyingLogic()
{
    if (isFlying && vy == 0) // vy == 0 implies it has landed or is on ground
    {
        vx = 0; // Stop horizontal movement upon landing from flight
        isFlying = false;
        return true; // Logic handled, original code returns here.
    }
    return false; // Not flying or still in air, or no early return needed.
}

// Returns true if Update() should return early (specifically, if Koopa recovered while held by a running Mario).
bool CKoopa::HandleShellRecoveryLogic(DWORD dt, vector<LPGAMEOBJECT>* coObjects, CMario* player)
{
    if ((state == KOOPA_STATE_SHELL_STATIC || state == KOOPA_STATE_BEING_HELD)
        && !isKicked && (GetTickCount64() - shellStart > KOOPA_SHELL_TIMEOUT))
    {
        if (player->GetIsRunning() == 1 && isHeld == 1)
        {
            RecoverFromShellWhileHeldByRunningPlayer(player, dt, coObjects);
            return true; // This path in original code had a return, so Update() should return.
        }
        else
        {
            // This path is for normal shell recovery (not held by running player, or was static shell)
            RecoverFromShellNormally();
            return false; // This path in original code did NOT have a return; logic continued.
        }
    }
    return false; // Timeout/conditions not met for shell recovery.
}

void CKoopa::RecoverFromShellWhileHeldByRunningPlayer(CMario* player, DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
    y -= 6; // "RED ALERT" position adjustment
    this->SetState((player->GetNx() > 0) ? KOOPA_STATE_WALKING_RIGHT : KOOPA_STATE_WALKING_LEFT);

    // Mario takes damage or powers down
    if (player->GetLevel() == MARIO_LEVEL_TAIL)
        player->SetState(MARIO_STATE_TAIL_DOWN);
    else if (player->GetLevel() == MARIO_LEVEL_BIG)
        player->SetState(MARIO_STATE_POWER_DOWN);
    else if (player->GetLevel() == MARIO_LEVEL_SMALL)
        player->SetState(MARIO_STATE_DIE);

    isHeld = 0; // Koopa is no longer held
    player->SetIsHoldingKoopa(0); // Mario is no longer holding

    // Check for immediate collision with blocking terrain after recovery
    vector<LPCOLLISIONEVENT> coEvents;
    coEvents.clear(); // Make sure it's clear before Scan
    CCollision::GetInstance()->Scan(this, dt, coObjects, coEvents);

    for (size_t i = 0; i < coEvents.size(); i++)
    {
        LPCOLLISIONEVENT e = coEvents[i];
        if (e->obj->IsBlocking())
        {
            this->SetState(KOOPA_STATE_DIE_ON_COLLIDE_WITH_TERRAIN); // Koopa dies
            // player->SetIsHoldingKoopa(0) and beingHeld = 0 already done
            player->StartKick(); // Mario performs a kick animation/state (as per original)
            break;
        }
    }
    for (size_t i = 0; i < coEvents.size(); i++) delete coEvents[i];
    // The original code implicitly returned from Update() after this block.
}

void CKoopa::RecoverFromShellNormally()
{
    this->SetPosition(x, y - 2); // Adjust position slightly upwards
    SetState(KOOPA_STATE_WALKING_LEFT); // Default to walking left after recovery
}

// This function encapsulates all logic that happens if `beingHeld == 1`,
// and all paths within it originally led to a `return` in the Update function.
void CKoopa::ProcessInteractionsWhileHeld(DWORD dt, vector<LPGAMEOBJECT>* coObjects, CMario* player)
{
    // Path 1: Collision with other enemies while Koopa is held
    if (HandleHeldCollisionsWithEnemies(dt, coObjects, player))
    {
        return; // Koopa died interacting with an enemy, player released it.
    }

    // Path 2: Player is NOT running, so they kick/throw the Koopa
    if (player->GetIsRunning() == 0)
    {
        HandleKickingWhenPlayerNotRunning(dt, coObjects, player);
        return; // Koopa is kicked.
    }

    // Path 3: Player IS running and holding Koopa (default if above conditions not met)
    // Update Koopa's position to stay with Mario.
    UpdatePositionWhenHeldByRunningPlayer(player);
    return; // Position updated.
}

// Returns true if Koopa died from collision with an enemy and was released.
bool CKoopa::HandleHeldCollisionsWithEnemies(DWORD dt, vector<LPGAMEOBJECT>* coObjects, CMario* player)
{
    vector<LPCOLLISIONEVENT> coEvents;
    coEvents.clear();
    CCollision::GetInstance()->Scan(this, dt, coObjects, coEvents);

    bool isKilledOnCollideWithEnemy = false;
    for (size_t i = 0; i < coEvents.size(); i++)
    {
        LPCOLLISIONEVENT e = coEvents[i];
        // Note: The SetState calls for enemies (g, wg) use state names that might seem swapped (WINGED_GOOMBA_STATE for Goomba).
        // This is preserved from the original code.
        if (CGoomba* g = dynamic_cast<CGoomba*>(e->obj))
        {
            if (g->GetIsDefeated() == 1 || g->IsDead() == 1) continue;
            this->SetState(KOOPA_STATE_DIE_ON_COLLIDE_WITH_ENEMY);
            e->obj->SetState(WINGED_GOOMBA_STATE_DIE_ON_HELD_KOOPA);
            isKilledOnCollideWithEnemy = true;
        }
        else if (CWingedGoomba* wg = dynamic_cast<CWingedGoomba*>(e->obj))
        {
            if (wg->GetIsDefeated() == 1 || wg->IsDead() == 1) continue;
            this->SetState(KOOPA_STATE_DIE_ON_COLLIDE_WITH_ENEMY);
            e->obj->SetState(GOOMBA_STATE_DIE_ON_HELD_KOOPA);
            isKilledOnCollideWithEnemy = true;
        }
        else if (CPiranhaPlant* pp = dynamic_cast<CPiranhaPlant*>(e->obj))
        {
            if (pp->GetIsDefeated() == 1 || pp->GetState() == PIRANHA_PLANT_STATE_DIED) continue;
            this->SetState(KOOPA_STATE_DIE_ON_COLLIDE_WITH_ENEMY);
            e->obj->SetState(PIRANHA_PLANT_STATE_DIED);
            isKilledOnCollideWithEnemy = true;
        }
        if (isKilledOnCollideWithEnemy) break; // Process one fatal collision
    }
    for (size_t i = 0; i < coEvents.size(); i++) delete coEvents[i];

    if (isKilledOnCollideWithEnemy)
    {
        player->SetIsHoldingKoopa(0); // Mario releases the Koopa
        isHeld = 0;                // Koopa is no longer held
        // Koopa becomes a falling, dead shell; apply gravity
        vy += ay * dt;
        y += vy * dt;
        return true; // Indicate Koopa died and was released.
    }
    return false; // No fatal collision with enemy.
}

void CKoopa::HandleKickingWhenPlayerNotRunning(DWORD dt, vector<LPGAMEOBJECT>* coObjects, CMario* player)
{
    this->SetState(KOOPA_STATE_SHELL_DYNAMIC); // Koopa becomes a kicked shell
    this->SetNx(player->GetNx());              // Kicked in the direction Mario is facing
    // vx is set by SetSpeedIfShellDynamic based on state and nx, or directly:
    vx = (this->nx > 0) ? KOOPA_SHELL_SPEED : -KOOPA_SHELL_SPEED;
    vy = 0; // Kicked horizontally

    isHeld = 0;
    player->SetIsHoldingKoopa(0);
    player->StartKick(); // Mario performs kick animation/state

    // Check for immediate collision with terrain after being kicked
    vector<LPCOLLISIONEVENT> coEventsKick;
    coEventsKick.clear();
    CCollision::GetInstance()->Scan(this, dt, coObjects, coEventsKick);

    for (size_t i = 0; i < coEventsKick.size(); i++)
    {
        LPCOLLISIONEVENT e = coEventsKick[i];
        if (e->obj->IsBlocking())
        {
            SetState(KOOPA_STATE_DIE_ON_COLLIDE_WITH_TERRAIN); // Koopa dies on impact
            // beingHeld, player->SetIsHoldingKoopa, player->StartKick already handled
            break;
        }
    }
    for (size_t i = 0; i < coEventsKick.size(); i++) delete coEventsKick[i];
    // Original code implicitly returned from Update() here.
}

void CKoopa::UpdatePositionWhenHeldByRunningPlayer(CMario* player)
{
    float mX, mY;
    player->GetPosition(mX, mY);
    float mNx = player->GetNx(); // Mario's facing direction

    // Adjust Koopa's position relative to Mario based on Mario's level (size)
    if (player->GetLevel() == MARIO_LEVEL_TAIL)
    {
        x = (mNx > 0) ? (mX + 12.0f) : (mX - 12.0f);
        y = mY + 1.0f;
    }
    else if (player->GetLevel() == MARIO_LEVEL_BIG)
    {
        x = (mNx > 0) ? (mX + 10.0f) : (mX - 10.0f);
        y = mY + 1.0f;
    }
    else // MARIO_LEVEL_SMALL
    {
        x = (mNx > 0) ? (mX + 8.0f) : (mX - 8.0f);
        y = mY - 1.0f;
    }
    // Original code implicitly returned from Update() here.
}

void CKoopa::ApplyPhysics(DWORD dt)
{
    // This applies if Koopa is not being held or special held conditions didn't apply.
    vy += ay * dt; // Apply gravity
    vx += ax * dt; // Apply horizontal acceleration (e.g., friction, or for ParaKoopas)
    // For normal walking/shell Koopas, ax is often 0, and vx is set directly by states.
}

void CKoopa::HandleWalkingAndEdgeDetection(vector<LPGAMEOBJECT>& potentialGrounds)
{
    if (state == KOOPA_STATE_WALKING_LEFT) {
        if (IsPlatformEdge(0.1f, potentialGrounds)) { // Assuming IsPlatformEdge is a member function
            SetState(KOOPA_STATE_WALKING_RIGHT); // Turn around
        }
    }
    else if (state == KOOPA_STATE_WALKING_RIGHT) {
        if (IsPlatformEdge(0.1f, potentialGrounds)) { // Assuming IsPlatformEdge is a member function
            SetState(KOOPA_STATE_WALKING_LEFT); // Turn around
        }
    }
}

void CKoopa::CheckShellStateTransition()
{
    // If a dynamic shell has stopped moving, it becomes a static shell.
    // Using a small epsilon for floating point comparison to zero.
    if (state == KOOPA_STATE_SHELL_DYNAMIC && std::abs(vx) < 0.000001f && std::abs(vy) < 0.000001f)
    {
        SetState(KOOPA_STATE_SHELL_STATIC);
    }
}

void CKoopa::StartShell()
{
	shellStart = GetTickCount64();
	isKicked = false;
	SetState(KOOPA_STATE_SHELL_STATIC);
}


void CKoopa::Render()
{
	if (isDefeated == 1)
		return;

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
			CParticle::GenerateParticleInChunk(this, 100);
			CGame::GetInstance()->GetGameState()->AddScore(100);
			vx = (player->GetNx() > 0) ? 0.2f : -0.2f;
			vy = -0.35f;
			ax = 0;
			ay = KOOPA_GRAVITY;
			isDead = 1;
			break;
		}
		case KOOPA_STATE_DIE_ON_COLLIDE_WITH_TERRAIN:
			dieStart = GetTickCount64();
			CParticle::GenerateParticleInChunk(this, 100);
			CGame::GetInstance()->GetGameState()->AddScore(100);
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
			isHeld = 1;
			isFlying = false;
			break;
	}
	CGameObject::SetState(state);

	//DebugOut(L"[INFO] Koopa: state=%d\n", state);
}