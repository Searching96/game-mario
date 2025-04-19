#include "Wing.h"
#include "Mario.h"

#include "PlayScene.h"

CWing::CWing(float x, float y, int side) : CGameObject(x, y) { this->side = side; }

void CWing::Render()
{
	if (killed == 1)
		return;
	if (state == WING_STATE_FLAP)
	{
		if (side == 0)
			CAnimations::GetInstance()->Get(ID_ANI_WING_FLAP_LEFT)->Render(x, y);
		else
			CAnimations::GetInstance()->Get(ID_ANI_WING_FLAP_RIGHT)->Render(x, y);
	}
	else if (state == WING_STATE_CLOSE)
	{
		if (side == 0)
			CAnimations::GetInstance()->Get(ID_ANI_WING_CLOSE_LEFT)->Render(x, y);
		else
			CAnimations::GetInstance()->Get(ID_ANI_WING_CLOSE_RIGHT)->Render(x, y);
	}

	//RenderBoundingBox();
}

void CWing::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
}

void CWing::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x - ATTACK_PARTICLE_BBOX_WIDTH / 2;
	t = y - ATTACK_PARTICLE_BBOX_HEIGHT / 2;
	r = l + ATTACK_PARTICLE_BBOX_WIDTH;
	b = t + ATTACK_PARTICLE_BBOX_HEIGHT;
}

void CWing::SetState(int state)
{
	CGameObject::SetState(state);
}
