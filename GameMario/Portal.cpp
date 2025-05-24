#include "Portal.h"
#include "Game.h"
#include "Textures.h"
#include "PlayScene.h"

CPortal::CPortal(int id, float x, float y, float width, float height, int z, float targetX, float exitY, int enterDirection, int exitDirection, float yLevel)

{
	this->id = id;
	this->x = x;
	this->y = y;
	this->width = width;
	this->height = height;
	this->targetX = targetX;
	this->exitY = exitY;
	this->enterDirection = enterDirection;
	this->exitDirection = exitDirection;
	this->yLevel = yLevel;
	zIndex = z;
}

void CPortal::RenderBoundingBox()
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

	CGame::GetInstance()->Draw(x - cx, y - cy, bbox, nullptr, BBOX_ALPHA, rect.right - 1, rect.bottom - 1);
}

void CPortal::Render()
{
	RenderBoundingBox();
}

void CPortal::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x - width / 2;
	t = y - height / 2;
	r = x + width / 2;
	b = y + height / 2;
}

void CPortal::Teleport(CMario* mario)
{
	mario->Teleport(this);
	LPPLAYSCENE(CGame::GetInstance()->GetCurrentScene())->ResetAllChunkConsumables();
}