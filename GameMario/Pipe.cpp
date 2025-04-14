#include "Pipe.h"

#include "Sprite.h"
#include "Sprites.h"

#include "Textures.h"
#include "Game.h"

void CPipe::RenderBoundingBox()
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

	float xx = x - this->cellWidth / 2 + rect.right / 2;
	float yy = y - this->cellHeight / 2 + rect.bottom / 2;

	CGame::GetInstance()->Draw(xx - cx, yy - cy, bbox, nullptr, BBOX_ALPHA, rect.right - 1, rect.bottom - 1);
}

void CPipe::Render()
{
	if (this->height <= 0) return;
	CSprites* s = CSprites::GetInstance();

	for (int i = 0; i < this->height; i++)
	{
		float xx = x + cellWidth;
		float yy = y + i * this->cellHeight;
		if (i == 0)
		{
			s->Get(this->spriteIdTopLeft)->Draw(x, yy);
			s->Get(this->spriteIdTopRight)->Draw(xx, yy);
		}
		else
		{
			s->Get(this->spriteIdBottomLeft)->Draw(x, yy);
			s->Get(this->spriteIdBottomRight)->Draw(xx, yy);
		}
	}

	//RenderBoundingBox();
}

void CPipe::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x - this->cellWidth / 2;
	t = y - this->cellHeight / 2;
	r = l + this->cellWidth * 2;
	b = t + this->cellHeight * this->height;
}

int CPipe::IsDirectionColliable(float nx, float ny)
{
	return 1;
}