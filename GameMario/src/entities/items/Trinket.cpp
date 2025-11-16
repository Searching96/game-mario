#include "entities/items/Trinket.h"

#include "engine/rendering/Sprite.h"
#include "engine/rendering/Sprites.h"

#include "engine/rendering/Textures.h"
#include "core/Game.h"

void CTrinket::RenderBoundingBox()
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

	float xx = x - TRINKET_WIDTH / 2 + rect.right / 2;
	float yy = y - TRINKET_HEIGHT / 2 + rect.bottom / 2;

	CGame::GetInstance()->Draw(xx - cx, yy - cy, bbox, nullptr, BBOX_ALPHA, rect.right - 1, rect.bottom - 1);
}

void CTrinket::Render()
{
	CSprites* s = CSprites::GetInstance();

	switch (type)
	{
	case 1:
		s->Get(ID_TEX_FLOWER)->Draw(x, y);
		break;
	case 2:
		s->Get(ID_TEX_STAR)->Draw(x, y);
		break;
	case 3:
		s->Get(ID_TEX_MUSHROOM)->Draw(x, y);
		break;
	default :
		s->Get(ID_TEX_FLOWER)->Draw(x, y);
		break;
	}


	//RenderBoundingBox();
}

void CTrinket::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x - TRINKET_WIDTH / 2;
	t = y - TRINKET_HEIGHT / 2;
	r = l + TRINKET_WIDTH;
	b = t + TRINKET_HEIGHT;
}

int CTrinket::IsDirectionColliable(float nx, float ny)
{
	return 0;
}