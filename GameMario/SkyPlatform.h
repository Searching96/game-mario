#pragma once
#include "Platform.h"
class CSkyPlatform :
    public CPlatform
{
public:
	CSkyPlatform(float x, float y, float cell_width, float cell_height, int width, int height,
		int sprite_top_left, int sprite_top_center, int sprite_top_right,
		int sprite_bottom_left, int sprite_bottom_center, int sprite_bottom_right)
		: CPlatform(x, y, cell_width, cell_height, width, height,
			sprite_top_left, sprite_top_center, sprite_top_right,
			sprite_bottom_left, sprite_bottom_center, sprite_bottom_right)
	{

	};
	int IsBlocking() { return 0; }
	int IsDirectionColliable(float nx, float ny);
};

