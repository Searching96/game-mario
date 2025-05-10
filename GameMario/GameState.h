// --- START OF FILE GameState.h ---

#pragma once
#include "Scene.h"
#include "Sprites.h"
#include "Utils.h"

// --- HUD Dimensions and Positions ---
#define HUD_HEIGHT 28
#define HUD_WIDTH 152 // Width of the main info box sprite
#define HUD_BACKGROUND_WIDTH 261 // Assuming full screen width for black bar
#define HUD_BACKGROUND_HEIGHT 51 // Estimated height for the black bar
#define HUD_MAIN_BOX_X_OFFSET 12  // Relative X offset of the main box inside the black bar
#define HUD_MAIN_BOX_Y_OFFSET 3  // Relative Y offset of the main box inside the black bar
#define CARD_BACKGROUND_WIDTH 72 // Width of a card slot
#define CARD_X_OFFSET 10 // X offset from HUD main box right edge

#define HUD_ELEMENT_Y_ROW1 11  // Y offset for first row elements from top of main box
#define HUD_ELEMENT_Y_ROW2 19 // Y offset for second row elements from top of main box
#define HUD_NUMBER_WIDTH 8 // Width of a single number sprite

// --- Sprite IDs ---
#define ID_SPRITE_HUD_BACKGROUND 1120001 // Black background bar
#define ID_SPRITE_HUD_MAIN_BOX 1120000   // The main textured info box

// Numbers 0-9
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

// HUD Text & Icons
#define ID_SPRITE_ICON_M 1120040          // Sprite for 'M' icon (or Mario head)
#define ID_SPRITE_ICON_L 1120041          // Sprite for 'L' icon (or Luigi icon)
#define ID_SPRITE_ICON_P_EMPTY 1120020    // P-meter empty triangle
#define ID_SPRITE_ICON_P_FILLED 1120021   // P-meter filled triangle
#define ID_SPRITE_ICON_P_NORMAL 1120030   // P-meter normal 'P'
#define ID_ANI_ICON_P_FILLED 1120000 // P-meter flashing 'P' (or filled end triangle)

// Card Icons
#define ID_SPRITE_CARD_BACKGROUND 1120050
//#define ID_SPRITE_CARD_SLOT_EMPTY 1120030 // Empty card slot
//#define ID_SPRITE_CARD_MUSHROOM 1120031   // Mushroom card
//#define ID_SPRITE_CARD_FLOWER 1120032     // Flower card
//#define ID_SPRITE_CARD_STAR 1120033       // Star card

//Pause
#define ID_SPRITE_PAUSE	1120060

// --- Other Constants ---
#define HUD_MAX_SCORE_DIGITS 7
#define HUD_MAX_TIME_DIGITS 3
#define HUD_MAX_COIN_DIGITS 2
#define HUD_P_METER_MAX_LEVELS 6 // Number of triangles in P-meter

class CGameState
{
private:
	LPSCENE current_scene;
	int lives;
	DWORD time; // Remaining time in MILLISECONDS
	int score;
	int coins; // Added coins
	int collected_cards[3]; // Added card storage (0: empty, 1: Mush, 2: Flower, 3: Star)

	// Helper to draw numbers
	void DrawNumber(float x, float y, int number, int max_digits, bool fill_with_0);

public:
	// Constructor updated to include coins and cards
	CGameState(LPSCENE scene, int lives, DWORD time, int score, int coins) :
		current_scene(scene), lives(lives), time(time), score(score), coins(coins) {
		// Initialize cards to empty
		for (int i = 0; i < 3; ++i) {
			collected_cards[i] = 0;
		}
	}

	void Update(DWORD dt);
	void SetScene(LPSCENE scene) { current_scene = scene; }
	void SetLives(int lives) { this->lives = lives; }
	void AddLives() { this->lives += 1; }
	int GetLives() { return lives; }
	void AddScore(int score) { this->score += score; }
	int GetScore() { return score; }
	DWORD GetTime() { return time / 1000; } // Return time in seconds for display
	void Reset() { time = 300000; lives = 3; score = 0; coins = 0; }
	void AddCoin() { this->coins = min(this->coins + 1, 99); } // Max 99 coins
	int GetCoins() { return coins; }
	void AddCard(int card_type); // Logic to add a card
	int* GetCards() { return collected_cards; }

	void RenderHUD();
};

typedef CGameState* LPGAMESTATE;