#include "LifeMushroom.h"

void CLifeMushroom::Render()
{
    if (isVisible == 0) return;

    CAnimations* animations = CAnimations::GetInstance();
    animations->Get(ID_ANI_LIFE_MUSHROOM)->Render(x, y);

    //RenderBoundingBox();
}