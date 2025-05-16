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
	void AddObject(LPGAMEOBJECT obj)
	{
		if (obj != nullptr)
			objects.push_back(obj);
	}

	void AddEnemy(LPGAMEOBJECT obj)
	{
		if (obj != nullptr)
			enemies.push_back(obj);
	}

	void RemoveObject(LPGAMEOBJECT obj)
	{
		objects.erase(std::remove(objects.begin(), objects.end(), obj), objects.end());
		// RED ALERT: do i need to erase koopa from enemies too?
		enemies.erase(std::remove(enemies.begin(), enemies.end(), obj), enemies.end());
	}

	vector<LPGAMEOBJECT>& GetObjects()
	{
		return objects; // Return reference to objects without modification
	}

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

	void SetIsObjectDeleted(int id, int isDel)
	{
		if (isDel) {
			isDeleted[id] = true;
		}
		else {
			// Only insert false if id is not already present
			if (isDeleted.find(id) == isDeleted.end())
				isDeleted[id] = false;
		}
	}

	int GetIsObjectDeleted(int id) { return isDeleted[id]; }

	int SetIsObjectConsumed(int id, int isCon)
	{
		if (isCon) {
			isConsumed[id] = true;
		}
		else {
			// Only insert false if id is not already present
			if (isConsumed.find(id) == isConsumed.end())
				isConsumed[id] = false;
		}
		return isConsumed[id];
	}

	int GetIsObjectConsumed(int id) { return isConsumed[id]; }

	vector<LPGAMEOBJECT> GetEnemies() { return enemies; }

	// function to check if a lpgameobject is in this chunk
	bool IsObjectInChunk(LPGAMEOBJECT obj)
	{
		if (obj == nullptr) return false;
		float obj_x, obj_y;
		obj->GetPosition(obj_x, obj_y);
		return (obj_x >= startX && obj_x <= endX);
	}
};

typedef CChunk* LPCHUNK;

