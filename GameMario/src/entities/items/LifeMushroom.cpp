#include "entities/items/LifeMushroom.h"
#include "core/Game.h"
#include "engine/rendering/Particle.h"

void CLifeMushroom::Render()
{
    if (isVisible == 0) return;

    CAnimations* animations = CAnimations::GetInstance();
    animations->Get(ID_ANI_LIFE_MUSHROOM)->Render(x, y);

    //RenderBoundingBox();
}

void CLifeMushroom::Activate()
{
    CGame::GetInstance()->GetGameState()->AddLives();
}