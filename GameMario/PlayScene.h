#pragma once
#include "Game.h"
#include "Textures.h"
#include "Scene.h"
#include "GameObject.h"

#include "Brick.h"
#include "Mario.h"
#include "Goomba.h"

#include "Chunk.h"
#include "SampleKeyEventHandler.h"

class CPlayScene : public CScene
{
protected:
	LPGAMEOBJECT player;
	vector<LPCHUNK> chunks;
	LPCHUNK currentParsingChunk = nullptr;
	wstring scene_file_path;

	float startCamX = 0.0f;
	float startCamY = 0.0f;
	float mapWidth = 0.0f;
	float mapHeight = 0.0f;
	float marginX = 0.0f;
	float marginY = 0.0f;

	void _ParseSection_SPRITES(string line);
	void _ParseSection_ANIMATIONS(string line);
	void _ParseSection_ASSETS(string line);
	void _ParseSection_SETTINGS(string line);
	void _ParseSection_MARIO(string line);
	void _ParseSection_CHUNK_OBJECTS(string line, LPCHUNK targetChunk);

	void LoadAssets(LPCWSTR assetFile);
	void LoadChunkObjects(int chunk_id, LPCHUNK targetChunk);
	void LoadChunksInRange(float cam_x, float cam_width);
	void UnloadChunksOutOfRange(float cam_x, float cam_width);

	void UpdateChunks(float cam_x, float cam_width);
	void UpdateObjects(DWORD dt, CMario* mario, vector<LPGAMEOBJECT>& coObjects);
	void UpdateCamera(CMario* mario, float player_cx, float player_cy, float cam_width, float cam_height);

public:
	CPlayScene(int id, LPCWSTR filePath);

	virtual void Load();
	virtual void Update(DWORD dt);
	virtual void Render();
	virtual void Unload();

	int GetId() const { return id; }
	LPGAMEOBJECT GetPlayer() { return player; }
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

	void PurgeDeletedObjects();

	static bool IsGameObjectDeleted(const LPGAMEOBJECT& o);
};

typedef CPlayScene* LPPLAYSCENE;