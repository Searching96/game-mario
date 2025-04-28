#pragma once
#include "Game.h"
#include "Textures.h"
#include "Scene.h"
#include "GameObject.h"
#include "Brick.h"
#include "Mario.h"
#include "Goomba.h"
#include "Chunk.h"
#include <set>
//#include "Koopas.h"


class CPlayScene : public CScene
{
protected:
    LPGAMEOBJECT player;
    vector<LPCHUNK> chunks;
    LPCHUNK currentParsingChunk = nullptr;
    wstring scene_file_path;
    bool is_camera_vertically_locked = false;

    void _ParseSection_SPRITES(string line);
    void _ParseSection_ANIMATIONS(string line);
    void _ParseSection_ASSETS(string line);
    void _ParseSection_CHUNK_OBJECTS(string line, LPCHUNK targetChunk);

    void LoadAssets(LPCWSTR assetFile);
    void LoadChunkObjects(int chunk_id, LPCHUNK targetChunk); // New
    void LoadChunksInRange(float cam_x, float cam_width); // New
    void UnloadChunksOutOfRange(float cam_x, float cam_width); // New

    void UpdateChunks(float cam_x, float cam_width);
    void UpdateObjects(DWORD dt, CMario* mario, vector<LPGAMEOBJECT>& coObjects);
    void UpdateCamera(CMario* mario, float cx, float cy, float cam_width, float cam_height, float mapWidth, float mapHeight, float marginX, float marginY);

public:
    CPlayScene(int id, LPCWSTR filePath);

    virtual void Load();
    virtual void Update(DWORD dt);
    virtual void Render();
    virtual void Unload();

    LPGAMEOBJECT GetPlayer() { return player; }
    LPCHUNK GetChunk(int id) {
        for (LPCHUNK chunk : chunks) {
            if (chunk->GetID() == id) return chunk;
        }
        return nullptr;
    }

    void Clear();
    void PurgeDeletedObjects();

    static bool IsGameObjectDeleted(const LPGAMEOBJECT& o);
};

typedef CPlayScene* LPPLAYSCENE;