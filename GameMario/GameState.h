#pragma once
#include "Scene.h"
#include "Sprites.h"
#define HUD_HEIGHT 28
#define HUD_WIDTH 152
#define HUD_X_OFFSET 13
#define HUD_Y_OFFSET 17

#define ID_SPRITE_HUD 1120000
#define ID_SPRITE_HUD_BACKGROUND 1120001

#define ID_SPRITE_NUMBER_0 1120010
#define ID_SPRITE_NUMBER_1 1120011
#define ID_SPRITE_NUMBER_2 1120012
#define ID_SPRITE_NUMBER_3 1120013
#define ID_SPRITE_NUMBER_4 1120014
#define ID_SPRITE_NUMBER_5 1120015
#define ID_SPRITE_NUMBER_6 1120016
#define ID_SPRITE_NUMBER_7 1120017
#define ID_SPRITE_NUMBER_8 1120018
#define ID_SPRITE_NUMBER_9 1120019


class CGameState
{
private:
	LPSCENE current_scene;
	int lives;
	DWORD time;
	int score;
public:
	CGameState(LPSCENE scene, int lives, int time, int score) : current_scene(scene), lives(lives), time(time), score(score) {}
	void Update(DWORD dt);
	void SetScene(LPSCENE scene) { current_scene = scene; }
	void SetLives(int lives) { this->lives = lives; }
	void AddScore(int score) { this->score += score; }
	void RenderHUD();
};

typedef CGameState* LPGAMESTATE;
