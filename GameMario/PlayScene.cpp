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
	string target_section = "[CHUNK " + to_string(chunk_id) + " ";

	while (getline(f, line))
	{
		if (line.empty() || line[0] == '#') continue;
		if (line[0] == '[')
		{
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

void CPlayScene::Load()
{
	DebugOut(L"[INFO] Start loading scene from : %s \n", sceneFilePath);
	ifstream f;
	f.open(sceneFilePath);
	int section = SCENE_SECTION_UNKNOWN;
	char str[MAX_SCENE_LINE];

	while (f.getline(str, MAX_SCENE_LINE))
	{
		string line(str);
		if (line.empty() || line[0] == '#') continue;

		if (line.substr(0, 7) == "[CHUNK ")
		{
			vector<string> tokens = split(line.substr(7, line.length() - 8), " ");
			if (tokens.size() >= 3)
			{
				int chunkId = atoi(tokens[0].c_str());
				float chunk_Start_X = atof(tokens[1].c_str());
				float chunk_End_X = atof(tokens[2].c_str());

				// Create empty chunk
				LPCHUNK chunk = new CChunk(chunkId, chunk_Start_X, chunk_End_X);
				chunks.push_back(chunk);
				DebugOut(L"[INFO] Registered chunk %d (%f, %f)\n", chunkId, chunk_Start_X, chunk_End_X);
			}
			else
			{
				DebugOut(L"[ERROR] Malformed chunk header: %hs\n", line.c_str());
			}
			section = SCENE_SECTION_CHUNK_OBJECT;
			continue;
		}

		if (line == "[ASSETS]") { section = SCENE_SECTION_ASSETS; continue; }
		if (line[0] == '[') { section = SCENE_SECTION_UNKNOWN; continue; }

		switch (section)
		{
		case SCENE_SECTION_ASSETS:
			_ParseSection_ASSETS(line);
			break;
		case SCENE_SECTION_CHUNK_OBJECT:
			// Skip object lines during initial load
			break;
		}
	}

	f.close();
	scene_file_path = sceneFilePath; // Store for later parsing

	// Load initial chunks based on camera
	CGame* game = CGame::GetInstance();
	float cam_x, cam_y;
	game->GetCamPos(cam_x, cam_y);
	float cam_width = (float)game->GetBackBufferWidth();
	LoadChunksInRange(cam_x, cam_width);

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

void CPlayScene::UpdateCamera(CMario* mario, float cx, float cy, float cam_width, float cam_height, float mapWidth, float mapHeight, float marginX, float marginY)
{
	CGame* game = CGame::GetInstance();
	float cam_x, cam_y;
	game->GetCamPos(cam_x, cam_y);
	float mario_X, mario_Y;
	mario->GetPosition(mario_X, mario_Y);

	cam_x = cx - cam_width / 2;
	if (cx > cam_x + cam_width - marginX)
		cam_x = cx - (cam_width - marginX);
	else if (cx < cam_x + marginX)
		cam_x = cx - marginX;

	cam_y = mapHeight - cam_height;
	if (mario->IsOnPlatform() && mario_Y > mapHeight - game->GetBackBufferHeight())
	{
		is_camera_vertically_locked = false;
	}
	else if (mario->GetLevel() == MARIO_LEVEL_TAIL && mario->GetJumpCount() > 1 && cy < cam_y)
	{
		is_camera_vertically_locked = true;
	}
	if (is_camera_vertically_locked)
	{
		cam_y = cy - cam_height / 2;
	}

	if (cam_x < -8) cam_x = -8;
	if (cam_x > mapWidth - cam_width - 8) cam_x = mapWidth - cam_width - 8;
	if (cam_y < 0) cam_y = 0;
	if (cam_y > mapHeight - cam_height - 8) cam_y = mapHeight - cam_height - 8;

	game->SetCamPos(cam_x, cam_y);
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
	float mapWidth = 2815.0f;
	float mapHeight = 432.0f;
	float marginX = 136.0f;
	float marginY = 40.0f;

	vector<LPGAMEOBJECT> coObjects;
	UpdateChunks(cam_x, cam_width);
	UpdateObjects(dt, mario, coObjects);
	UpdateCamera(mario, cx, cy, cam_width, cam_height, mapWidth, mapHeight, marginX, marginY);
	PurgeDeletedObjects();
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