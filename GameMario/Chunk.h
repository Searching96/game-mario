#pragma once
#include "GameObject.h"
#include <map>

class CChunk
{
protected:
	int id;
	float startX, endX;
	bool isLoaded = false;

	vector<LPGAMEOBJECT> objects;
	vector<LPGAMEOBJECT> enemies;
	map<int, bool> isDeleted;
	map<int, bool> isConsumed;
public:
	CChunk(int id, float startX, float endX) : id(id), startX(startX), endX(endX) {};
	~CChunk() {};
	void AddObject(LPGAMEOBJECT obj);
	void AddEnemy(LPGAMEOBJECT obj);
	void RemoveObject(LPGAMEOBJECT obj);
	vector<LPGAMEOBJECT>& GetObjects() { return objects; }

	void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	void Clear();
	void Render();
	void PurgeDeletedObjects();

	int GetID() const { return id; }
	float GetStartX() const { return startX; }
	float GetEndX() const { return endX; }
	bool IsLoaded() const { return isLoaded; }
	void SetLoaded(bool loaded) { isLoaded = loaded; }
	void ResetChunkState()
	{
		ResetChunkConsumableState();
		ResetChunkDeletedState();
	}
	void ResetChunkConsumableState();
	void ResetChunkDeletedState();

	void SetIsObjectDeleted(int id, int isDel);
	int GetIsObjectDeleted(int id) { return isDeleted[id]; }
	void SetIsObjectConsumed(int id, int isCon);
	int GetIsObjectConsumed(int id) { return isConsumed[id]; }

	vector<LPGAMEOBJECT> GetEnemies() { return enemies; }
	bool IsObjectInChunk(LPGAMEOBJECT obj);

	void RevealAllCoinBrick();
};

typedef CChunk* LPCHUNK;

