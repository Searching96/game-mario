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
#define VERTICAL_GROUND_LOCK_BUFFER 48.0f
#define HORIZONTAL_DEAD_ZONE_THRESHOLD 12.0f // Was 16.0f, user changed to 12.0f
#define CAMERA_BOUNDARY_MARGIN -8.0f
#define DEFAULT_HORIZONTAL_SMOOTH 0.3f
#define DEFAULT_VERTICAL_SMOOTH 0.3f

// --- Suggested Z-Index Hierarchy ---
#define ZINDEX_BACKGROUND_EFFECTS   10 // Clouds, distant background elements
#define ZINDEX_BACKGROUND_SCENERY   20 // Bushes, Trees (behind pipes)
#define ZINDEX_PLATFORMS            40 // Ground platforms, Sky Platforms (usually behind pipes/blocks)
#define ZINDEX_PIRANHA_PLANT		45 // Piranha Plants behind pipes.
#define ZINDEX_PIPES                50 // Standard Pipes
#define ZINDEX_BLOCKS               60 // Bricks, Q-Blocks, Boxes
#define ZINDEX_ITEMS                80 // Coins (when static), Mushrooms, Leaves (after spawning)
#define ZINDEX_ENEMIES              90 // Goombas, Koopas, Piranhas (usually behind player)
#define ZINDEX_PLAYER              100 // Mario
#define ZINDEX_PLAYER_EFFECTS      110 // Tail Whip visual (Mario renders this), maybe held Koopa?
#define ZINDEX_PARTICLES           120 // Attack Particle (rendered by whip/Mario)
#define ZINDEX_FOREGROUND_EFFECTS  150 // Foreground elements, UI overlays (if part of game objects)
// Default (if not specified or unknown)
#define ZINDEX_DEFAULT              75 // Place somewhere in the middle


CPlayScene::CPlayScene(int id, LPCWSTR filePath) :
	CScene(id, filePath)
{
	player = NULL;
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
		if (tex == NULL) {
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
	if (tokens.size() < 2) { // Need coords X and Y
		DebugOut(L"[WARN] Invalid mario line format: %hs\n", line.c_str());
		return;
	}

	float x, y;
	try {
		x = stoi(tokens[0]);
		y = stof(tokens[1]);
	}
	catch (const exception& e) {
		DebugOut(L"[ERROR] Failed to parse basic Mario data (type,x,y) for line: %hs. Exception: %hs\n", line.c_str(), e.what());
		return;
	}
	player = (CMario*)(new CMario(x, y, ZINDEX_PLAYER));
	return;
}

void CPlayScene::_ParseSection_CHUNK_OBJECTS(string line, LPCHUNK targetChunk)
{
	if (targetChunk == nullptr) {
		DebugOut(L"[FATAL ERROR] _ParseSection_CHUNK_OBJECT called with null targetChunk!\n");
		return;
	}
	vector<string> tokens = split(line);
	if (tokens.size() < 3) { // Min: type, x, y
		DebugOut(L"[WARN] Skipping invalid object line (too few tokens): %hs in chunk %d\n", line.c_str(), targetChunk->GetID());
		return;
	}

	// Use stoi/stof with try-catch for robustness
	int object_type;
	float x, y;
	try {
		object_type = stoi(tokens[0]);
		x = stof(tokens[1]);
		y = stof(tokens[2]);
	}
	catch (const exception& e) {
		DebugOut(L"[ERROR] Failed to parse basic object data (type,x,y) for line: %hs in chunk %d. Exception: %hs\n", line.c_str(), targetChunk->GetID(), e.what());
		return;
	}

	CGameObject* obj = NULL;
	int zIndex = ZINDEX_DEFAULT; // Start with a default

	try {
		switch (object_type)
		{
		case OBJECT_TYPE_GOOMBA:
		{
			zIndex = ZINDEX_ENEMIES;
			obj = new CGoomba(x, y, zIndex); break;
		}
		case OBJECT_TYPE_KOOPA:
		{
			zIndex = ZINDEX_ENEMIES;
			obj = new CKoopa(x, y, zIndex); break;
		}
		case OBJECT_TYPE_PIRANHA_PLANT:
		{
			zIndex = ZINDEX_PIRANHA_PLANT;
			int fireball_zIndex = ZINDEX_FOREGROUND_EFFECTS;
			CFireball* fireball = new CFireball(x, y - PIRANHA_PLANT_BBOX_HEIGHT - PIRANHA_PLANT_BBOX_OFFSET, fireball_zIndex);
			obj = new CPiranhaPlant(x, y, zIndex, fireball);
			targetChunk->AddObject(fireball);
			targetChunk->AddObject(obj);
			return;
		}
		case OBJECT_TYPE_WINGED_GOOMBA:
		{
			zIndex = ZINDEX_ENEMIES;
			obj = new CWingedGoomba(x, y, zIndex);
			break;
		}
		case OBJECT_TYPE_BRICK:
		{
			zIndex = ZINDEX_BLOCKS;
			if (tokens.size() < 6) throw runtime_error("Insufficient params for BRICK");
			float cell_width = stof(tokens[3]);
			float cell_height = stof(tokens[4]);
			int sprite_id = stoi(tokens[5]);
			obj = new CBrick(x, y, zIndex, sprite_id);
			break;
		}
		case OBJECT_TYPE_COIN:
		{
			zIndex = ZINDEX_ITEMS;
			if (tokens.size() < 4) throw runtime_error("Insufficient params for COIN");
			int type = stoi(tokens[3]);
			obj = new CCoin(x, y, zIndex, type);
			break;
		}
		case OBJECT_TYPE_PLATFORM:
		{
			zIndex = ZINDEX_PLATFORMS;
			if (tokens.size() < 5) throw runtime_error("Insufficient params for PLATFORM");
			float width = stof(tokens[3]);
			float height = stof(tokens[4]);
			obj = new CPlatform(x, y, zIndex, width, height);
			break;
		}
		case OBJECT_TYPE_SKYPLATFORM:
		{
			zIndex = ZINDEX_PLATFORMS;
			if (tokens.size() < 5) throw runtime_error("Insufficient params for SKYPLATFORM");
			float width = stof(tokens[3]);
			float height = stof(tokens[4]);
			obj = new CSkyPlatform(x, y, zIndex, width, height);
			break;
		}
		case OBJECT_TYPE_COIN_QBLOCK:
		{
			zIndex = ZINDEX_BLOCKS;
			int coin_zIndex = ZINDEX_ITEMS;
			CCoin* coin = new CCoin(x, y, coin_zIndex, 1); // Coin type 1 for QBlock?
			obj = new CCoinQBlock(x, y, zIndex, coin);
			targetChunk->AddObject(coin);
			targetChunk->AddObject(obj);
			return; // Handled
		}
		case OBJECT_TYPE_BUFF_QBLOCK:
		{
			zIndex = ZINDEX_BLOCKS;
			int buff_zIndex = ZINDEX_ITEMS;
			CMushroom* mushroom = new CMushroom(x, y, buff_zIndex);
			CSuperLeaf* superleaf = new CSuperLeaf(x, y, buff_zIndex);
			obj = new CBuffQBlock(x, y, zIndex, mushroom, superleaf);
			targetChunk->AddObject(mushroom);
			targetChunk->AddObject(superleaf);
			targetChunk->AddObject(obj);
			return; // Handled
		}
		case OBJECT_TYPE_LIFE_BRICK:
		{
			zIndex = ZINDEX_BLOCKS;
			int mushroom_zIndex = ZINDEX_ITEMS;
			CLifeMushroom* mushroom = new CLifeMushroom(x, y, mushroom_zIndex);
			obj = new CLifeBrick(x, y, zIndex, mushroom);
			targetChunk->AddObject(mushroom);
			targetChunk->AddObject(obj);
			return; // Handled
		}
		case OBJECT_TYPE_BOX:
		{
			zIndex = ZINDEX_BLOCKS;
			if (tokens.size() < 6) throw runtime_error("Insufficient params for BOX");
			float width = stof(tokens[3]);
			float height = stof(tokens[4]);
			int color = stoi(tokens[5]);
			int bottomShadow = (tokens.size() >= 7) ? stoi(tokens[6]) : 0; // Optional param
			obj = new CBox(x, y, zIndex, width, height, color, bottomShadow);
			break;
		}
		case OBJECT_TYPE_TREE:
		{
			zIndex = ZINDEX_BACKGROUND_SCENERY;
			if (tokens.size() < 10) throw runtime_error("Insufficient params for TREE");
			float cell_width = stof(tokens[3]);
			float cell_height = stof(tokens[4]);
			int height = stoi(tokens[5]);
			int sprite_top_left = stoi(tokens[6]);
			int sprite_top_right = stoi(tokens[7]);
			int sprite_bottom_left = stoi(tokens[8]);
			int sprite_bottom_right = stoi(tokens[9]);
			obj = new CTree(x, y, zIndex, cell_width, cell_height, height, sprite_top_left, sprite_top_right, sprite_bottom_left, sprite_bottom_right);
			break;
		}
		case OBJECT_TYPE_PIPE: // Shared logic for grid-based sprites
		{
			zIndex = ZINDEX_PIPES;
			if (tokens.size() < 10) throw runtime_error("Insufficient params for PIPE");
			float cell_width = stof(tokens[3]);
			float cell_height = stof(tokens[4]);
			int height = stoi(tokens[5]);
			int sprite_top_left = stoi(tokens[6]);
			int sprite_top_right = stoi(tokens[7]);
			int sprite_bottom_left = stoi(tokens[8]);
			int sprite_bottom_right = stoi(tokens[9]);
			obj = new CPipe(x, y, zIndex, cell_width, cell_height, height, sprite_top_left, sprite_top_right, sprite_bottom_left, sprite_bottom_right);
			break;
		}
		case OBJECT_TYPE_BUSH:
		{
			zIndex = ZINDEX_BACKGROUND_SCENERY;
			if (tokens.size() < 4) throw runtime_error("Insufficient params for BUSH");
			float width = stof(tokens[3]);
			obj = new CBush(x, y, zIndex, width);
			break;
		}
		case OBJECT_TYPE_CLOUD: // Shared logic for simple width-based sprites
		{
			zIndex = ZINDEX_BACKGROUND_EFFECTS;
			if (tokens.size() < 4) throw runtime_error("Insufficient params for CLOUD");
			float width = stof(tokens[3]);
			obj = new CCloud(x, y, zIndex, width);
			break;
		}
		case OBJECT_TYPE_TRINKET:
		{
			zIndex = ZINDEX_BACKGROUND_SCENERY;
			if (tokens.size() < 4) throw runtime_error("Insufficient params for TRINKET");
			int type = stoi(tokens[3]); // Assuming type is float? Could be int.
			obj = new CTrinket(x, y, zIndex, type);
			break;
		}
		case OBJECT_TYPE_PORTAL:
		{
			zIndex = ZINDEX_DEFAULT;
			if (tokens.size() < 6) throw runtime_error("Insufficient params for PORTAL");
			float r = stof(tokens[4]); // Correct index? Should be 3 & 4 for r, b
			float b = stof(tokens[5]); // Check portal definition expected order
			int scene_id = stoi(tokens[6]); // Assuming r,b,scene_id order
			// TODO: Double check CPortal constructor argument order and token indices!
			// Assuming CPortal(x, y, zIndex, right, bottom, scene_id)
			obj = new CPortal(x, y, zIndex, stof(tokens[3]), stof(tokens[4]), stoi(tokens[5]));
			break;
		}
		// Remove cases for base items if they are only created via QBlocks
		// case OBJECT_TYPE_MUSHROOM: // Only created via BuffQBlock/LifeBrick now
		// case OBJECT_TYPE_SUPERLEAF: // Only created via BuffQBlock now
		// case OBJECT_TYPE_QUESTION_BLOCK: // Abstract? Use CoinQBlock/BuffQBlock

		default:
			DebugOut(L"[WARN] Unhandled object type %d in chunk %d for line: %hs\n", object_type, targetChunk->GetID(), line.c_str());
			return; // Don't add unknown objects
		}
		// Add the object to the chunk if it was created and not handled by a special case return
		if (obj != nullptr) {
			// Optional: Allow override from file
			size_t zIndexTokenPos = 10; // Adjust this based on longest param list + 1
			if (tokens.size() > zIndexTokenPos) {
				try {
					zIndex = stoi(tokens[zIndexTokenPos]); // Override default if specified
				}
				catch (const std::exception& e) {
					DebugOut(L"[WARN] Failed to parse Z-Index for object type %d: %hs. Using default %d.\n", object_type, e.what(), zIndex);
				}
			}
			obj->SetZIndex(zIndex); // Set the determined Z-index
			targetChunk->AddObject(obj);
		}
	}
	catch (const exception& e) {
		// Catch errors from stoi/stof or runtime_error for insufficient params
		DebugOut(L"[ERROR] Failed processing object line: %hs in chunk %d. Exception: %hs\n", line.c_str(), targetChunk->GetID(), e.what());
		if (obj) { // Clean up partially created object if exception happened after 'new'
			//delete obj; // This is risky if AddObject took ownership! Best practice: use smart pointers or ensure AddObject handles cleanup on failure. For now, just log.
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
	float unload_buffer = CGame::GetInstance()->GetBackBufferWidth() * 1.5f;
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
	player = NULL; // Ensure player is null before loading
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

	// --- Initialize Camera ---
	CGame* game = CGame::GetInstance();
	float cam_width = (float)game->GetBackBufferWidth();
	float cam_height = (float)game->GetBackBufferHeight();

	// Calculate initial ground cam Y (if possible)
	float initial_cam_y = 0.0f;
	if (mapHeight > 0) {
		initial_cam_y = mapHeight - cam_height + CAMERA_BOUNDARY_MARGIN; // Use constant
		if (initial_cam_y < 0) initial_cam_y = 0;
	}
	// Allow override from settings file if provided
	if (startCamY != 0.0f) { // Assuming 0.0f means not set
		initial_cam_y = startCamY;
	}
	float initial_cam_x = startCamX; // Use setting directly

	// Clamp initial camera position
	// Use max/min for clarity (requires <algorithm>)
	initial_cam_x = max(CAMERA_BOUNDARY_MARGIN, initial_cam_x); // Left bound
	if (mapWidth > 0) {
		initial_cam_x = min(initial_cam_x, mapWidth - cam_width + CAMERA_BOUNDARY_MARGIN); // Right bound
	}
	initial_cam_y = max(0.0f, initial_cam_y); // Top bound
	if (mapHeight > 0) {
		initial_cam_y = min(initial_cam_y, mapHeight - cam_height + CAMERA_BOUNDARY_MARGIN); // Bottom bound
	}

	game->SetCamPos(initial_cam_x, initial_cam_y);
	DebugOut(L"[INFO] Initial camera set to (%f, %f).\n", initial_cam_x, initial_cam_y);

	// --- Load Initial Chunks ---
	LoadChunksInRange(initial_cam_x, cam_width);

	// Ensure player exists after loading initial chunks (should be in one of them)
	if (player == NULL) {
		DebugOut(L"[FATAL ERROR] Player object not found after loading initial chunks!\n");
		// Handle this error appropriately - maybe load ALL chunks or throw exception?
		// For now, just warn. Could indicate player defined outside initial view.
	}

	DebugOut(L"[INFO] Done loading scene %ls\n", sceneFilePath);
}

// --- Update Functions ---

void CPlayScene::UpdateChunks(float cam_x, float cam_width)
{
	// These seem okay, buffers can be tuned if needed
	LoadChunksInRange(cam_x, cam_width);
	UnloadChunksOutOfRange(cam_x, cam_width);
}

void CPlayScene::UpdateObjects(DWORD dt, CMario* mario, vector<LPGAMEOBJECT>& coObjects)
{
	// 1. Collect potential colliders (excluding player)
	coObjects.clear(); // Clear from previous frame
	for (LPCHUNK chunk : chunks) {
		if (!chunk->IsLoaded()) continue; // Optimization: Skip unloaded chunks
		vector<LPGAMEOBJECT>& chunkObjects = chunk->GetObjects();
		for (LPGAMEOBJECT obj : chunkObjects) {
			if (obj != mario) // Simple exclusion
				coObjects.push_back(obj);
		}
	}

	// 2. Update all loaded objects (including player within its chunk iteration)
	// Check for time freeze effect
	bool isChronoStopped = mario->GetIsPowerUp() || mario->GetIsTailUp() ||
		mario->GetIsPowerDown() || mario->GetIsTailDown();

	for (LPCHUNK chunk : chunks) {
		if (!chunk->IsLoaded()) continue; // Optimization: Skip unloaded chunks
		// Important: Iterate over a *copy* if obj->Update might modify the chunk's object list (e.g., spawning items)
		// However, since we purge deleted objects later, iterating the original might be okay if adds happen at end.
		// Safest is often a copy, but less performant. Sticking with original for now based on existing code.
		vector<LPGAMEOBJECT>& chunkObjects = chunk->GetObjects(); // Get reference
		for (size_t i = 0; i < chunkObjects.size(); ++i) { // Use index loop if Update might invalidate iterators
			LPGAMEOBJECT obj = chunkObjects[i];
			if (obj == nullptr) continue; // Skip if already marked for deletion? (Purge handles this later)

			// Skip updates if time is stopped (except for Mario himself)
			if (isChronoStopped && obj != mario) {
				continue;
			}

			obj->Update(dt, &coObjects); // Pass the collected colliders
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

	// Smooth Mario's Y
	static float smoothed_center_y = mario_center_y; // Initialize first time?
	// A better init might be in Load() or first Update() call, but static works.
	smoothed_center_y += (mario_center_y - smoothed_center_y) * 0.5f; // Use a smoothing factor constant?

	// --- Horizontal Following ---
	static bool moving_right_prev = mario_vx >= 0;
	static bool isHorizontallyPaused = false;
	static float playerXatTurn = 0.0f;
	static float targetXatTurn = 0.0f;
	// float dead_zone_threshold = HORIZONTAL_DEAD_ZONE_THRESHOLD; // Use constant
	float target_cam_x;
	bool current_moving_right = mario_vx >= 0;
	bool direction_changed = (mario_vx > 0.01f && !moving_right_prev) || (mario_vx < -0.01f && moving_right_prev);

	if (isHorizontallyPaused) {
		target_cam_x = targetXatTurn;
		if (abs(player_cx - playerXatTurn) >= HORIZONTAL_DEAD_ZONE_THRESHOLD) { // Use constant
			isHorizontallyPaused = false;
			target_cam_x = player_cx - cam_width / 2.0f; // Recalculate immediately
		}
	}
	else {
		target_cam_x = player_cx - cam_width / 2.0f;
		if (direction_changed) {
			isHorizontallyPaused = true;
			playerXatTurn = player_cx;
			targetXatTurn = cam_x; // Lock target to current camera pos
			target_cam_x = targetXatTurn;
		}
	}
	if (abs(mario_vx) > 0.01f) {
		moving_right_prev = current_moving_right;
	}

	// --- Vertical Camera Logic ---
	static bool bypassGroundLock = false;
	static bool was_on_platform_prev = mario->IsOnPlatform(); // Init in Load?

	float absolute_ground_cam_y = 0;
	if (mapHeight > 0) {
		absolute_ground_cam_y = mapHeight - cam_height + CAMERA_BOUNDARY_MARGIN; // Use constant
		absolute_ground_cam_y = max(0.0f, absolute_ground_cam_y); // Clamp >= 0
	}

	float upward_allowance_threshold_y = absolute_ground_cam_y - VERTICAL_GROUND_LOCK_BUFFER;
	float tracking_target_y = smoothed_center_y - cam_height / 2.0f; // Target to center Mario

	bool is_flying_state = (mario->GetLevel() == MARIO_LEVEL_TAIL && mario->GetIsFlying()); // Assume GetIsFlying exists
	bool is_in_water = false;
	bool is_on_vine = false;
	bool is_on_platform = mario->IsOnPlatform();

	// Calculate mario height status once
	bool isMarioHigh = (smoothed_center_y < upward_allowance_threshold_y);

	// Update Bypass Flag State
	if (is_on_platform) {
		bypassGroundLock = false;
	}
	else if (was_on_platform_prev && !is_on_platform) { // Just became airborne
		if (isMarioHigh) { // Was high when leaving platform?
			bypassGroundLock = true;
		}
	} // else bypass state persists while airborne

	// Determine Target Camera Y
	float target_cam_y;
	if (is_in_water || is_on_vine || is_flying_state) {
		target_cam_y = tracking_target_y;
	}
	else if (bypassGroundLock) {
		target_cam_y = tracking_target_y;
	}
	else { // Default: Not special, not bypassing
		if (isMarioHigh) { // High -> Track center (covers ON sky platform)
			target_cam_y = tracking_target_y;
		}
		else { // Low -> Lock ground (covers ON ground platform or airborne low)
			target_cam_y = absolute_ground_cam_y;
		}
	}

	// --- Clamping ---
	// Clamp Target Y
	target_cam_y = max(0.0f, target_cam_y); // Clamp above 0
	if (mapHeight > 0) {
		target_cam_y = min(target_cam_y, absolute_ground_cam_y); // Clamp below ground level
	}
	// Clamp Target X
	target_cam_x = max(CAMERA_BOUNDARY_MARGIN, target_cam_x);
	if (mapWidth > 0) {
		target_cam_x = min(target_cam_x, mapWidth - cam_width + CAMERA_BOUNDARY_MARGIN);
	}

	// --- Apply Smoothing ---
	cam_x += (target_cam_x - cam_x) * DEFAULT_HORIZONTAL_SMOOTH; // Use constants
	cam_y += (target_cam_y - cam_y) * DEFAULT_VERTICAL_SMOOTH;

	// --- Final Clamping on Actual Position ---
	cam_x = max(CAMERA_BOUNDARY_MARGIN, cam_x);
	if (mapWidth > 0) {
		cam_x = min(cam_x, mapWidth - cam_width + CAMERA_BOUNDARY_MARGIN);
	}
	cam_y = max(0.0f, cam_y);
	if (mapHeight > 0) {
		cam_y = min(cam_y, absolute_ground_cam_y);
	}

	// Set Camera Position
	game->SetCamPos(cam_x, cam_y);

	// Update previous state
	was_on_platform_prev = is_on_platform;

	// --- Final Debug Output --- (Simplified)
	//DebugOut(L"Cam:(%.1f, %.1f) Tgt:(%.1f, %.1f) M:(%.1f, %.1f) V:(%.2f, %.2f) OnP:%d Byp:%d HPa:%d\n",
	//	cam_x, cam_y, target_cam_x, target_cam_y,
	//	player_cx, smoothed_center_y, // Using smoothed Y for consistency with logic
	//	mario_vx, mario_vy,
	//	is_on_platform, bypassGroundLock, isHorizontallyPaused);
}


// Inside CPlayScene::Update(DWORD dt)

void CPlayScene::Update(DWORD dt)
{
	if (player == NULL) {
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
	bool isChronoStopped = mario->GetIsPowerUp() || mario->GetIsTailUp() ||
		mario->GetIsPowerDown() || mario->GetIsTailDown();

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

	std::stable_sort(renderList.begin(), renderList.end(),
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
	player = NULL;

	DebugOut(L"[INFO] Scene %d unloaded.\n", id);
}

// Keep IsGameObjectDeleted as is, used by PurgeDeletedObjects
bool CPlayScene::IsGameObjectDeleted(const LPGAMEOBJECT& o) { return o == NULL || o->IsDeleted(); } // Add IsDeleted check

void CPlayScene::PurgeDeletedObjects()
{
	for (LPCHUNK chunk : chunks) {
		if (chunk && chunk->IsLoaded()) { // Check if chunk is valid and loaded
			chunk->PurgeDeletedObjects(); // Delegate purging to the chunk
		}
	}
}