#include "Bush.h"

#include "Sprite.h"
#include "Sprites.h"

#include "Textures.h"
#include "Game.h"

void CBush::RenderBoundingBox()
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

	float xx = x - BUSH_WIDTH / 2 + rect.right / 2;
	float yy = y - BUSH_HEIGHT / 2 + rect.bottom / 2;

	CGame::GetInstance()->Draw(xx - cx, yy - cy, bbox, nullptr, BBOX_ALPHA, rect.right - 1, rect.bottom - 1);
}

void CBush::Render()
{
	CSprites* s = CSprites::GetInstance();

	for (int j = 0; j < this->width; j++)
	{
		float xx = x + j * BUSH_WIDTH;
		s->Get(ID_TEX_BUSH)->Draw(xx, y);
	}

	//RenderBoundingBox();
}

void CBush::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x - BUSH_WIDTH / 2;
	t = y - BUSH_HEIGHT / 2;
	r = l + BUSH_WIDTH * this->width;
	b = t + BUSH_HEIGHT;
}

int CBush::IsDirectionColliable(float nx, float ny)
{
	return 0;
}