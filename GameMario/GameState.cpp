#include "GameState.h"
#include "PlayScene.h"
#include "Mario.h" // Needed to get player state (P-meter)
#include "Game.h"  // Include Game for GetInstance, GetCamPos etc.
#include "Sprites.h" // Include Sprites for GetInstance

void CGameState::Update(DWORD dt) {
	// get player
	CMario* player = (CMario*)((LPPLAYSCENE)CGame::GetInstance()->GetCurrentScene())->GetPlayer();
	if (player == nullptr || player->GetState() == MARIO_STATE_DIE
		|| player->GetState() == MARIO_STATE_DIE_ON_FALLING || player->GetIsTeleporting() != 0)
		return;

	// Only decrease time if it's positive to prevent underflow issues
	if (time > dt) time -= dt;
	else
	{
		time = 0; // Clamp to zero
		// Ensure player exists before trying to kill them
		if (current_scene) {
			CPlayScene* playScene = dynamic_cast<CPlayScene*>(current_scene);
			if (playScene) {
				CMario* player = dynamic_cast<CMario*>(playScene->GetPlayer());
				if (player && player->GetState() != MARIO_STATE_DIE) { // Avoid setting die state multiple times
					player->SetState(MARIO_STATE_DIE);
				}
			}
		}
	}
}

// Helper function to draw numbers right-aligned
void CGameState::DrawNumber(float x_right, float y, int number, int max_digits, bool fill_with_0 = true)
{
	CSprites* s = CSprites::GetInstance();
	std::string num_str = std::to_string(number);
	int num_len = num_str.length();

	// Pad with leading zeros if needed (or just draw spaces)
	int padding = max_digits - num_len;
	if (padding < 0) padding = 0; // Should not happen if max_digits is correct

	// Draw digits from right to left
	float current_x = x_right;
	for (int i = num_len - 1; i >= 0; i--) {
		int digit = num_str[i] - '0';
		int sprite_id = ID_SPRITE_NUMBER_0 + digit;
		s->Get(sprite_id)->Draw(current_x - HUD_NUMBER_WIDTH / 2, y); // Center digit horizontally
		current_x -= HUD_NUMBER_WIDTH;
	}

	// Draw leading zeros
	if (fill_with_0)
		for (int i = 0; i < padding; i++) {
			s->Get(ID_SPRITE_NUMBER_0)->Draw(current_x - HUD_NUMBER_WIDTH / 2, y);
			current_x -= HUD_NUMBER_WIDTH;
		}
}

// Helper to add a card (example logic, assumes cards are added sequentially)
void CGameState::AddCard(int card_type) {
	for (int i = 0; i < 3; ++i) {
		if (collected_cards[i] == 0) { // Find first empty slot
			collected_cards[i] = card_type;
			break;
		}
	}
	// If all slots are full, maybe overwrite the first, or do nothing
}


void CGameState::RenderHUD()
{
	CGame* game = CGame::GetInstance();
	CSprites* s = CSprites::GetInstance();
	CPlayScene* scene = dynamic_cast<CPlayScene*>(current_scene);
	CMario* player = nullptr;
	if (scene) {
		player = dynamic_cast<CMario*>(scene->GetPlayer());
	}

	// Camera and Screen dimensions
	float cam_x, cam_y;
	game->GetCamPos(cam_x, cam_y);
	float screen_width = (float)game->GetBackBufferWidth();
	float screen_height = (float)game->GetBackBufferHeight();

	// --- Draw HUD Background (Black Bar) ---
	// Positioned at the bottom of the screen, centered horizontally
	float hud_background_x = cam_x + screen_width / 2;
	float hud_background_y = cam_y + screen_height - HUD_BACKGROUND_HEIGHT / 2;
	// Ensure sprite 1120001 exists and is the correct size (e.g., 256x38)
	if (s->Get(ID_SPRITE_HUD_BACKGROUND))
		s->Get(ID_SPRITE_HUD_BACKGROUND)->Draw(hud_background_x, hud_background_y);


	// --- Calculate Top-Left Corner of the Main HUD Box ---
	// Relative to the camera's top-left
	float main_hud_box_base_x = cam_x + HUD_MAIN_BOX_X_OFFSET;
	float main_hud_box_base_y = cam_y + screen_height - HUD_BACKGROUND_HEIGHT + HUD_MAIN_BOX_Y_OFFSET;

	// --- Draw Main HUD Box ---
	// Positioned relative to the base coordinates, centered on its sprite dimensions
	float main_hud_box_center_x = main_hud_box_base_x + HUD_WIDTH / 2;
	float main_hud_box_center_y = main_hud_box_base_y + HUD_HEIGHT / 2;
	s->Get(ID_SPRITE_HUD_MAIN_BOX)->Draw(main_hud_box_center_x, main_hud_box_center_y);

	// --- Define Content Area Top-Left ---
	// This is where elements INSIDE the main box are positioned relative to
	float content_x = main_hud_box_base_x;
	float content_y = main_hud_box_base_y;

	// --- Draw Row 1 Elements ---
	float y_row1 = content_y + HUD_ELEMENT_Y_ROW1;

	// World Number
	float world_num_x = content_x + 40; // Adjust X offset as needed
	int world_id = 1; // Default or get from scene if available
	if (scene) world_id = scene->GetId();
	int world_sprite_id = ID_SPRITE_NUMBER_0 + (world_id % 10); // Assumes single digit world 1-9
	s->Get(world_sprite_id)->Draw(world_num_x, y_row1);

	// P-Meter (Speed)
	int p_level = 0;
	bool p_is_max = false;
	if (player) {
		p_level = round(player->GetPMeter() * 7); // Assuming GetPowerMeter() returns 0-7
		p_is_max = p_level == 7;
	}
	float p_meter_start_x = content_x + 56; // Adjust X offset
	float p_meter_spacing = 8; // Spacing between triangles
	for (int i = 0; i < HUD_P_METER_MAX_LEVELS; ++i) {
		float p_x = p_meter_start_x + i * p_meter_spacing;
		int p_sprite_id = (i < p_level) ? ID_SPRITE_ICON_P_FILLED : ID_SPRITE_ICON_P_EMPTY;
		s->Get(p_sprite_id)->Draw(p_x, y_row1);
	}
	// Draw the flashing 'P' / final indicator
	float p_final_x = p_meter_start_x + HUD_P_METER_MAX_LEVELS * p_meter_spacing + 5;
	if (p_is_max) {
		CAnimations::GetInstance()->Get(ID_ANI_ICON_P_FILLED)->Render(p_final_x, y_row1);
	}
	else {
		s->Get(ID_SPRITE_ICON_P_NORMAL)->Draw(p_final_x, y_row1);
	}

	// Coin Count (Draw using helper, right-aligned)
	float coin_num_x_right = content_x + 148; // Right edge for number drawing
	DrawNumber(coin_num_x_right, y_row1, coins, HUD_MAX_COIN_DIGITS, false);

	// --- Draw Row 2 Elements ---
	float y_row2 = content_y + HUD_ELEMENT_Y_ROW2;

	// Mario Icon ('M')
	float m_icon_x = content_x + 12; // Adjust X offset as needed
	if (s->Get(ID_SPRITE_ICON_M))
		s->Get(ID_SPRITE_ICON_M)->Draw(m_icon_x, y_row2);

	// Lives Count (Draw using helper, right-aligned)
	float lives_num_x_right = content_x + 44; // Right edge for number drawing
	DrawNumber(lives_num_x_right, y_row2, lives, 2, false); // Assuming max 2 digits for lives

	// Score (Draw using helper, right-aligned)
	float score_x_right = content_x + 108; // Right edge for score drawing
	DrawNumber(score_x_right, y_row2, score, HUD_MAX_SCORE_DIGITS);

	// Timer (Draw using helper, right-aligned)
	float timer_x_right = content_x + 148; // Right edge for timer
	DrawNumber(timer_x_right, y_row2, GetTime(), HUD_MAX_TIME_DIGITS); // Use GetTime() for seconds


	//// --- Draw Item Card Slots ---
	//// Positioned to the right of the main HUD box
	//Draw Card background
	float card_background_x = main_hud_box_center_x + HUD_WIDTH / 2 + CARD_X_OFFSET + CARD_BACKGROUND_WIDTH / 2;
	float card_background_y = main_hud_box_center_y;
	s->Get(ID_SPRITE_CARD_BACKGROUND)->Draw(card_background_x, card_background_y);

	//float card_start_x = content_x + 208; // Adjust X offset for the first card
	//float card_y = content_y + 6;       // Adjust Y offset
	//float card_spacing = 24 + 1;        // Width of card + spacing

	//int* cards = GetCards();
	//for (int i = 0; i < 3; ++i) {
	//	float card_x = card_start_x + i * card_spacing;
	//	int card_sprite_id = ID_SPRITE_CARD_SLOT_EMPTY; // Default to empty

	//	switch (cards[i]) {
	//	case 1: card_sprite_id = ID_SPRITE_CARD_MUSHROOM; break;
	//	case 2: card_sprite_id = ID_SPRITE_CARD_FLOWER; break;
	//	case 3: card_sprite_id = ID_SPRITE_CARD_STAR; break;
	//	}

	//	if (s->Get(card_sprite_id)) // Check if sprite exists
	//		s->Get(card_sprite_id)->Draw(card_x, card_y);
	//}

	if (game->GetGameSpeed() == 0) //Is Pausing game
	{
		float cam_x, cam_y;
		game->GetCamPos(cam_x, cam_y);
		// Draw the pause background
		float pause_background_x = cam_x + screen_width / 2;
		float pause_background_y = cam_y + screen_height / 2;
		s->Get(ID_SPRITE_PAUSE)->Draw(pause_background_x, pause_background_y);
	}
}