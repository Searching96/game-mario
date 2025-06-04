#include "Platform.h"

#include "Sprite.h"
#include "Sprites.h"

#include "Textures.h"
#include "Game.h"

void CPlatform::RenderBoundingBox()
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

	float xx = x - PLATFORM_CELL_WIDTH / 2 + rect.right / 2;
	float yy = y - PLATFORM_CELL_HEIGHT / 2 + rect.bottom / 2;

	CGame::GetInstance()->Draw(xx - cx, yy - cy, bbox, nullptr, BBOX_ALPHA, rect.right - 1, rect.bottom - 1);
}

void CPlatform::Render()
{
	if (this->height <= 0 || this->width <= 0) return;
	CSprites* s = CSprites::GetInstance();

	for (int i = 0; i < this->height; i++)
	{
		for (int j = 0; j < this->width; j++)
		{
			float xx = x + j * PLATFORM_CELL_WIDTH;
			float yy = y + i * PLATFORM_CELL_HEIGHT;

			// Check platform type before drawing
			switch (this->type)
			{
			case 1:
				if (i == 0)
				{
					if (j == 0)
						s->Get(ID_SPRITE_STRIPED_TOP_LEFT)->Draw(xx, yy);
					else if (j == this->width - 1)
						s->Get(ID_SPRITE_STRIPED_TOP_RIGHT)->Draw(xx, yy);
					else
						s->Get(ID_SPRITE_STRIPED_TOP_CENTER)->Draw(xx, yy);
				}
				else
				{
					if (j == 0)
						s->Get(ID_SPRITE_STRIPED_BOTTOM_LEFT)->Draw(xx, yy);
					else if (j == this->width - 1)
						s->Get(ID_SPRITE_STRIPED_BOTTOM_RIGHT)->Draw(xx, yy);
					else
						s->Get(ID_SPRITE_STRIPED_BOTTOM_CENTER)->Draw(xx, yy);
				}
				break;
			case 2:
				if (i == 0)
				{
					if (j == 0)
						s->Get(ID_SPRITE_HILL_TOP_LEFT)->Draw(xx, yy);
					else if (j == this->width - 1)
						s->Get(ID_SPRITE_HILL_TOP_RIGHT)->Draw(xx, yy);
					else
						s->Get(ID_SPRITE_HILL_TOP_CENTER)->Draw(xx, yy);
				}
				else
				{
					if (j == 0)
						s->Get(ID_SPRITE_HILL_BOTTOM_LEFT)->Draw(xx, yy);
					else if (j == this->width - 1)
						s->Get(ID_SPRITE_HILL_BOTTOM_RIGHT)->Draw(xx, yy);
					else
						s->Get(ID_SPRITE_HILL_BOTTOM_CENTER)->Draw(xx, yy);
				}
				break;
			case 0:
			default:
				if (i == 0)
				{
					if (j == 0)
						s->Get(ID_SPRITE_PLATFORM_TOP_LEFT)->Draw(xx, yy);
					else if (j == this->width - 1)
						s->Get(ID_SPRITE_PLATFORM_TOP_RIGHT)->Draw(xx, yy);
					else
						s->Get(ID_SPRITE_PLATFORM_TOP_CENTER)->Draw(xx, yy);
				}
				else
				{
					if (j == 0)
						s->Get(ID_SPRITE_PLATFORM_BOTTOM_LEFT)->Draw(xx, yy);
					else if (j == this->width - 1)
						s->Get(ID_SPRITE_PLATFORM_BOTTOM_RIGHT)->Draw(xx, yy);
					else
						s->Get(ID_SPRITE_PLATFORM_BOTTOM_CENTER)->Draw(xx, yy);
				}
				break;
			}
		}
	}

	//RenderBoundingBox();
}

void CPlatform::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x - PLATFORM_CELL_WIDTH / 2;
	t = y - PLATFORM_CELL_HEIGHT / 2;
	r = l + PLATFORM_CELL_WIDTH * this->width;
	b = t + PLATFORM_CELL_HEIGHT * this->height;
}

int CPlatform::IsDirectionColliable(float nx, float ny)
{
	return 1;
}