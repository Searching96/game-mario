#include "SampleKeyEventHandler.h"

#include "debug.h"
#include "Game.h"

#include "Mario.h"
#include "PlayScene.h"

void CSampleKeyHandler::OnKeyDown(int KeyCode)
{
	//DebugOut(L"[INFO] KeyDown: %d\n", KeyCode);
	CMario* player = (CMario*)((LPPLAYSCENE)CGame::GetInstance()->GetCurrentScene())->GetPlayer();

	float x, y;
	player->GetPosition(x, y);

	switch (KeyCode)
	{
	case DIK_DOWN:
		if (player->IsChangingLevel())
			return;
		player->SetState(MARIO_STATE_SIT);
		break;
	case DIK_UP:
		player->SetIsHoldingUpKey(true);
		break;
	case DIK_S:
		if (player->GetJumpCount() >= MAX_JUMP_COUNT && player->GetLevel() == MARIO_LEVEL_TAIL)
			player->SetState(MARIO_STATE_HOVER);
		else
			player->SetState(MARIO_STATE_JUMP);
		break;
	case DIK_A:
		if (player->GetLevel() == MARIO_LEVEL_TAIL)
			player->SetState(MARIO_STATE_TAIL_WHIP);
		break;
	case DIK_1:
		player->SetLevel(MARIO_LEVEL_SMALL);
		break;
	case DIK_2:
		player->SetPosition(x, y - MARIO_BIG_BBOX_HEIGHT / 2);
		player->SetLevel(MARIO_LEVEL_BIG);
		break;
	case DIK_3:
		player->SetPosition(x, y - MARIO_BIG_BBOX_HEIGHT / 2);
		player->SetLevel(MARIO_LEVEL_TAIL);
		break;
	case DIK_0:
		player->SetState(MARIO_STATE_DIE_ON_BEING_KILLED);
		break;
	}
}

void CSampleKeyHandler::OnKeyUp(int KeyCode)
{
	//DebugOut(L"[INFO] KeyUp: %d\n", KeyCode);

	CMario* mario = (CMario*)((LPPLAYSCENE)CGame::GetInstance()->GetCurrentScene())->GetPlayer();
	switch (KeyCode)
	{
	case DIK_S:
		mario->SetState(MARIO_STATE_RELEASE_JUMP);
		break;
	case DIK_DOWN:
		mario->SetState(MARIO_STATE_SIT_RELEASE);
		break;
	case DIK_UP:
		mario->SetIsHoldingUpKey(false);
		break;
	case DIK_LEFT:
		mario->SetState(MARIO_STATE_RELEASE_MOVE);
		break;
	case DIK_RIGHT:
		mario->SetState(MARIO_STATE_RELEASE_MOVE);
		break;
	case DIK_A:
		mario->SetState(MARIO_STATE_RELEASE_RUN);
		break;
	case DIK_MINUS:
		CGame::GetInstance()->DecreaseGameSpeed();
		break;
	case DIK_EQUALS:
		CGame::GetInstance()->IncreaseGameSpeed();
		break;
	case DIK_W:
		CGame::GetInstance()->PauseGame();
		break;
	case DIK_R:
		DebugOut(L"[INFO] Reloading scene...\n");
		CGame::GetInstance()->ReloadScene();
		break;
	case DIK_T:
		DebugOut(L"[INFO] Reloading assets...\n");
		CGame::GetInstance()->ReloadAssets();
	}
}

void CSampleKeyHandler::KeyState(BYTE* states)
{
	LPGAME game = CGame::GetInstance();
	CMario* player = (CMario*)((LPPLAYSCENE)CGame::GetInstance()->GetCurrentScene())->GetPlayer();

	if (game->IsKeyDown(DIK_RIGHT))
	{
		if (player->IsChangingLevel())
			return;

		if (game->IsKeyDown(DIK_A))
			player->SetState(MARIO_STATE_RUNNING_RIGHT);
		else
			player->SetState(MARIO_STATE_WALKING_RIGHT);
	}
	else if (game->IsKeyDown(DIK_LEFT))
	{
		if (player->IsChangingLevel())
			return;

		if (game->IsKeyDown(DIK_A))
			player->SetState(MARIO_STATE_RUNNING_LEFT);
		else
			player->SetState(MARIO_STATE_WALKING_LEFT);
	}
	else
		player->SetState(MARIO_STATE_IDLE);
}