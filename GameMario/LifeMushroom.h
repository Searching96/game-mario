#pragma once
#include "Mushroom.h"

#define ID_ANI_LIFE_MUSHROOM 999666

class CLifeMushroom :
    public CMushroom
{
public:
	CLifeMushroom(float x, float y) : CMushroom(x, y)
	{
	};
	void Render();
};

