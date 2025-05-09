#include "Chunk.h"
#include "Mario.h"
#include "PlayScene.h"

void CChunk::Update(DWORD dt, std::vector<LPGAMEOBJECT>* coObjects)
{
	for (LPGAMEOBJECT obj : objects)
	{
		if (obj != nullptr)
			obj->Update(dt, coObjects);
	}
}

// Clear all objects in the chunk
void CChunk::Clear()
{
	for (LPGAMEOBJECT obj : objects)
	{
		if (obj != nullptr)
			delete obj;
	}
	objects.clear();
	enemies.clear();
	//isDeleted.clear();
	isLoaded = false;
}

void CChunk::Render()
{
	for (LPGAMEOBJECT obj : objects)
	{
		if (obj != nullptr)// && !isDeleted[obj]) // Skip defeated objects
			obj->Render();
	}
}

void CChunk::PurgeDeletedObjects()
{
	for (auto it = objects.begin(); it != objects.end(); )
	{
		LPGAMEOBJECT obj = *it;
		//if (obj != nullptr && (isDeleted[obj] || obj->IsDeleted()))
		if (obj != nullptr && obj->IsDeleted())
		{
			delete obj; // Free memory
			it = objects.erase(it); // Erase and advance iterator
		}
		else
		{
			++it;
		}
	}
	// repeat deletion for enemies
	for (auto it = enemies.begin(); it != enemies.end(); )
	{
		LPGAMEOBJECT obj = *it;
		if (obj != nullptr && obj->IsDeleted())
		{
			//delete obj; // Free memory
			it = enemies.erase(it); // Erase and advance iterator
		}
		else
		{
			++it;
		}
	}
}