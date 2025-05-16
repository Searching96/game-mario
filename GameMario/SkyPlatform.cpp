#include "SkyPlatform.h"

int CSkyPlatform::IsDirectionColliable(float nx, float ny)
{
	if (ny == -1) return 1;
	else return 0;
}

void CSkyPlatform::Render()
{
	CSprites* s = CSprites::GetInstance();
	if (this->height <= 0 || this->width <= 0) return;
	for (int i = 0; i < this->height; i++)
	{
		for (int j = 0; j < this->width; j++)
		{
			float xx = x + j * PLATFORM_CELL_WIDTH;
			float yy = y + i * PLATFORM_CELL_HEIGHT;
			s->Get(ID_SPRITE_SKY_PLATFORM)->Draw(xx, yy);
		}
	}
	//RenderBoundingBox();

}