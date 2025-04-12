#pragma once
#include "GameObject.h"

#include <vector>
#include <list>

#define QUESTIONBLOCK_BBOX_WIDTH  16
#define QUESTIONBLOCK_BBOX_HEIGHT 16

#define ID_ANI_QUESTIONBLOCK 1030000
#define ID_ANI_QUESTIONBLOCK_HIT 1031000

#define QUESTIONBLOCK_STATE_NOT_HIT 100
#define QUESTIONBLOCK_STATE_HIT 200

extern list<LPGAMEOBJECT> objects;

class CQuestionBlock : public CGameObject
{
protected:
	bool isHit = false;

public:
	CQuestionBlock(float x, float y);

	void Render();
	void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	void GetBoundingBox(float& l, float& t, float& r, float& b);
	void SetState(int state);
	//void OnCollisionWith(LPCOLLISIONEVENT e);
};

