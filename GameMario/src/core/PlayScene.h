#pragma once
#include "core/Game.h"
#include "engine/rendering/Textures.h"
#include "core/Scene.h"
#include "core/GameObject.h"

#include "entities/blocks/Brick.h"
#include "entities/player/Mario.h"
#include "entities/enemies/Goomba.h"
#include "world/Border.h"

#include "world/Chunk.h"
#include "engine/input/SampleKeyEventHandler.h"

class CPlayScene : public CScene
{
protected:
	LPGAMEOBJECT player;
	vector<LPGAMEOBJECT> borders;
	vector<LPCHUNK> chunks;
	LPCHUNK currentParsingChunk = nullptr;
	wstring scene_file_path;

	float startCamX = 0.0f;
	float startCamY = 0.0f;
	float mapWidth = 0.0f;
	float mapHeight = 0.0f;
	int cameraMode = 0; // 0: locked to ground, 1: steady scrolling
	float scrollCamXStart = 0.0f;
	float scrollCamXEnd = 0.0f;

	bool switchingScene = false;

	void _ParseSection_SPRITES(string line);
	void _ParseSection_ANIMATIONS(string line);
	void _ParseSection_ASSETS(string line);
	void _ParseSection_SETTINGS(string line);
	void _ParseSection_MARIO(string line);
	void _ParseSection_BORDER(string line);
	void _ParseSection_CHUNK_OBJECTS(string line, LPCHUNK targetChunk);

	void LoadAssets(LPCWSTR assetFile);
	void LoadChunkObjects(int chunk_id, LPCHUNK targetChunk);
	void LoadChunksInRange(float cam_x, float cam_width);
	void UnloadChunksOutOfRange(float cam_x, float cam_width);

	void UpdateChunks(float cam_x, float cam_width);
	void UpdateCamera(CMario* mario, float cam_width, float cam_height);
	//void UpdateObjects(DWORD dt, CMario* mario, vector<LPGAMEOBJECT>& coObjects);

public:
	CPlayScene(int id, LPCWSTR filePath);

	virtual void Load();
	virtual void Update(DWORD dt);
	virtual void Render();
	virtual void Unload();
	void ReloadAssets();
	void DefeatEnemiesOutOfRange();
	void RespawnEnemiesInRange();
	void ResetAllChunkState();
	void ResetAllChunkConsumables();
	void ResetAllChunkDeleted();

	void LoadChunkWithX(float x)
	{
		for (LPCHUNK chunk : chunks) {
			if (chunk->GetStartX() <= x && chunk->GetEndX() >= x) {
				LoadChunkObjects(chunk->GetID(), chunk);
			}
		}
	}

	int GetId() const { return id; }

	bool GetIsSwitchingScene() const { return switchingScene; }

	LPGAMEOBJECT GetPlayer() { return player; }

	void InitializeSwitchSceneSequence()
	{
		switchingScene = true;
		player->SetState(MARIO_STATE_SWITCH_SCENE);
	}

	void SetPlayer(LPGAMEOBJECT player) {
		float x, y;
		this->player->GetPosition(x, y);
		player->SetPosition(x, y);
		player->SetSpeed(0, 0);
		this->player = player;
	}

	LPCHUNK GetChunk(int id) {
		for (LPCHUNK chunk : chunks) {
			if (chunk->GetID() == id) return chunk;
		}
		return nullptr;
	}
	vector<LPCHUNK> GetChunks()
	{
		return chunks;
	}

	vector<LPCHUNK> GetLoadedChunks()
	{
		vector<LPCHUNK> loadedChunks;
		for (LPCHUNK chunk : chunks) {
			if (chunk->IsLoaded()) {
				loadedChunks.push_back(chunk);
			}
		}
		return loadedChunks;
	}

	void TeleportToMapEnd();

	void PurgeDeletedObjects();

	static bool IsGameObjectDeleted(const LPGAMEOBJECT& o);
};

typedef CPlayScene* LPPLAYSCENE;