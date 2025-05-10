#include <iostream>
#include <fstream>
#include <vector>     // Make sure vector is included if not implicitly
#include <string>     // Make sure string is included if not implicitly
#include <cmath>
#include <algorithm>
#include <stdexcept>  // For invalid_argument, out_of_range

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
#include "FallPitch.h"

#include "SampleKeyEventHandler.h"

using namespace std;

// --- Constants ---
#define SCENE_SECTION_UNKNOWN -1
#define SCENE_SECTION_ASSETS	1
#define SCENE_SECTION_SETTINGS 2
#define SCENE_SECTION_MARIO	3
#define SCENE_SECTION_CHUNK_OBJECT 4

#define ASSETS_SECTION_UNKNOWN -1
#define ASSETS_SECTION_SPRITES 1
#define ASSETS_SECTION_ANIMATIONS 2

#define MAX_SCENE_LINE 1024

// Camera specific constants
#define HORIZONTAL_MARGIN 12.0f   // 12px margin on each side of center (middle 24px is free zone)
#define VIEWPORT_X_OFFSET 8.0f
#define VIEWPORT_Y_OFFSET 14.0f

// --- Suggested Z-Index Hierarchy ---
#define ZINDEX_BACKGROUND_EFFECTS   10 // Clouds, distant background elements
#define ZINDEX_BACKGROUND_SCENERY   20 // Bushes, Trees (behind pipes)
#define ZINDEX_PLATFORMS            40 // Ground platforms, Sky Platforms (usually behind pipes/blocks)
#define ZINDEX_PIRANHA_PLANT		45 // Piranha Plants behind pipes.
#define ZINDEX_PIPES                50 // Standard Pipes
#define ZINDEX_MUSHROOM             60 // Mushroom
#define ZINDEX_HIDDEN_COIN			65 // Q-Block Coins
#define ZINDEX_BLOCKS               70 // Bricks, Q-Blocks, Boxes
#define ZINDEX_ITEMS                80 // Coins (when static), Mushrooms, Leaves (after spawning)
#define ZINDEX_ENEMIES              90 // Goombas, Koopas, Piranhas (usually behind player)
#define ZINDEX_PLAYER              100 // Mario
#define ZINDEX_PLAYER_EFFECTS      110 // Tail Whip visual (Mario renders this), maybe held Koopa?
#define ZINDEX_PARTICLES           120 // Attack Particle (rendered by whip/Mario)
#define ZINDEX_FOREGROUND_EFFECTS  150 // Foreground elements, UI overlays (if part of game objects)
// Default (if not specified or unknown)
#define ZINDEX_DEFAULT              75 // Place somewhere in the middle

#define DEPENDENT_ID				9999	// ID for objects those are not listed in text file and initiate via another object initiation

#define SPAWN_CAMERA_BUFFER			20.0f

CPlayScene::CPlayScene(int id, LPCWSTR filePath) :
	CScene(id, filePath)
{
	player = nullptr;
	key_handler = new CSampleKeyHandler(this); // 'this' is the CPlayScene instance
}

// --- Parsing Functions ---

void CPlayScene::_ParseSection_SPRITES(string line)
{
	vector<string> tokens = split(line);
	if (tokens.size() < 6) return;

	try {
		int ID = stoi(tokens[0]);
		int l = stoi(tokens[1]);
		int t = stoi(tokens[2]);
		int r = stoi(tokens[3]);
		int b = stoi(tokens[4]);
		int texID = stoi(tokens[5]);

		LPTEXTURE tex = CTextures::GetInstance()->Get(texID);
		if (tex == nullptr) {
			DebugOut(L"[ERROR] Texture ID %d not found for sprite %d!\n", texID, ID);
			return;
		}
		CSprites::GetInstance()->Add(ID, l, t, r, b, tex);
	}
	catch (const invalid_argument& ia) {
		DebugOut(L"[ERROR] Invalid argument parsing SPRITE line: %hs. Exception: %hs\n", line.c_str(), ia.what());
	}
	catch (const out_of_range& oor) {
		DebugOut(L"[ERROR] Out of range parsing SPRITE line: %hs. Exception: %hs\n", line.c_str(), oor.what());
	}
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
	if (tokens.size() < 3 || tokens.size() % 2 == 0) { // Need ID + pairs of sprite/time
		DebugOut(L"[WARN] Invalid animation line format: %hs\n", line.c_str());
		return;
	}

	try {
		LPANIMATION ani = new CAnimation();
		int ani_id = stoi(tokens[0]);
		for (size_t i = 1; i < tokens.size(); i += 2) {
			int sprite_id = stoi(tokens[i]);
			int frame_time = stoi(tokens[i + 1]);
			// Basic validation
			if (frame_time <= 0) {
				DebugOut(L"[WARN] Invalid frame time %d for animation %d, sprite %d. Setting to 100ms.\n", frame_time, ani_id, sprite_id);
				frame_time = 100; // Default to a sensible value
			}
			// Optional: Check if sprite_id exists? Might be slow.
			ani->Add(sprite_id, frame_time);
		}
		CAnimations::GetInstance()->Add(ani_id, ani);
	}
	catch (const invalid_argument& ia) {
		DebugOut(L"[ERROR] Invalid argument parsing ANIMATION line: %hs. Exception: %hs\n", line.c_str(), ia.what());
	}
	catch (const out_of_range& oor) {
		DebugOut(L"[ERROR] Out of range parsing ANIMATION line: %hs. Exception: %hs\n", line.c_str(), oor.what());
	}
}

void CPlayScene::_ParseSection_SETTINGS(string line)
{
	vector<string> tokens = split(line);
	if (tokens.size() < 2) return;
	string key = tokens[0];
	try {
		float value = stof(tokens[1]); // Use stof for consistency

		if (key == "start_cam_x") startCamX = value;
		else if (key == "start_cam_y") startCamY = value;
		else if (key == "map_width") mapWidth = value;
		else if (key == "map_height") mapHeight = value;
		else if (key == "margin_x") marginX = value; // Keep if might be used later
		else if (key == "margin_y") marginY = value; // Keep if might be used later
		else DebugOut(L"[WARN] Unknown setting key: %hs\n", key.c_str());

	}
	catch (const invalid_argument& ia) {
		DebugOut(L"[ERROR] Invalid argument parsing SETTING line: %hs. Exception: %hs\n", line.c_str(), ia.what());
	}
	catch (const out_of_range& oor) {
		DebugOut(L"[ERROR] Out of range parsing SETTING line: %hs. Exception: %hs\n", line.c_str(), oor.what());
	}
}

void CPlayScene::_ParseSection_MARIO(string line)
{
	vector<string> tokens = split(line);
	if (tokens.size() < 3) { // Need id, x, y
		DebugOut(L"[WARN] Invalid mario line format: %hs\n", line.c_str());
		return;
	}

	int id;
	float x, y;
	try {
		id = stoi(tokens[0]);
		x = stoi(tokens[1]);
		y = stof(tokens[2]);
	}
	catch (const exception& e) {
		DebugOut(L"[ERROR] Failed to parse basic Mario data (type,x,y) for line: %hs. Exception: %hs\n", line.c_str(), e.what());
		return;
	}
	player = (CMario*)(new CMario(id, x, y, ZINDEX_PLAYER));
	return;
}

void CPlayScene::_ParseSection_CHUNK_OBJECTS(string line, LPCHUNK targetChunk)
{
	if (targetChunk == nullptr) {
		DebugOut(L"[FATAL ERROR] _ParseSection_CHUNK_OBJECT called with null targetChunk!\n");
		return;
	}
	vector<string> tokens = split(line);
	if (tokens.size() < 4) {
		DebugOut(L"[WARN] Skipping invalid object line (too few tokens): %hs in chunk %d\n", line.c_str(), targetChunk->GetID());
		return;
	}

	int id;
	int object_type;
	float x, y;
	try {
		id = stoi(tokens[0]);
		object_type = stoi(tokens[1]);
		x = stof(tokens[2]);
		y = stof(tokens[3]);
	}
	catch (const exception& e) {
		DebugOut(L"[ERROR] Failed to parse basic object data (type,id,x,y) for line: %hs in chunk %d. Exception: %hs\n", line.c_str(), targetChunk->GetID(), e.what());
		return;
	}

	targetChunk->SetIsObjectDeleted(id, false);

	if (targetChunk->GetIsObjectDeleted(id)) {
		DebugOut(L"[WARN] Object with ID %d already deleted in chunk %d. Skipping...\n", id, targetChunk->GetID());
		return;
	}

	CGameObject* obj = nullptr;
	int zIndex = ZINDEX_DEFAULT;

	try {
		switch (object_type)
		{
		case OBJECT_TYPE_GOOMBA:
			zIndex = ZINDEX_ENEMIES;
			obj = new CGoomba(id, x, y, zIndex, targetChunk->GetID());
			targetChunk->AddEnemy(obj);
			break;
		case OBJECT_TYPE_KOOPA:
			zIndex = ZINDEX_ENEMIES;
			obj = new CKoopa(id, x, y, zIndex, targetChunk->GetID());
			targetChunk->AddEnemy(obj);
			break;
		case OBJECT_TYPE_PIRANHA_PLANT:
		{
			zIndex = ZINDEX_PIRANHA_PLANT;
			int fireball_zIndex = ZINDEX_FOREGROUND_EFFECTS;
			CFireball* fireball = new CFireball(DEPENDENT_ID, x, y - PIRANHA_PLANT_BBOX_HEIGHT - PIRANHA_PLANT_BBOX_OFFSET, fireball_zIndex);
			obj = new CPiranhaPlant(id, x, y, zIndex, targetChunk->GetID(), fireball);
			targetChunk->AddObject(fireball);
			targetChunk->AddObject(obj);
			targetChunk->AddEnemy(obj);
			return;
		}
		case OBJECT_TYPE_WINGED_GOOMBA:
			zIndex = ZINDEX_ENEMIES;
			obj = new CWingedGoomba(id, x, y, zIndex, targetChunk->GetID());
			targetChunk->AddEnemy(obj);
			break;
		case OBJECT_TYPE_BRICK:
		{
			zIndex = ZINDEX_BLOCKS;
			if (tokens.size() < 7) throw runtime_error("Insufficient params for BRICK");
			float cell_width = stof(tokens[4]);
			float cell_height = stof(tokens[5]);
			int sprite_id = stoi(tokens[6]);
			obj = new CBrick(DEPENDENT_ID, x, y, zIndex, sprite_id);
			break;
		}
		case OBJECT_TYPE_COIN:
		{
			zIndex = ZINDEX_ITEMS;
			if (tokens.size() < 5) throw runtime_error("Insufficient params for COIN");
			int type = stoi(tokens[4]);
			obj = new CCoin(id, x, y, zIndex, targetChunk->GetID(), type);
			targetChunk->SetIsObjectConsumed(obj->GetId(), false);
			if (targetChunk->GetIsObjectConsumed(obj->GetId()))
				return;
			break;
		}
		case OBJECT_TYPE_PLATFORM:
		case OBJECT_TYPE_SKYPLATFORM:
		{
			zIndex = ZINDEX_PLATFORMS;
			if (tokens.size() < 6) throw runtime_error("Insufficient params for PLATFORM/SKYPLATFORM");
			float width = stof(tokens[4]);
			float height = stof(tokens[5]);
			if (object_type == OBJECT_TYPE_PLATFORM)
				obj = new CPlatform(id, x, y, zIndex, width, height);
			else
				obj = new CSkyPlatform(id, x, y, zIndex, width, height);
			break;
		}
		case OBJECT_TYPE_COIN_QBLOCK:
		{
			zIndex = ZINDEX_BLOCKS;
			int coin_zIndex = ZINDEX_HIDDEN_COIN;
			CCoin* coin = new CCoin(DEPENDENT_ID, x, y, coin_zIndex, targetChunk->GetID(), 1);
			obj = new CCoinQBlock(id, x, y, zIndex, targetChunk->GetID(), coin);
			targetChunk->AddObject(coin);
			targetChunk->AddObject(obj);
			targetChunk->SetIsObjectConsumed(obj->GetId(), false);
			if (targetChunk->GetIsObjectConsumed(obj->GetId()))
				((CCoinQBlock*)obj)->SetIsHit(true);
			return;
		}
		case OBJECT_TYPE_BUFF_QBLOCK:
		{
			zIndex = ZINDEX_BLOCKS;
			int leaf_zIndex = ZINDEX_ITEMS;
			int mushroom_zIndex = ZINDEX_MUSHROOM;
			CMushroom* mushroom = new CMushroom(DEPENDENT_ID, x, y, mushroom_zIndex);
			CSuperLeaf* superleaf = new CSuperLeaf(DEPENDENT_ID, x, y, leaf_zIndex);
			obj = new CBuffQBlock(id, x, y, zIndex, targetChunk->GetID(), mushroom, superleaf);
			targetChunk->AddObject(mushroom);
			targetChunk->AddObject(superleaf);
			targetChunk->AddObject(obj);
			targetChunk->SetIsObjectConsumed(obj->GetId(), false);
			if (targetChunk->GetIsObjectConsumed(obj->GetId()))
				((CBuffQBlock*)obj)->SetIsHit(true);
			return;
		}
		case OBJECT_TYPE_LIFE_BRICK:
		{
			zIndex = ZINDEX_BLOCKS;
			int mushroom_zIndex = ZINDEX_MUSHROOM;
			CLifeMushroom* mushroom = new CLifeMushroom(DEPENDENT_ID, x, y, mushroom_zIndex);
			obj = new CLifeBrick(id, x, y, zIndex, targetChunk->GetID(), mushroom);
			targetChunk->AddObject(mushroom);
			targetChunk->AddObject(obj);
			targetChunk->SetIsObjectConsumed(obj->GetId(), false);
			if (targetChunk->GetIsObjectConsumed(obj->GetId()))
				((CLifeBrick*)obj)->SetIsHit(true);
			return;
		}
		case OBJECT_TYPE_BOX:
		{
			zIndex = ZINDEX_BLOCKS;
			if (tokens.size() < 7) throw runtime_error("Insufficient params for BOX");
			float width = stof(tokens[4]);
			float height = stof(tokens[5]);
			int color = stoi(tokens[6]);
			int bottomShadow = (tokens.size() >= 8) ? stoi(tokens[7]) : 0;
			obj = new CBox(id, x, y, zIndex, width, height, color, bottomShadow);
			break;
		}
		case OBJECT_TYPE_TREE:
		case OBJECT_TYPE_PIPE:
		{
			zIndex = (object_type == OBJECT_TYPE_TREE) ? ZINDEX_BACKGROUND_SCENERY : ZINDEX_PIPES;
			if (tokens.size() < 11) throw runtime_error("Insufficient params for TREE/PIPE");
			float cell_width = stof(tokens[4]);
			float cell_height = stof(tokens[5]);
			int height = stoi(tokens[6]);
			int sprite_top_left = stoi(tokens[7]);
			int sprite_top_right = stoi(tokens[8]);
			int sprite_bottom_left = stoi(tokens[9]);
			int sprite_bottom_right = stoi(tokens[10]);

			if (object_type == OBJECT_TYPE_TREE)
				obj = new CTree(id, x, y, zIndex, cell_width, cell_height, height, sprite_top_left, sprite_top_right, sprite_bottom_left, sprite_bottom_right);
			else
				obj = new CPipe(id, x, y, zIndex, cell_width, cell_height, height, sprite_top_left, sprite_top_right, sprite_bottom_left, sprite_bottom_right);
			break;
		}
		case OBJECT_TYPE_BUSH:
		case OBJECT_TYPE_CLOUD:
		{
			zIndex = (object_type == OBJECT_TYPE_BUSH) ? ZINDEX_BACKGROUND_SCENERY : ZINDEX_BACKGROUND_EFFECTS;
			if (tokens.size() < 5) throw runtime_error("Insufficient params for BUSH/CLOUD");
			float width = stof(tokens[4]);
			obj = (object_type == OBJECT_TYPE_BUSH) ? (CGameObject*)new CBush(id, x, y, zIndex, width) : (CGameObject*)new CCloud(id, x, y, zIndex, width);
			break;
		}
		case OBJECT_TYPE_TRINKET:
		{
			zIndex = ZINDEX_BACKGROUND_SCENERY;
			if (tokens.size() < 5) throw runtime_error("Insufficient params for TRINKET");
			int type = stoi(tokens[4]);
			obj = new CTrinket(id, x, y, zIndex, type);
			break;
		}
		case OBJECT_TYPE_PORTAL:
		{
			zIndex = ZINDEX_DEFAULT;
			if (tokens.size() < 7) throw runtime_error("Insufficient params for PORTAL");
			float r = stof(tokens[4]);
			float b = stof(tokens[5]);
			int scene_id = stoi(tokens[6]);
			obj = new CPortal(id, x, y, zIndex, r, b, scene_id);
			break;
		}
		case OBJECT_TYPE_FALL_PITCH:
		{
			if (tokens.size() < 6) {
				DebugOut(L"[WARN] Skipping FALL_PITCH - Insufficient params in chunk %d: %hs\n", targetChunk->GetID(), line.c_str());
				return;
			}
			float r = stof(tokens[4]);
			float b = stof(tokens[5]);
			obj = new CFallPitch(id, x, y, r, b);
			break;
		}
		default:
			DebugOut(L"[WARN] Unhandled object type %d in chunk %d for line: %hs\n", object_type, targetChunk->GetID(), line.c_str());
			return;
		}

		if (obj != nullptr) {
			size_t zIndexTokenPos = 11;
			if (tokens.size() > zIndexTokenPos) {
				try {
					zIndex = stoi(tokens[zIndexTokenPos]);
				}
				catch (const exception& e) {
					DebugOut(L"[WARN] Failed to parse Z-Index for object type %d: %hs. Using default %d.\n", object_type, e.what(), zIndex);
				}
			}
			obj->SetZIndex(zIndex);
			targetChunk->AddObject(obj);
		}
	}
	catch (const exception& e) {
		DebugOut(L"[ERROR] Failed processing object line: %hs in chunk %d. Exception: %hs\n", line.c_str(), targetChunk->GetID(), e.what());
		if (obj) {
			DebugOut(L"[WARN] Object of type %d might have leaked memory due to parsing error.\n", object_type);
		}
	}
}

// --- Loading Functions ---

void CPlayScene::LoadAssets(LPCWSTR assetFile)
{
	DebugOut(L"[INFO] Start loading assets from: %ls\n", assetFile);
	ifstream f(assetFile); // Use constructor for RAII
	if (!f.is_open()) {
		DebugOut(L"[ERROR] Failed to open asset file: %ls\n", assetFile);
		return;
	}

	int section = ASSETS_SECTION_UNKNOWN;
	string line;
	int lineNum = 0;
	while (getline(f, line))
	{
		lineNum++;
		if (line.empty() || line[0] == '#') continue; // Skip empty lines and comments

		if (line == "[SPRITES]") { section = ASSETS_SECTION_SPRITES; continue; }
		if (line == "[ANIMATIONS]") { section = ASSETS_SECTION_ANIMATIONS; continue; }
		if (line[0] == '[') {
			DebugOut(L"[WARN] Unknown section %hs in asset file %ls at line %d\n", line.c_str(), assetFile, lineNum);
			section = ASSETS_SECTION_UNKNOWN; continue;
		}

		switch (section)
		{
		case ASSETS_SECTION_SPRITES: _ParseSection_SPRITES(line); break;
		case ASSETS_SECTION_ANIMATIONS: _ParseSection_ANIMATIONS(line); break;
			// default: // No action needed if section is unknown or not sprites/animations
		}
	}
	// f closes automatically due to RAII
	DebugOut(L"[INFO] Done loading assets from %ls\n", assetFile);
}

void CPlayScene::LoadChunkObjects(int chunk_id, LPCHUNK targetChunk)
{
	// Use stored scene_file_path (wstring)
	ifstream f(scene_file_path); // Use wstring constructor
	if (!f.is_open()) {
		DebugOut(L"[ERROR] Failed to open scene file for chunk loading: %ls\n", scene_file_path.c_str());
		return;
	}

	string line;
	bool in_target_section = false;
	string target_section_header = "[CHUNK\t" + to_string(chunk_id) + "\t"; // Pre-calculate

	while (getline(f, line))
	{
		if (line.empty() || line[0] == '#') continue;

		if (line[0] == '[') {
			// Check if this line starts with the target section header
			in_target_section = (line.rfind(target_section_header, 0) == 0); // Use rfind for prefix check
			continue; // Move to next line after processing section header
		}

		if (in_target_section) {
			_ParseSection_CHUNK_OBJECTS(line, targetChunk);
		}
	}
	// f closes automatically
	targetChunk->SetLoaded(true);
	DebugOut(L"[INFO] Loaded objects for chunk %d\n", chunk_id);
}

void CPlayScene::LoadChunksInRange(float cam_x, float cam_width)
{
	// Consider adding a small buffer if objects slightly outside the view need to be loaded early
	// float load_buffer = chunkWidth; // Example: Load one chunk ahead? Depends on chunk size.
	float load_buffer = CGame::GetInstance()->GetBackBufferWidth() * 0.5f; // Example: Half screen buffer
	float view_left = cam_x - load_buffer;
	float view_right = cam_x + cam_width + load_buffer;

	for (LPCHUNK chunk : chunks) // Use range-based for loop
	{
		// Check if chunk overlaps with the view + buffer and is not already loaded
		if (!chunk->IsLoaded() && chunk->GetEndX() >= view_left && chunk->GetStartX() <= view_right)
		{
			LoadChunkObjects(chunk->GetID(), chunk);
		}
	}
}

void CPlayScene::UnloadChunksOutOfRange(float cam_x, float cam_width) // **** MODIFIED ****
{
	float unload_buffer = CGame::GetInstance()->GetBackBufferWidth() * 0.5f;
	float view_left = cam_x - unload_buffer;
	float view_right = cam_x + cam_width + unload_buffer;

	for (LPCHUNK chunk : chunks)
	{
		if (chunk && chunk->IsLoaded() && (chunk->GetEndX() < view_left || chunk->GetStartX() > view_right))
		{
			chunk->Clear();
			chunk->SetLoaded(false);
			DebugOut(L"[INFO] Unloaded objects for chunk %d\n", chunk->GetID());
		}
	}
}

void CPlayScene::Load()
{
	DebugOut(L"[INFO] Start loading scene from: %ls\n", sceneFilePath);
	ifstream f(sceneFilePath); // Use wstring path
	if (!f.is_open()) {
		DebugOut(L"[ERROR] Failed to open scene file: %ls\n", sceneFilePath);
		return;
	}

	// Reset or default values
	startCamX = 0.0f; startCamY = 0.0f; mapWidth = 0.0f; mapHeight = 0.0f;
	marginX = 0.0f; marginY = 0.0f;
	// current_cam_base_y removed
	player = nullptr; // Ensure player is null before loading
	chunks.clear(); // Clear existing chunks if reloading scene

	int section = SCENE_SECTION_UNKNOWN;
	string line;
	int lineNum = 0;
	currentParsingChunk = nullptr; // Reset parsing chunk pointer

	while (getline(f, line))
	{
		lineNum++;
		// Trim whitespace (optional but good practice)
		//line = trim(line); // Assumes 'trim' exists in Utils.h

		if (line.empty() || line[0] == '#') continue;

		if (line == "[SCENE_SETTINGS]") { section = SCENE_SECTION_SETTINGS; continue; }
		if (line == "[ASSETS]") { section = SCENE_SECTION_ASSETS; continue; }
		if (line.rfind("[CHUNK", 0) == 0) { // Check prefix using rfind
			section = SCENE_SECTION_CHUNK_OBJECT; // Mark section type

			// --- Parse Chunk Header ---
			size_t closeBracketPos = line.find(']');
			if (closeBracketPos != string::npos) {
				string chunkHeader = line.substr(1, closeBracketPos - 1);
				vector<string> tokens = split(chunkHeader);
				if (tokens.size() >= 4 && tokens[0] == "CHUNK") {
					try {
						int chunkId = stoi(tokens[1]);
						float chunk_Start_X = stof(tokens[2]);
						float chunk_End_X = stof(tokens[3]);

						if (GetChunk(chunkId) == nullptr) {
							LPCHUNK chunk = new CChunk(chunkId, chunk_Start_X, chunk_End_X);
							chunks.push_back(chunk);
							currentParsingChunk = chunk; // Set for subsequent object lines
							DebugOut(L"[INFO] Registered chunk %d (%f, %f)\n", chunkId, chunk_Start_X, chunk_End_X);
						}
						else {
							DebugOut(L"[WARN] Chunk %d already exists, attempting to reuse (potential issue?).\n", chunkId);
							currentParsingChunk = GetChunk(chunkId); // Re-use existing
						}
					}
					catch (const exception& e) {
						DebugOut(L"[ERROR] Failed to parse chunk header: %hs. Exception: %hs\n", line.c_str(), e.what());
						currentParsingChunk = nullptr;
					}
				}
				else {
					DebugOut(L"[ERROR] Malformed chunk header: %hs\n", line.c_str());
					currentParsingChunk = nullptr;
				}
			}
			else {
				DebugOut(L"[ERROR] Malformed chunk header (no closing bracket): %hs\n", line.c_str());
				currentParsingChunk = nullptr;
			}
			continue; // Move to next line after processing chunk header
		}
		if (line == "[MARIO]") { section = SCENE_SECTION_MARIO; continue; }
		// If it's another section type, reset currentParsingChunk
		if (line[0] == '[') {
			section = SCENE_SECTION_UNKNOWN;
			currentParsingChunk = nullptr;
			DebugOut(L"[WARN] Unknown section %hs in scene file at line %d\n", line.c_str(), lineNum);
			continue;
		}

		// Process line based on current section
		switch (section)
		{
		case SCENE_SECTION_ASSETS:   _ParseSection_ASSETS(line); break;
		case SCENE_SECTION_SETTINGS: _ParseSection_SETTINGS(line); break;
		case SCENE_SECTION_MARIO: _ParseSection_MARIO(line); break;
		case SCENE_SECTION_CHUNK_OBJECT:
			// Objects are loaded later via LoadChunkObjects, skip here
			// We only parse the chunk headers during this initial pass.
			break;
			// No default needed, handled by unknown section check
		}
	}
	// f closes automatically

	scene_file_path = sceneFilePath; // Store for LoadChunkObjects

	// --- Get Screen Dimensions ---
	CGame* game = CGame::GetInstance();
	const float cam_width = (float)game->GetBackBufferWidth();
	const float cam_height = (float)game->GetBackBufferHeight();

	game->SetCamPos(startCamX, startCamY);
	DebugOut(L"[INFO] Initial camera set to (%.1f, %.1f)\n", startCamX, startCamY);

	// --- Load Initial Chunks ---
	DebugOut(L"[INFO] Loading initial chunks around camera...\n");
	LoadChunksInRange(startCamX, cam_width);

	DebugOut(L"[INFO] Scene loading complete for %ls\n", sceneFilePath);
}

// --- Update Functions ---

void CPlayScene::UpdateChunks(float cam_x, float cam_width)
{
	// These seem okay, buffers can be tuned if needed
	LoadChunksInRange(cam_x, cam_width);
	UnloadChunksOutOfRange(cam_x, cam_width);
}

//void CPlayScene::UpdateObjects(DWORD dt, CMario* mario, vector<LPGAMEOBJECT>& coObjects)
//{
//	// 1. Collect potential colliders (excluding player)
//	coObjects.clear(); // Clear from previous frame
//	for (LPCHUNK chunk : chunks) {
//		if (!chunk->IsLoaded()) continue; // Optimization: Skip unloaded chunks
//		vector<LPGAMEOBJECT>& chunkObjects = chunk->GetObjects();
//		for (LPGAMEOBJECT obj : chunkObjects) {
//			if (obj != mario) // Simple exclusion
//				coObjects.push_back(obj);
//		}
//	}
//
//	// 2. Update all loaded objects (including player within its chunk iteration)
//	// Check for time freeze effect
//	bool isChronoStopped = mario->GetIsPowerUp() || mario->GetIsTailUp() ||
//		mario->GetIsPowerDown() || mario->GetIsTailDown();
//
//	for (LPCHUNK chunk : chunks) {
//		if (!chunk->IsLoaded()) continue; // Optimization: Skip unloaded chunks
//		// Important: Iterate over a *copy* if obj->Update might modify the chunk's object list (e.g., spawning items)
//		// However, since we purge deleted objects later, iterating the original might be okay if adds happen at end.
//		// Safest is often a copy, but less performant. Sticking with original for now based on existing code.
//		vector<LPGAMEOBJECT>& chunkObjects = chunk->GetObjects(); // Get reference
//		for (size_t i = 0; i < chunkObjects.size(); ++i) { // Use index loop if Update might invalidate iterators
//			LPGAMEOBJECT obj = chunkObjects[i];
//			if (obj == nullptr) continue; // Skip if already marked for deletion? (Purge handles this later)
//
//			// Skip updates if time is stopped (except for Mario himself)
//			if (isChronoStopped && obj != mario) {
//				continue;
//			}
//
//			obj->Update(dt, &coObjects); // Pass the collected colliders
//		}
//	}
//}

void CPlayScene::UpdateCamera(CMario* mario, float player_cx, float player_cy, float cam_width, float cam_height) {
	if (!mario) return; // Safety check

	CGame* game = CGame::GetInstance();
	float cam_x, cam_y;
	game->GetCamPos(cam_x, cam_y); // Current camera position

	float mario_x, mario_y;
	mario->GetPosition(mario_x, mario_y); // Mario's world position

	// Initialize static state
	static bool s_isLockedToGround = true;
	float visible_world_cam_height = cam_height - HUD_BACKGROUND_HEIGHT + VIEWPORT_Y_OFFSET / 2;

	// Determine if Mario is flying with Tail
	bool is_Racoon_and_MaxPMeter = (mario->GetLevel() == MARIO_LEVEL_TAIL && mario->GetPMeter() == 1.0f);
	//bool is_Racoon_and_MaxPMeter = (mario->GetLevel() == MARIO_LEVEL_TAIL && mario->MaxPMeter());
	bool cameraTouchedGround = cam_y >= (mapHeight - visible_world_cam_height);

	// Update lock state
	if (is_Racoon_and_MaxPMeter) {
		s_isLockedToGround = false; // Unlock camera when max P meter
	}
	else if (cameraTouchedGround) {
		s_isLockedToGround = true; // Lock camera when on ground
	}

	// Calculate target horizontal position with margin
	float desiredCamX = mario_x - (cam_width / 2.0f);
	float minCamXDeadZone = cam_x - HORIZONTAL_MARGIN;
	float maxCamXDeadZone = cam_x + HORIZONTAL_MARGIN;
	float targetCamX = max(minCamXDeadZone, min(desiredCamX, maxCamXDeadZone));

	// Calculate target vertical position
	float targetCamY;
	if (s_isLockedToGround) {
		// Lock to ground level
		targetCamY = (mapHeight > visible_world_cam_height) ? (mapHeight - visible_world_cam_height) : 0.0f;
		targetCamY = max(0.0f, targetCamY);
	}
	else {
		// Center on Mario vertically when unlocked
		targetCamY = mario_y - (cam_height / 2.0f);
	}

	// Clamp camera to map boundaries
	float minCamX = -VIEWPORT_X_OFFSET;
	float maxCamX = (mapWidth > cam_width) ? (mapWidth - cam_width - VIEWPORT_X_OFFSET) : minCamX;
	maxCamX = max(minCamX, maxCamX);
	float minCamY = 0.0f;
	float maxCamY = (mapHeight > visible_world_cam_height) ? (mapHeight - visible_world_cam_height) : minCamY;
	maxCamY = max(minCamY, maxCamY);

	cam_x = max(minCamX, min(targetCamX, maxCamX));
	cam_y = max(minCamY, min(targetCamY, maxCamY));

	// Set final camera position
	game->SetCamPos(cam_x, cam_y);

	// Debug output
	//DebugOut(L"Cam pos: %f, %f\n", cam_x, cam_y);
}

void CPlayScene::Update(DWORD dt)
{
	if (player == nullptr) {
		return;
	}

	CMario* mario = (CMario*)player;
	CGame* game = CGame::GetInstance();

	float cam_width = (float)game->GetBackBufferWidth();
	float cam_height = (float)game->GetBackBufferHeight();
	float current_cam_x, current_cam_y;
	game->GetCamPos(current_cam_x, current_cam_y);

	// Update Chunks based on current camera
	UpdateChunks(current_cam_x, cam_width);

	// --- Build the Comprehensive coObjects list for this frame ---
	vector<LPGAMEOBJECT> coObjects;
	coObjects.clear(); // Start fresh

	// 1. Add all objects from loaded chunks
	for (LPCHUNK chunk : chunks) {
		if (!chunk->IsLoaded()) continue;
		vector<LPGAMEOBJECT>& chunkObjects = chunk->GetObjects();
		for (LPGAMEOBJECT obj : chunkObjects) {
			if (obj && !obj->IsDeleted()) // Check if object is valid and not deleted
				coObjects.push_back(obj);
		}
	}

	// 2. Add Mario's ACTIVE TailWhip (if any) to the list
	CTailWhip* activeWhip = mario->GetActiveTailWhip();
	if (activeWhip != nullptr) {
		coObjects.push_back(activeWhip);
	}

	// --- Update Player ---
	// Pass the comprehensive list (including the whip if active)
	player->Update(dt, &coObjects);

	// --- Update all OTHER game objects within chunks ---
	bool isChronoStopped = mario->GetIsPowerUp() || mario->GetIsTailUp() 
		|| mario->GetIsPowerDown() || mario->GetIsTailDown() || (mario->GetState() == MARIO_STATE_DIE)
		|| (mario->GetState() == MARIO_STATE_DIE_ON_FALLING);

	for (LPCHUNK chunk : chunks) {
		if (!chunk->IsLoaded()) continue;
		vector<LPGAMEOBJECT>& chunkObjects = chunk->GetObjects();
		// Iterate carefully, as Update might delete objects or add new ones
		// Using index loop is safer if list can change during iteration
		for (size_t i = 0; i < chunkObjects.size(); ++i) {
			LPGAMEOBJECT obj = chunkObjects[i];
			// Check if obj is still valid (might have been deleted by player update?)
			// A robust system might queue deletions instead of immediate delete.
			// Assuming objects marked deleted are handled by Purge later.
			if (obj == nullptr || obj->IsDeleted()) continue;

			// Skip updates if time is stopped
			if (isChronoStopped) continue; // Player already updated

			// Pass the comprehensive list
			obj->Update(dt, &coObjects);
		}
	}

	// --- Update Camera ---
	float player_cx, player_cy;
	mario->GetPosition(player_cx, player_cy);
	UpdateCamera(mario, player_cx, player_cy, cam_width, cam_height);

	DefeatEnemiesOutOfRange();
	RespawnEnemiesInRange();

	// --- Clean up ---
	PurgeDeletedObjects(); // Handle objects marked for deletion
}


void CPlayScene::Render()
{
	vector<LPGAMEOBJECT> renderList;
	renderList.reserve(100);

	for (LPCHUNK chunk : chunks) {
		if (chunk && chunk->IsLoaded()) {
			vector<LPGAMEOBJECT>& chunkObjects = chunk->GetObjects();
			for (LPGAMEOBJECT obj : chunkObjects) {
				if (obj != nullptr && !obj->IsDeleted()) {
					renderList.push_back(obj);
				}
			}
		}
	}

	if (player != nullptr && !player->IsDeleted()) {
		renderList.push_back(player);
	}

	stable_sort(renderList.begin(), renderList.end(),
		[](LPGAMEOBJECT a, LPGAMEOBJECT b) {
			return a->GetZIndex() < b->GetZIndex();
		});

	for (LPGAMEOBJECT obj : renderList) {
		obj->Render();
	}
}

void CPlayScene::Unload()
{
	DebugOut(L"[INFO] Unloading scene %d...\n", id);
	// Unload objects chunk by chunk
	for (LPCHUNK chunk : chunks) {
		if (chunk) { // Check if chunk pointer is valid
			chunk->Clear(); // Clear objects within the chunk
			delete chunk;   // Delete the chunk itself
		}
	}
	chunks.clear(); // Clear the vector of pointers

	// Clear Assets (Important!) - Should be done *carefully* if assets are shared between scenes
	// Assuming CAnimations/CSprites are scene-specific or managed elsewhere appropriately.
	// If they are truly global singletons used across scenes, clearing them here is WRONG.
	// If they are scene-specific asset managers, clearing here is correct.
	// Based on name "GetInstance", they seem global. Let's assume assets are managed elsewhere
	// and this scene doesn't own their lifetime beyond loading them.
	// CAnimations::GetInstance()->Clear(); // POTENTIALLY DANGEROUS if global
	// CSprites::GetInstance()->Clear();    // POTENTIALLY DANGEROUS if global

	// Reset player pointer
	player = nullptr;

	DebugOut(L"[INFO] Scene %d unloaded.\n", id);
}

void CPlayScene::DefeatEnemiesOutOfRange()
{
	const float DEFEAT_RANGE = SPAWN_CAMERA_BUFFER;

	float camStartX, camStartY;
	CGame::GetInstance()->GetCamPos(camStartX, camStartY);
	float camEndX = camStartX + CGame::GetInstance()->GetBackBufferWidth();
	vector<LPCHUNK> loadedChunks = GetLoadedChunks();
	vector<LPGAMEOBJECT> allEnemies;

	// Lấy tất cả các enemy từ các chunk đã load
	for (LPCHUNK chunk : loadedChunks) {
		const vector<LPGAMEOBJECT>& enemies = chunk->GetEnemies();
		allEnemies.insert(allEnemies.end(), enemies.begin(), enemies.end());
	}

	// Duyệt qua tất cả các enemy
	for (LPGAMEOBJECT enemy : allEnemies)
	{
		if (enemy == nullptr || enemy->IsDeleted()) continue;

		float eX, eY;
		enemy->GetPosition(eX, eY);

		// Nếu enemy nằm ngoài phạm vi camera + defeat range
		if (eX < camStartX - DEFEAT_RANGE || eX > camEndX + DEFEAT_RANGE)
		{
			// Kiểm tra loại enemy cụ thể và đánh bại nó
			if (CGoomba* goomba = dynamic_cast<CGoomba*>(enemy))
			{
				if (!goomba->GetIsDefeated())
					goomba->SetIsDefeated(true);
			}
			else if (CKoopa* koopa = dynamic_cast<CKoopa*>(enemy))
			{
				if (!koopa->GetIsDefeated())
					koopa->SetIsDefeated(true);
				float koopa_x, koopa_y;
				koopa->GetPosition(koopa_x, koopa_y);
				vector<LPCHUNK> chunks = GetLoadedChunks();
				for (LPCHUNK chunk : chunks)
				{
					if (chunk->GetID() != koopa->GetOriginalChunkId() && chunk->GetStartX() <= koopa_x && chunk->GetEndX() >= koopa_x)
					{
						chunk->RemoveObject(koopa);
						koopa->Delete();
						break;
					}
				}
			}
			else if (CPiranhaPlant* piranha = dynamic_cast<CPiranhaPlant*>(enemy))
			{
				if (!piranha->GetIsDefeated())
					piranha->SetIsDefeated(true);
			}
			else if (CWingedGoomba* wingedGoomba = dynamic_cast<CWingedGoomba*>(enemy))
			{
				if (!wingedGoomba->GetIsDefeated())
					wingedGoomba->SetIsDefeated(true);
			}
		}
	}
}

void CPlayScene::RespawnEnemiesInRange()
{
	const float RESPAWN_RANGE_MIN = SPAWN_CAMERA_BUFFER;
	const float RESPAWN_RANGE_MAX = SPAWN_CAMERA_BUFFER + 80.0f;

	float camStartX, camStartY;
	CGame::GetInstance()->GetCamPos(camStartX, camStartY);
	float camEndX = camStartX + CGame::GetInstance()->GetBackBufferWidth();
	vector<LPCHUNK> loadedChunks = GetLoadedChunks();
	vector<LPGAMEOBJECT> allEnemies;

	for (LPCHUNK chunk : loadedChunks) {
		const vector<LPGAMEOBJECT>& enemies = chunk->GetEnemies();
		allEnemies.insert(allEnemies.end(), enemies.begin(), enemies.end());
	}

	for (LPGAMEOBJECT enemy : allEnemies)
	{
		if (enemy == nullptr || enemy->IsDeleted()) continue;

		float eX0, eY0;

		auto shouldRespawn = [&](float posX) {
			bool left = posX > camStartX - RESPAWN_RANGE_MAX && posX <= camStartX - RESPAWN_RANGE_MIN;
			bool right = posX >= camEndX + RESPAWN_RANGE_MIN && posX < camEndX + RESPAWN_RANGE_MAX;
			return left || right;
			};

		if (CGoomba* goomba = dynamic_cast<CGoomba*>(enemy))
		{
			goomba->GetOriginalPosition(eX0, eY0);
			if (goomba->GetIsDefeated() && shouldRespawn(eX0))
			{
				LPCHUNK originalChunk = GetChunk(goomba->GetOriginalChunkId());
				if (originalChunk)
				{
					originalChunk->RemoveObject(goomba);
					CGoomba* newGoomba = new CGoomba(goomba->GetId(), eX0, eY0, goomba->GetZIndex(), goomba->GetOriginalChunkId());
					originalChunk->AddObject(newGoomba);
					originalChunk->AddEnemy(newGoomba);
				}
			}
		}
		else if (CKoopa* koopa = dynamic_cast<CKoopa*>(enemy))
		{
			koopa->GetOriginalPosition(eX0, eY0);

			// --- Prevent respawn if a Koopa with the same ID exists in any loaded chunk (except original chunk) ---
			bool koopaExistsInOtherChunk = false;
			for (LPCHUNK chunk : loadedChunks)
			{
				if (chunk->GetID() == koopa->GetOriginalChunkId()) continue;
				const vector<LPGAMEOBJECT>& chunkEnemies = chunk->GetEnemies();
				for (LPGAMEOBJECT obj : chunkEnemies)
				{
					CKoopa* otherKoopa = dynamic_cast<CKoopa*>(obj);
					if (otherKoopa && otherKoopa->GetId() == koopa->GetId())
					{
						koopaExistsInOtherChunk = true;
						break;
					}
				}
				if (koopaExistsInOtherChunk) break;
			}
			if (koopaExistsInOtherChunk) continue;

			if (koopa->GetIsDefeated() && shouldRespawn(eX0))
			{
				LPCHUNK originalChunk = GetChunk(koopa->GetOriginalChunkId());
				if (originalChunk)
				{
					originalChunk->RemoveObject(koopa);
					CKoopa* newKoopa = new CKoopa(koopa->GetId(), eX0, eY0, koopa->GetZIndex(), koopa->GetOriginalChunkId());
					originalChunk->AddObject(newKoopa);
					originalChunk->AddEnemy(newKoopa);
				}
			}
		}
		else if (CPiranhaPlant* piranha = dynamic_cast<CPiranhaPlant*>(enemy))
		{
			piranha->GetOriginalPosition(eX0, eY0);
			if (piranha->GetIsDefeated() && shouldRespawn(eX0))
			{
				LPCHUNK originalChunk = GetChunk(piranha->GetOriginalChunkId());
				if (originalChunk)
				{
					originalChunk->RemoveObject(piranha);
					CPiranhaPlant* newPiranha = new CPiranhaPlant(piranha->GetId(), eX0, eY0, piranha->GetZIndex(), piranha->GetOriginalChunkId(), piranha->GetFireball());
					originalChunk->AddObject(newPiranha);
					originalChunk->AddEnemy(newPiranha);
				}
			}
		}
		else if (CWingedGoomba* wingedGoomba = dynamic_cast<CWingedGoomba*>(enemy))
		{
			wingedGoomba->GetOriginalPosition(eX0, eY0);
			if (wingedGoomba->GetIsDefeated() && shouldRespawn(eX0))
			{
				LPCHUNK originalChunk = GetChunk(wingedGoomba->GetOriginalChunkId());
				if (originalChunk)
				{
					originalChunk->RemoveObject(wingedGoomba);
					CWingedGoomba* newWingedGoomba = new CWingedGoomba(wingedGoomba->GetId(), eX0, eY0, wingedGoomba->GetZIndex(), wingedGoomba->GetOriginalChunkId());
					originalChunk->AddObject(newWingedGoomba);
					originalChunk->AddEnemy(newWingedGoomba);
				}
			}
		}
	}
}

// Keep IsGameObjectDeleted as is, used by PurgeDeletedObjects
bool CPlayScene::IsGameObjectDeleted(const LPGAMEOBJECT& o) { return o == nullptr || o->IsDeleted(); } // Add IsDeleted check

void CPlayScene::PurgeDeletedObjects()
{
	for (LPCHUNK chunk : chunks) {
		if (chunk && chunk->IsLoaded()) { // Check if chunk is valid and loaded
			chunk->PurgeDeletedObjects(); // Delegate purging to the chunk
		}
	}
}