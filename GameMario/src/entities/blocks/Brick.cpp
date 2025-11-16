#include "entities/blocks/Brick.h"

#include "engine/rendering/Sprite.h"
#include "engine/rendering/Sprites.h"

#include "engine/rendering/Textures.h"
#include "core/Game.h"

void CBrick::RenderBoundingBox()
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

	float xx = x - BRICK_CELL_WIDTH / 2 + rect.right / 2;
	float yy = y - BRICK_CELL_HEIGHT / 2 + rect.bottom / 2;

	CGame::GetInstance()->Draw(xx - cx, yy - cy, bbox, nullptr, BBOX_ALPHA, rect.right - 1, rect.bottom - 1);
}

void CBrick::Render()
{
	CAnimations* ani = CAnimations::GetInstance();
	int aniId = -1;
	if (type == 0)
		aniId = ID_ANI_BRICK;
	else if (type == 1)
		aniId = ID_ANI_STRIPED_BRICK;
	else if (type == 2)
		aniId = ID_ANI_HIDDEN_BRICK;
	if (aniId != -1)
		ani->Get(aniId)->Render(x, y);

	//RenderBoundingBox();
}

void CBrick::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x - BRICK_CELL_WIDTH / 2;
	t = y - BRICK_CELL_HEIGHT / 2;
	r = l + BRICK_CELL_WIDTH;
	b = t + BRICK_CELL_HEIGHT;
}

int CBrick::IsDirectionColliable(float nx, float ny)
{
	return 1;
}