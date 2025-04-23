#pragma once
#include "GameObject.h"
#include <map>
class CChunk
{
protected:
	int id;
	float start_x, end_x;
	vector<LPGAMEOBJECT> objects;
	map<LPGAMEOBJECT, bool> isDefeated;
public:
	CChunk(int id, float start_x, float end_x) : id(id), start_x(start_x), end_x(end_x) {};
	~CChunk() {};
	void AddObject(LPGAMEOBJECT obj)
	{
		if (obj != nullptr)
		{
			objects.push_back(obj);
			isDefeated[obj] = false; // Initialize as not defeated
		}
	}

	void RemoveObject(LPGAMEOBJECT obj)
	{
		objects.erase(std::remove(objects.begin(), objects.end(), obj), objects.end());
		isDefeated.erase(obj); // Remove from defeated map
	}

	std::vector<LPGAMEOBJECT>& GetObjects()
	{
		return objects; // Return reference to objects without modification
	}

	void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	void Clear();
	void Render();
	void PurgeDeletedObjects();

	int GetID() { return id; }
};

typedef CChunk* LPCHUNK;

