#include "FallingPlatform.h"
#include "Textures.h"
#include "Game.h"
#include "Debug.h"
#include "Chunk.h"
#include "PlayScene.h"

#define FALLING_PLATFORM_GRAVITY 0.0003f
#define FALLING_PLATFORM_VX 0.035f

CFallingPlatform::CFallingPlatform(int id, float x, float y, int z, int originalChunkId, int width) : CGameObject(id, x, y, z)
{
	this->width = width;
	this->vx = -FALLING_PLATFORM_VX;
	this->vy = 0;  // Initially not falling
	this->ay = FALLING_PLATFORM_GRAVITY;
	this->x0 = x;
	this->y0 = y;
	this->fallStart = 0;
	this->originalChunkId = originalChunkId;
	this->state = FALLING_PLATFORM_STATE_INACTIVE;
}

void CFallingPlatform::Render() {
	if (this->width <= 0) return;

	if (isDefeated == 1)
		return;
	CSprites* s = CSprites::GetInstance();

	// Draw left edge
	s->Get(ID_SPRITE_FALLING_PLATFORM_LEFT)->Draw(x, y);

	// Draw middle sections
	for (int i = 1; i < width - 1; i++) {
		s->Get(ID_SPRITE_FALLING_PLATFORM_CENTER)->Draw(x + i * FALLING_PLATFORM_CELL_WIDTH, y);
	}

	// Draw right edge
	if (width > 1) {
		s->Get(ID_SPRITE_FALLING_PLATFORM_RIGHT)->Draw(x + (width - 1) * FALLING_PLATFORM_CELL_WIDTH, y);
	}

	//RenderBoundingBox(); // Uncomment for debugging
}

void CFallingPlatform::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects) {

	if (isDefeated == 1)
		return;

	// Apply gravity and move platform when active
	if (state == FALLING_PLATFORM_STATE_ACTIVE)
	{
		vy += ay * dt;

		// Check if platform should be marked complete after timeout
		if (GetTickCount64() - fallStart > FALLING_PLATFORM_TIMEOUT) {
			SetState(FALLING_PLATFORM_STATE_COMPLETE);
			LPCHUNK chunk = ((LPPLAYSCENE)CGame::GetInstance()->GetCurrentScene())->GetChunk(originalChunkId);
			chunk->SetIsObjectDeleted(this->GetId(), true);
			isDeleted = true;
			return;
		}
	}

	// Update position
	x += vx * dt;
	y += vy * dt;

	CGameObject::Update(dt, coObjects);
	CCollision::GetInstance()->Process(this, dt, coObjects);
}

void CFallingPlatform::GetBoundingBox(float& l, float& t, float& r, float& b) {
	l = x - FALLING_PLATFORM_CELL_WIDTH / 2;
	t = y - FALLING_PLATFORM_CELL_HEIGHT / 2;
	r = l + width * FALLING_PLATFORM_CELL_WIDTH;
	b = t + FALLING_PLATFORM_CELL_HEIGHT;
}

void CFallingPlatform::RenderBoundingBox()
{
	D3DXVECTOR3 p(x, y, 0);
	RECT rect;

	LPTEXTURE bbox = CTextures::GetInstance()->Get(ID_TEX_BBOX);

	float l, t, r, b;

	GetBoundingBox(l, t, r, b);
	rect.left = 0;
	rect.top = 0;
	rect.right = (int)r - (int)l;
	rect.bottom = (int)b - (int)t;

	float cx, cy;
	CGame::GetInstance()->GetCamPos(cx, cy);

	float xx = x - FALLING_PLATFORM_CELL_WIDTH / 2 + rect.right / 2;
	float yy = y - FALLING_PLATFORM_CELL_HEIGHT / 2 + rect.bottom / 2;

	CGame::GetInstance()->Draw(xx - cx, yy - cy, bbox, nullptr, BBOX_ALPHA, rect.right - 1, rect.bottom - 1);
}

void CFallingPlatform::SetState(int state)
{
	CGameObject::SetState(state);
	switch (state)
	{
	case FALLING_PLATFORM_STATE_ACTIVE:
		fallStart = GetTickCount64();
		vx = 0;
		break;
	case FALLING_PLATFORM_STATE_INACTIVE:
		// Reset platform
		vy = 0;
		ay = 0;
		break;
	case FALLING_PLATFORM_STATE_COMPLETE:
		// Make platform "defeated" so it can respawn
		break;
	}
}