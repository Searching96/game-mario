#pragma once
#include "core/GameObject.h"

#define ID_ANI_MUSHROOM_NORMAL 170110
#define ID_ANI_FLOWER_NORMAL 170210
#define ID_ANI_STAR_NORMAL   170310

#define ID_ANI_MUSHROOM_USED 170100
#define ID_ANI_FLOWER_USED 170200
#define ID_ANI_STAR_USED   170300

#define BUFF_STATE_NOT_USED 1000
#define BUFF_STATE_USED 2000
#define BUFF_STATE_COMPLETED    666

#define BUFF_FLOAT_SPEED    0.1f
#define BUFF_BBOX_WIDTH 16
#define BUFF_BBOX_HEIGHT    16

#define TYPE_SWITCH_TIMEOUT 200
#define USED_TIMEOUT    1000

class CBuffRoulette :
    public CGameObject
{
private:
    int type; // 1: mushroom, 2: flower, 3: star
    DWORD lastTypeSwitch;
    DWORD usedStart = -1;
    int next_scene;
public:
    CBuffRoulette(int id, float x, float y, int z, int next_scene);
    void Render();
    void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
    void GetBoundingBox(float& l, float& t, float& r, float& b);
    int IsBlocking() { return 0; }
    int IsCollidable() { return 1; };
    void OnCollisionWith(LPCOLLISIONEVENT e);
    void OnNoCollision(DWORD dt);
    void SetState(int state);
};
