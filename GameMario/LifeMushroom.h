#pragma once
#include "Mushroom.h"

#define ID_ANI_LIFE_MUSHROOM 999666

class CLifeMushroom :
    public CMushroom
{
public:
	CLifeMushroom(int id, float x, float y, int z) : CMushroom(id, x, y, z)
	{
	};
	void Render();
};

