#include "GameState.h"
#include "PlayScene.h"

void CGameState::Update(DWORD dt) {
	time -= dt;
	if (time <= 0) {
		time = 0;
		dynamic_cast<CPlayScene*>(current_scene)->GetPlayer()->SetState(MARIO_STATE_DIE);
	}
}
void CGameState::RenderHUD()
{
	float cam_x, cam_y;
	CGame* game = CGame::GetInstance();
	CSprites* s = CSprites::GetInstance();
	game->GetCamPos(cam_x, cam_y);
	float screen_width = (float)game->GetBackBufferWidth();
	float screen_height = (float)game->GetBackBufferHeight();
	s->Get(ID_SPRITE_HUD_BACKGROUND)->Draw(cam_x + screen_width / 2, cam_y + screen_height - (HUD_HEIGHT + HUD_Y_OFFSET) / 2);
	s->Get(ID_SPRITE_HUD)->Draw(cam_x + HUD_WIDTH / 2 + HUD_X_OFFSET, cam_y + game->GetBackBufferHeight() - HUD_HEIGHT / 2 - HUD_Y_OFFSET);

	//Draw numbers
	//World
	CPlayScene* scene = dynamic_cast<CPlayScene*>(current_scene);
	int number_sprite_id;
	switch (scene->GetId())
	{
	case 1:
		number_sprite_id = ID_SPRITE_NUMBER_1;
		break;
	case 2:
		number_sprite_id = ID_SPRITE_NUMBER_2;
		break;
	case 3:
		number_sprite_id = ID_SPRITE_NUMBER_3;
		break;
	case 4:
		number_sprite_id = ID_SPRITE_NUMBER_4;
		break;
	case 5:
		number_sprite_id = ID_SPRITE_NUMBER_5;
		break;
	case 6:
		number_sprite_id = ID_SPRITE_NUMBER_6;
		break;
	case 7:
		number_sprite_id = ID_SPRITE_NUMBER_7;
		break;
	case 8:
		number_sprite_id = ID_SPRITE_NUMBER_8;
		break;
	case 9:
		number_sprite_id = ID_SPRITE_NUMBER_9;
		break;
	default:
		number_sprite_id = ID_SPRITE_NUMBER_0;
		break;
	}
	s->Get(number_sprite_id)->Draw(cam_x, cam_y);
	//DebugOut(L"HUD pos: %d, %d\n", hud_left_x, hud_top_y);
}