#include <iostream>
#include <fstream>
#include "AssetIDs.h"

#include "PlayScene.h"
#include "Utils.h"
#include "Textures.h"
#include "Sprites.h"
#include "Portal.h"
#include "Coin.h"
#include "Platform.h"
#include "QuestionBlock.h"
#include "Mushroom.h"
#include "SuperLeaf.h"
#include "PiranhaPlant.h"
#include "CoinQBlock.h"
#include "BuffQBlock.h"
#include "TailWhip.h"
#include "Fireball.h"
#include "Koopa.h"
#include "WingedGoomba.h"
#include "LifeBrick.h"

#include "Box.h"
#include "Tree.h"
#include "Pipe.h"
#include "Bush.h"
#include "Cloud.h"
#include "Trinket.h"
#include "SkyPlatform.h"
#include "AttackParticle.h"

#include "SampleKeyEventHandler.h"

using namespace std;

CPlayScene::CPlayScene(int id, LPCWSTR filePath) :
	CScene(id, filePath)
{
	player = NULL;
	key_handler = new CSampleKeyHandler(this);
}


#define SCENE_SECTION_UNKNOWN -1
#define SCENE_SECTION_ASSETS	1
#define SCENE_SECTION_CHUNK_OBJECT 2
#define SCENE_SECTION_SETTINGS 3

#define ASSETS_SECTION_UNKNOWN -1
#define ASSETS_SECTION_SPRITES 1
#define ASSETS_SECTION_ANIMATIONS 2

#define MAX_SCENE_LINE 1024

void CPlayScene::_ParseSection_SPRITES(string line)
{
	vector<string> tokens = split(line);

	if (tokens.size() < 6) return; // skip invalid lines

	int ID = atoi(tokens[0].c_str());
	int l = atoi(tokens[1].c_str());
	int t = atoi(tokens[2].c_str());
	int r = atoi(tokens[3].c_str());
	int b = atoi(tokens[4].c_str());
	int texID = atoi(tokens[5].c_str());

	LPTEXTURE tex = CTextures::GetInstance()->Get(texID);
	if (tex == NULL)
	{
		DebugOut(L"[ERROR] Texture ID %d not found!\n", texID);
		return;
	}

	CSprites::GetInstance()->Add(ID, l, t, r, b, tex);
}

void CPlayScene::_ParseSection_ASSETS(string line)
{
	vector<string> tokens = split(line);

	if (tokens.size() < 1) return;

	wstring path = ToWSTR(tokens[0]);

	LoadAssets(path.c_str());
}

void CPlayScene::_ParseSection_ANIMATIONS(string line)
{
	vector<string> tokens = split(line);

	if (tokens.size() < 3) return; // skip invalid lines - an animation must at least has 1 frame and 1 frame time

	//DebugOut(L"--> %s\n",ToWSTR(line).c_str());

	LPANIMATION ani = new CAnimation();

	int ani_id = atoi(tokens[0].c_str());
	for (int i = 1; i < tokens.size(); i += 2)	// why i+=2 ?  sprite_id | frame_time  
	{
		int sprite_id = atoi(tokens[i].c_str());
		int frame_time = atoi(tokens[i + 1].c_str());
		ani->Add(sprite_id, frame_time);
	}

	CAnimations::GetInstance()->Add(ani_id, ani);
}

void CPlayScene::_ParseSection_SETTINGS(string line)
{
	vector<string> tokens = split(line);
	if (tokens.size() < 2) return;
	string key = tokens[0];
	try {
		float value = stof(tokens[1]);

		if (key == "start_cam_x") {
			startCamX = value;
		}
		else if (key == "start_cam_y") {
			startCamY = value;
		}
		else if (key == "map_width") {
			mapWidth = value;
		}
		else if (key == "map_height") {
			mapHeight = value;
		}
		else if (key == "margin_x") marginX = value;
		else if (key == "margin_y") marginY = value;
		else {
		}
	}
	catch (...) {
		DebugOut(L"[ERROR] Failed to parse setting line: %hs\n", line.c_str());
	}
}

/*
	Parse a line in section [OBJECTS]
*/
void CPlayScene::_ParseSection_CHUNK_OBJECTS(string line, LPCHUNK targetChunk)
{
	// Safety check
	if (targetChunk == nullptr) {
		DebugOut(L"[FATAL ERROR] _ParseSection_CHUNK_OBJECT called with null targetChunk!\n");
		return;
	}

	vector<string> tokens = split(line);

	// Minimum tokens: object_type, x, y
	if (tokens.size() < 3) {
		DebugOut(L"[WARN] Skipping invalid object line in chunk %d (too few tokens): %hs\n", targetChunk->GetID(), line.c_str());
		return;
	}

	int object_type = atoi(tokens[0].c_str());
	float x = (float)atof(tokens[1].c_str());
	float y = (float)atof(tokens[2].c_str());

	CGameObject* obj = NULL; // The primary object created

	switch (object_type)
	{
	case OBJECT_TYPE_MARIO:
	{
		if (player != NULL) {
			DebugOut(L"[ERROR] MARIO object was created before (in chunk %d)!\n", targetChunk->GetID());
			return;
		}
		// Create associated objects
		CAttackParticle* attackParticle = new CAttackParticle(x, y);
		CTailWhip* tailWhip = new CTailWhip(x + 8, y, attackParticle);
		obj = new CMario(x, y, tailWhip);
		player = (CMario*)obj;

		// Add all associated objects to the chunk
		targetChunk->AddObject(attackParticle);
		targetChunk->AddObject(tailWhip);
		targetChunk->AddObject(obj);

		DebugOut(L"[INFO] Player object created in chunk %d!\n", targetChunk->GetID());
		return;
	}

	case OBJECT_TYPE_GOOMBA:
		obj = new CGoomba(x, y);
		break;

	case OBJECT_TYPE_KOOPA:
		obj = new CKoopa(x, y);
		break;

	case OBJECT_TYPE_PIRANHA_PLANT:
	{
		CFireball* fireball = new CFireball(x, y - PIRANHA_PLANT_BBOX_HEIGHT - PIRANHA_PLANT_BBOX_OFFSET);
		obj = new CPiranhaPlant(x, y, fireball);
		targetChunk->AddObject(fireball);
		targetChunk->AddObject(obj);
		return;
	}

	case OBJECT_TYPE_WINGED_GOOMBA:
	{
		obj = new CWingedGoomba(x, y);
		targetChunk->AddObject(obj);
		return;
	}

	case OBJECT_TYPE_BRICK:
	{
		if (tokens.size() < 6) {
			DebugOut(L"[WARN] Skipping BRICK - Insufficient params in chunk %d: %hs\n", targetChunk->GetID(), line.c_str());
			return;
		}
		float cell_width = (float)atof(tokens[3].c_str());
		float cell_height = (float)atof(tokens[4].c_str());
		int sprite_id = atoi(tokens[5].c_str());
		obj = new CBrick(x, y, cell_width, cell_height, sprite_id);
		break;
	}

	case OBJECT_TYPE_COIN:
	{
		if (tokens.size() < 4) {
			DebugOut(L"[WARN] Skipping COIN - Insufficient params in chunk %d: %hs\n", targetChunk->GetID(), line.c_str());
			return;
		}
		int type = atoi(tokens[3].c_str());
		obj = new CCoin(x, y, type);
		break;
	}

	case OBJECT_TYPE_PLATFORM:
	{
		if (tokens.size() < 5) {
			DebugOut(L"[WARN] Skipping PLATFORM - Insufficient params in chunk %d: %hs\n", targetChunk->GetID(), line.c_str());
			return;
		}
		float width = (float)atof(tokens[3].c_str());
		float height = (float)atof(tokens[4].c_str());
		obj = new CPlatform(x, y, width, height);
		break;
	}

	case OBJECT_TYPE_SKYPLATFORM:
	{
		if (tokens.size() < 5) {
			DebugOut(L"[WARN] Skipping SKYPLATFORM - Insufficient params in chunk %d: %hs\n", targetChunk->GetID(), line.c_str());
			return;
		}
		float width = (float)atof(tokens[3].c_str());
		float height = (float)atof(tokens[4].c_str());
		obj = new CSkyPlatform(x, y, width, height);
		break;
	}

	case OBJECT_TYPE_QUESTION_BLOCK:
	{
		obj = new CQuestionBlock(x, y);
		break;
	}

	case OBJECT_TYPE_MUSHROOM:
	{
		obj = new CMushroom(x, y);
		break;
	}

	case OBJECT_TYPE_SUPERLEAF:
	{
		obj = new CSuperLeaf(x, y);
		break;
	}

	case OBJECT_TYPE_COIN_QBLOCK:
	{
		if (tokens.size() < 3) {
			DebugOut(L"[WARN] Skipping COIN_QBLOCK - Insufficient params in chunk %d: %hs\n", targetChunk->GetID(), line.c_str());
			return;
		}
		CCoin* coin = new CCoin(x, y, 1);
		obj = new CCoinQBlock(x, y, coin);
		targetChunk->AddObject(coin);
		targetChunk->AddObject(obj);
		return;
	}

	case OBJECT_TYPE_BUFF_QBLOCK:
	{
		CMushroom* mushroom = new CMushroom(x, y);
		CSuperLeaf* superleaf = new CSuperLeaf(x, y);
		obj = new CBuffQBlock(x, y, mushroom, superleaf);
		targetChunk->AddObject(mushroom);
		targetChunk->AddObject(superleaf);
		targetChunk->AddObject(obj);
		return;
	}

	case OBJECT_TYPE_LIFE_BRICK:
	{
		if (tokens.size() < 3) {
			DebugOut(L"[WARN] Skipping LIFE_BRICK - Insufficient params in chunk %d: %hs\n", targetChunk->GetID(), line.c_str());
			return;
		}
		CLifeMushroom* mushroom = new CLifeMushroom(x, y);
		obj = new CLifeBrick(x, y, mushroom);
		targetChunk->AddObject(mushroom);
		targetChunk->AddObject(obj);
		return;
	}

	case OBJECT_TYPE_BOX:
	{
		if (tokens.size() < 6) {
			DebugOut(L"[WARN] Skipping BOX - Insufficient params in chunk %d: %hs\n", targetChunk->GetID(), line.c_str());
			return;
		}
		float width = (float)atof(tokens[3].c_str());
		float height = (float)atof(tokens[4].c_str());
		int color = atoi(tokens[5].c_str());
		int bottomShadow = (tokens.size() >= 7) ? atoi(tokens[6].c_str()) : 0;
		obj = new CBox(x, y, width, height, color, bottomShadow);
		break;
	}

	case OBJECT_TYPE_TREE:
	{
		if (tokens.size() < 10) {
			DebugOut(L"[WARN] Skipping TREE - Insufficient params in chunk %d: %hs\n", targetChunk->GetID(), line.c_str());
			return;
		}
		float cell_width = (float)atof(tokens[3].c_str());
		float cell_height = (float)atof(tokens[4].c_str());
		int height = atoi(tokens[5].c_str());
		int sprite_top_left = atoi(tokens[6].c_str());
		int sprite_top_right = atoi(tokens[7].c_str());
		int sprite_bottom_left = atoi(tokens[8].c_str());
		int sprite_bottom_right = atoi(tokens[9].c_str());
		obj = new CTree(
			x, y,
			cell_width, cell_height, height,
			sprite_top_left, sprite_top_right,
			sprite_bottom_left, sprite_bottom_right
		);
		break;
	}

	case OBJECT_TYPE_PIPE:
	{
		if (tokens.size() < 10) {
			DebugOut(L"[WARN] Skipping PIPE - Insufficient params in chunk %d: %hs\n", targetChunk->GetID(), line.c_str());
			return;
		}
		float cell_width = (float)atof(tokens[3].c_str());
		float cell_height = (float)atof(tokens[4].c_str());
		int height = atoi(tokens[5].c_str());
		int sprite_top_left = atoi(tokens[6].c_str());
		int sprite_top_right = atoi(tokens[7].c_str());
		int sprite_bottom_left = atoi(tokens[8].c_str());
		int sprite_bottom_right = atoi(tokens[9].c_str());
		obj = new CPipe(
			x, y,
			cell_width, cell_height, height,
			sprite_top_left, sprite_top_right,
			sprite_bottom_left, sprite_bottom_right
		);
		break;
	}

	case OBJECT_TYPE_BUSH:
	{
		if (tokens.size() < 4) {
			DebugOut(L"[WARN] Skipping BUSH - Insufficient params in chunk %d: %hs\n", targetChunk->GetID(), line.c_str());
			return;
		}
		float width = (float)atof(tokens[3].c_str());
		obj = new CBush(
			x, y, width
		);
		break;
	}

	case OBJECT_TYPE_CLOUD:
	{
		if (tokens.size() < 4) {
			DebugOut(L"[WARN] Skipping CLOUD - Insufficient params in chunk %d: %hs\n", targetChunk->GetID(), line.c_str());
			return;
		}
		float width = (float)atof(tokens[3].c_str());
		obj = new CCloud(
			x, y, width
		);
		break;
	}

	case OBJECT_TYPE_TRINKET:
	{
		if (tokens.size() < 4) {
			DebugOut(L"[WARN] Skipping TRINKET - Insufficient params in chunk %d: %hs\n", targetChunk->GetID(), line.c_str());
			return;
		}
		float type = (float)atof(tokens[3].c_str());
		obj = new CTrinket(
			x, y, type
		);
		break;
	}

	case OBJECT_TYPE_PORTAL:
	{
		if (tokens.size() < 6) {
			DebugOut(L"[WARN] Skipping PORTAL - Insufficient params in chunk %d: %hs\n", targetChunk->GetID(), line.c_str());
			return;
		}
		float r = (float)atof(tokens[3].c_str());
		float b = (float)atof(tokens[4].c_str());
		int scene_id = atoi(tokens[5].c_str());
		obj = new CPortal(x, y, r, b, scene_id);
		break;
	}

	default:
		DebugOut(L"[ERROR] Invalid object type: %d in chunk %d for line: %hs\n", object_type, targetChunk->GetID(), line.c_str());
		return;
	}

	// General object adding for simple cases
	if (obj != NULL)
	{
		targetChunk->AddObject(obj);
	}
}

void CPlayScene::LoadAssets(LPCWSTR assetFile)
{
	DebugOut(L"[INFO] Start loading assets from : %s \n", assetFile);
	ifstream f;
	f.open(assetFile);
	int section = ASSETS_SECTION_UNKNOWN;
	char str[MAX_SCENE_LINE];
	while (f.getline(str, MAX_SCENE_LINE))
	{
		string line(str);
		if (line[0] == '#') continue;
		if (line == "[SPRITES]") { section = ASSETS_SECTION_SPRITES; continue; }
		if (line == "[ANIMATIONS]") { section = ASSETS_SECTION_ANIMATIONS; continue; }
		if (line[0] == '[') { section = SCENE_SECTION_UNKNOWN; continue; }
		switch (section)
		{
		case ASSETS_SECTION_SPRITES: _ParseSection_SPRITES(line); break;
		case ASSETS_SECTION_ANIMATIONS: _ParseSection_ANIMATIONS(line); break;
		}
	}
	f.close();
	DebugOut(L"[INFO] Done loading assets from %s\n", assetFile);
}

void CPlayScene::LoadChunkObjects(int chunk_id, LPCHUNK targetChunk)
{
	ifstream f;
	f.open(scene_file_path);
	string line;
	bool in_target_section = false;

	string target_section = "[CHUNK\t" + to_string(chunk_id) + "\t";

	while (getline(f, line))
	{
		if (line.empty() || line[0] == '#') continue;

		if (line[0] == '[')
		{
			// Check if this is our target chunk section
			in_target_section = (line.find(target_section) == 0);
			continue;
		}

		if (in_target_section)
		{
			_ParseSection_CHUNK_OBJECTS(line, targetChunk);
		}
	}

	f.close();
	targetChunk->SetLoaded(true);
	DebugOut(L"[INFO] Loaded objects for chunk %d\n", chunk_id);
}

void CPlayScene::LoadChunksInRange(float cam_x, float cam_width)
{
	float buffer = CGame::GetInstance()->GetBackBufferWidth();
	float view_left = cam_x - buffer;
	float view_right = cam_x + cam_width + buffer;

	for (LPCHUNK chunk : chunks)
	{
		if (!chunk->IsLoaded() && chunk->GetStartX() <= view_right && chunk->GetEndX() >= view_left)
		{
			LoadChunkObjects(chunk->GetID(), chunk);
		}
	}
}

void CPlayScene::UnloadChunksOutOfRange(float cam_x, float cam_width)
{
	float buffer = CGame::GetInstance()->GetBackBufferWidth();
	float view_left = cam_x - buffer;
	float view_right = cam_x + cam_width + buffer;

	for (auto it = chunks.begin(); it != chunks.end(); )
	{
		LPCHUNK chunk = *it;
		if (chunk->IsLoaded() && (chunk->GetEndX() < view_left || chunk->GetStartX() > view_right))
		{
			bool hasMario = false;
			for (LPGAMEOBJECT obj : chunk->GetObjects())
			{
				if (obj == player)
				{
					hasMario = true;
					break;
				}
			}
			if (hasMario)
			{
				++it;
				continue;
			}
			chunk->Clear();
			chunk->SetLoaded(false);
			++it;
			DebugOut(L"[INFO] Unloaded objects for chunk %d\n", chunk->GetID());
		}
		else
		{
			++it;
		}
	}
}

// In CPlayScene::Load()

void CPlayScene::Load()
{
	DebugOut(L"[INFO] Start loading scene from : %s \n", sceneFilePath);
	ifstream f;
	f.open(sceneFilePath);
	int section = SCENE_SECTION_UNKNOWN;
	char str[MAX_SCENE_LINE];

	// Reset settings to defaults before parsing, in case the file is missing some
	startCamX = 0.0f;
	startCamY = 0.0f;
	mapWidth = 0.0f; // Or perhaps screen width as default?
	mapHeight = 0.0f; // Or screen height?
	marginX = 0.0f;
	marginY = 0.0f;
	current_cam_base_y = 0.0f;
	currentParsingChunk = nullptr; // Reset parsing state

	while (f.getline(str, MAX_SCENE_LINE))
	{
		string line(str);
		if (line.empty() || line[0] == '#') continue;

		// Section checks
		if (line == "[SCENE_SETTINGS]") { section = SCENE_SECTION_SETTINGS; continue; } // Add this check
		if (line == "[ASSETS]") { section = SCENE_SECTION_ASSETS; continue; }
		if (line.substr(0, 6) == "[CHUNK")
		{
			// Extract the content inside brackets
			size_t closeBracketPos = line.find(']');
			if (closeBracketPos != string::npos) {
				string chunkHeader = line.substr(1, closeBracketPos - 1); // Remove [ and ]
				vector<string> tokens = split(chunkHeader);

				if (tokens.size() >= 4) { // "CHUNK", ID, start_X, end_X
					int chunkId = atoi(tokens[1].c_str());
					float chunk_Start_X = atof(tokens[2].c_str());
					float chunk_End_X = atof(tokens[3].c_str());

					if (GetChunk(chunkId) == nullptr) { // Avoid recreating chunks on reload
						LPCHUNK chunk = new CChunk(chunkId, chunk_Start_X, chunk_End_X);
						chunks.push_back(chunk);
						DebugOut(L"[INFO] Registered chunk %d (%f, %f)\n", chunkId, chunk_Start_X, chunk_End_X);
						currentParsingChunk = chunk; // Keep track for potential object parsing immediately after
					}
					else {
						currentParsingChunk = GetChunk(chunkId); // Point to existing chunk
						DebugOut(L"[INFO] Chunk %d already exists, reusing.\n", chunkId);
					}
				}
				else {
					DebugOut(L"[ERROR] Malformed chunk header: %hs\n", line.c_str());
					currentParsingChunk = nullptr;
				}
				section = SCENE_SECTION_CHUNK_OBJECT; // Set section type
				continue; // Process next line
				// --- End chunk header parsing ---
			}
		}
		if (line[0] == '[') { section = SCENE_SECTION_UNKNOWN; continue; } // Generic section skip

		// Line processing based on section
		switch (section)
		{
		case SCENE_SECTION_ASSETS:
			_ParseSection_ASSETS(line);
			break;
		case SCENE_SECTION_SETTINGS:    // Add this case
			_ParseSection_SETTINGS(line);
			break;
		case SCENE_SECTION_CHUNK_OBJECT:
			// If using Solution 2 (find player during load), you might parse here.
			// Otherwise, skip object lines during initial load.
			// Example for Solution 2:
			// if (currentParsingChunk != nullptr && !playerFoundInFile) {
			//     // Check if line is Mario definition and store info
			// }
			break;
		}
	}
	f.close();

	scene_file_path = sceneFilePath; // Store for later chunk object loading

	// --- Set Initial Camera and Base Y ---
	CGame* game = CGame::GetInstance();
	float initial_cam_x = startCamX;
	// We'll set initial_cam_y based on the calculated base_y later
	float cam_width = (float)game->GetBackBufferWidth();
	float cam_height = (float)game->GetBackBufferHeight();

	// Initialize camera base Y based on mapHeight (most reliable starting point)
	if (mapHeight > 0) {
		current_cam_base_y = mapHeight - cam_height - 8; // Default base = ground view (-8 offset)
		if (current_cam_base_y < 0) current_cam_base_y = 0; // Clamp to top
	}
	else {
		current_cam_base_y = 0; // Default if no mapHeight specified
	}
	// You could override with startCamY here if needed: current_cam_base_y = startCamY;

	float initial_cam_y = current_cam_base_y; // Start the camera at the calculated base

	// Clamp initial camera position (using the determined initial_cam_y)
	if (mapWidth > 0 && initial_cam_x > mapWidth - cam_width - 8) initial_cam_x = mapWidth - cam_width - 8;
	if (initial_cam_x < -8) initial_cam_x = -8;
	// Clamp Y based on mapHeight, ensuring it respects the base we just set
	if (mapHeight > 0 && initial_cam_y > mapHeight - cam_height - 8) initial_cam_y = mapHeight - cam_height - 8;
	if (initial_cam_y < 0) initial_cam_y = 0;

	game->SetCamPos(initial_cam_x, initial_cam_y);
	DebugOut(L"[INFO] Initial camera set to (%f, %f).\n", initial_cam_x, initial_cam_y);
	DebugOut(L"[INFO] Initial camera base Y set to %f.\n", current_cam_base_y);

	// --- Load initial chunks based on the camera position we just set ---
	LoadChunksInRange(initial_cam_x, cam_width);

	// --- If using Solution 2, ensure player's chunk is loaded if not already ---
	// if (playerFoundInFile) {
	//    LPCHUNK playerChunk = GetChunk(playerStartChunkId);
	//    if (playerChunk && !playerChunk->IsLoaded()) {
	//        LoadChunkObjects(playerStartChunkId, playerChunk);
	//    }
	//    // Error checking if player object wasn't created
	// }


	DebugOut(L"[INFO] Done loading scene %s\n", sceneFilePath);
}

void CPlayScene::UpdateChunks(float cam_x, float cam_width)
{
	LoadChunksInRange(cam_x, cam_width);
	UnloadChunksOutOfRange(cam_x, cam_width);
}

void CPlayScene::UpdateObjects(DWORD dt, CMario* mario, vector<LPGAMEOBJECT>& coObjects)
{
	for (LPCHUNK chunk : chunks)
	{
		vector<LPGAMEOBJECT>& chunkObjects = chunk->GetObjects();
		for (LPGAMEOBJECT obj : chunkObjects)
		{
			if (obj != mario)
				coObjects.push_back(obj);
		}
	}

	bool isChronoStopped = mario->GetIsPowerUp() || mario->GetIsTailUp() ||
		mario->GetIsPowerDown() || mario->GetIsTailDown();

	for (LPCHUNK chunk : chunks)
	{
		vector<LPGAMEOBJECT> chunkObjects = chunk->GetObjects();
		for (LPGAMEOBJECT obj : chunkObjects)
		{
			if (isChronoStopped && obj != mario)
				continue;
			obj->Update(dt, &coObjects);
		}
	}
}

void CPlayScene::UpdateCamera(CMario* mario, float player_cx, float player_cy, float cam_width, float cam_height) {
	CGame* game = CGame::GetInstance();
	float cam_x, cam_y;
	game->GetCamPos(cam_x, cam_y);

	float mario_vx, mario_vy;
	mario->GetSpeed(mario_vx, mario_vy);
	float mario_l, mario_t, mario_r, mario_b;
	mario->GetBoundingBox(mario_l, mario_t, mario_r, mario_b);
	float mario_center_y = mario_t + (mario_b - mario_t) / 2.0f;

	// --- Smooth Mario's Center Y to Reduce Jitter ---
	static float smoothed_center_y = mario_center_y;
	smoothed_center_y += (mario_center_y - smoothed_center_y) * 0.5f;

	// --- Horizontal Following with Dead Zone ---
	static float last_mario_x = player_cx;
	static bool moving_right = mario_vx >= 0;
	bool direction_changed = (mario_vx >= 0) != moving_right;
	moving_right = mario_vx >= 0;

	float dead_zone = 16.0f;
	float target_cam_x = player_cx - cam_width / 2.0f;
	if (direction_changed && abs(player_cx - last_mario_x) < dead_zone) {
		target_cam_x = cam_x;
	}
	last_mario_x = player_cx;

	// --- Absolute Ground Level ---
	float absolute_ground_cam_y = 0;
	if (mapHeight > 0) {
		absolute_ground_cam_y = mapHeight - cam_height - 8;
		if (absolute_ground_cam_y < 0) absolute_ground_cam_y = 0;
	}

	// --- Mario States ---
	bool is_flying_state = (mario->GetLevel() == MARIO_LEVEL_TAIL && (mario->GetJumpCount() > 0 || mario->GetIsHovering()));
	bool is_on_platform = mario->IsOnPlatform();
	bool is_in_water = false;
	bool is_on_vine = false;
	bool is_vertical_level = true;

	// --- Identify Sky Platform ---
	float platform_height_threshold = 200.0f;
	bool is_on_sky_platform = is_on_platform && is_vertical_level && (mario_b < mapHeight - platform_height_threshold);

	// --- Update Vertical Lock State ---
	is_camera_vertically_locked = is_flying_state || is_in_water || is_on_vine;

	// --- Falling State with Hysteresis ---
	static bool is_falling = false;
	static float last_platform_base_y = current_cam_base_y; // Track last platform's base
	float fall_threshold = last_platform_base_y + cam_height * 0.75f;
	float hysteresis_margin = 50.0f;
	if (mario_vy > 0 && smoothed_center_y > fall_threshold + hysteresis_margin) {
		is_falling = true;
	}
	else if (is_on_platform || smoothed_center_y < fall_threshold - hysteresis_margin) {
		is_falling = false;
		last_platform_base_y = current_cam_base_y; // Update on landing or above platform
	}

	// --- Determine Target Camera Y ---
	float target_cam_y;
	float smooth_factor = 0.3f;
	if (is_in_water || is_on_vine) {
		target_cam_x = player_cx - cam_width / 2.0f;
		target_cam_y = smoothed_center_y - cam_height / 2.0f;
		current_cam_base_y = target_cam_y;
	}
	else if (is_flying_state) {
		target_cam_y = smoothed_center_y - cam_height / 2.0f;
		current_cam_base_y = target_cam_y;
	}
	else if (is_on_sky_platform) {
		target_cam_y = mario_b - cam_height * 0.75f;
		current_cam_base_y = target_cam_y;
		smooth_factor = 0.5f;
	}
	else if (is_on_platform || !is_vertical_level) {
		target_cam_y = absolute_ground_cam_y;
		current_cam_base_y = absolute_ground_cam_y;
		smooth_factor = 0.5f;
	}
	else {
		// Jumping or falling
		if (is_falling) {
			target_cam_y = smoothed_center_y - cam_height / 2.0f; // Track Mario
		}
		else {
			target_cam_y = current_cam_base_y; // Lock to last platform
		}
	}

	// --- Clamp Camera ---
	if (target_cam_x < -8) target_cam_x = -8;
	if (mapWidth > 0 && target_cam_x > mapWidth - cam_width - 8) target_cam_x = mapWidth - cam_width - 8;
	if (target_cam_y < 0) target_cam_y = 0;
	if (mapHeight > 0 && target_cam_y > mapHeight - cam_height - 8) target_cam_y = mapHeight - cam_height - 8;

	// --- Apply Camera Position with Smoothing ---
	cam_x += (target_cam_x - cam_x) * smooth_factor;
	cam_y += (target_cam_y - cam_y) * smooth_factor;

	game->SetCamPos(cam_x, cam_y);

	// --- Debug Output ---
	DebugOut(L"CamX:%.1f TgtX:%.1f CamY:%.1f TgtY:%.1f BaseY:%.1f SkyP:%d OnP:%d Vx:%.2f Vy:%.2f FeetY:%.1f CenterY:%.1f SmoothedY:%.1f MapH:%.1f CamH:%.1f AbsGndY:%.1f VertLev:%d Smooth:%.2f Falling:%d FallThresh:%.1f\n",
		cam_x, target_cam_x, cam_y, target_cam_y, current_cam_base_y, is_on_sky_platform, is_on_platform, mario_vx, mario_vy, mario_b, mario_center_y, smoothed_center_y, mapHeight, cam_height, absolute_ground_cam_y, is_vertical_level, smooth_factor, is_falling, fall_threshold);
}
void CPlayScene::Update(DWORD dt)
{
	if (player == NULL) return;

	CMario* mario = (CMario*)player;
	CGame* game = CGame::GetInstance();
	float cam_x, cam_y;
	game->GetCamPos(cam_x, cam_y);
	float cam_width = (float)game->GetBackBufferWidth();
	float cam_height = (float)game->GetBackBufferHeight();

	float cx, cy;
	mario->GetPosition(cx, cy);

	vector<LPGAMEOBJECT> coObjects;
	UpdateChunks(cam_x, cam_width); // Load/unload chunks based on current camera
	UpdateObjects(dt, mario, coObjects); // Update all loaded objects

	// Update camera using player pos and loaded scene settings
	UpdateCamera(mario, cx, cy, cam_width, cam_height);

	PurgeDeletedObjects(); // Clean up deleted objects
}

void CPlayScene::Render()
{
	CAttackParticle* attackParticle = player ? ((CMario*)player)->GetTailWhip()->GetAttackParticle() : NULL;

	for (LPCHUNK chunk : chunks)
	{
		vector<LPGAMEOBJECT>& chunkObjects = chunk->GetObjects();
		for (LPGAMEOBJECT obj : chunkObjects)
		{
			if (obj != player && obj != attackParticle)
				obj->Render();
		}
	}

	if (player != NULL)
	{
		player->Render();
		if (attackParticle != NULL)
			attackParticle->Render();
	}
}

void CPlayScene::Clear()
{
	Unload();
}

void CPlayScene::Unload()
{
	for (LPCHUNK chunk : chunks)
	{
		chunk->Clear();
		delete chunk;
	}
	chunks.clear();

	CAnimations::GetInstance()->Clear();
	CSprites::GetInstance()->Clear();

	player = NULL;

	DebugOut(L"[INFO] Scene %d unloaded! \n", id);
}

bool CPlayScene::IsGameObjectDeleted(const LPGAMEOBJECT& o) { return o == NULL; }

void CPlayScene::PurgeDeletedObjects()
{
	for (LPCHUNK chunk : chunks)
	{
		chunk->PurgeDeletedObjects();
	}
}