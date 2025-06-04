#include "Cloud.h"

#include "Sprite.h"
#include "Sprites.h"

#include "Textures.h"
#include "Game.h"

void CCloud::RenderBoundingBox()
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

	float xx = x - CLOUD_WIDTH / 2 + rect.right / 2;
	float yy = y - CLOUD_HEIGHT / 2 + rect.bottom / 2;

	CGame::GetInstance()->Draw(xx - cx, yy - cy, bbox, nullptr, BBOX_ALPHA, rect.right - 1, rect.bottom - 1);
}

void CCloud::Render()
{
	if (this->width <= 0) return;
	CSprites* s = CSprites::GetInstance();

	if (type == 1)
	{
		// Swirl cloud
		if (this->width == 1)
		{
			s->Get(ID_TEX_SWIRL_CLOUD_SINGLE)->Draw(x, y);
			return;
		}
		else if (this->width == 2)
		{
			s->Get(ID_TEX_SWIRL_CLOUD_LEFT)->Draw(x, y);
			s->Get(ID_TEX_SWIRL_CLOUD_RIGHT)->Draw(x + CLOUD_WIDTH, y);
			return;
		}
		else
		{
			//Do not render if width > 2
		}
	}
	else {

		float yy = y + CLOUD_HEIGHT;


		for (int i = 0; i < this->width; i++)
		{

			float xx = x + i * CLOUD_WIDTH;
			float yy = y + CLOUD_HEIGHT;
			if (i == 0)
			{
				s->Get(ID_TEX_CLOUD_TOP_LEFT)->Draw(x, y);
				s->Get(ID_TEX_CLOUD_BOTTOM_LEFT)->Draw(x, yy);
			}
			else if (i == this->width - 1)
			{
				s->Get(ID_TEX_CLOUD_TOP_RIGHT)->Draw(xx, y);
				s->Get(ID_TEX_CLOUD_BOTTOM_RIGHT)->Draw(xx, yy);
			}
			else
			{
				s->Get(ID_TEX_CLOUD_TOP_CENTER)->Draw(xx, y);
				s->Get(ID_TEX_CLOUD_BOTTOM_CENTER)->Draw(xx, yy);
			}
		}
	}

	//RenderBoundingBox();
}

void CCloud::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x - CLOUD_WIDTH / 2;
	t = y - CLOUD_HEIGHT / 2;
	r = l + CLOUD_WIDTH * this->width;
	b = t + CLOUD_HEIGHT * this->height;
}

int CCloud::IsDirectionColliable(float nx, float ny)
{
	return 0;
}