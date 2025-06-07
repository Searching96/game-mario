#include "Chunk.h"
#include "Mario.h"
#include "PlayScene.h"
#include "CoinBrick.h"
#include "BuffRoulette.h"

void CChunk::Update(DWORD dt, std::vector<LPGAMEOBJECT>* coObjects)
{
	for (LPGAMEOBJECT obj : objects)
	{
		if (obj != nullptr)
			obj->Update(dt, coObjects);
	}
}

void CChunk::AddObject(LPGAMEOBJECT obj)
{
	if (obj != nullptr)
	{
		if (dynamic_cast<CBuffRoulette*>(obj))
			for (LPGAMEOBJECT it : objects)
			{
				if (dynamic_cast<CBuffRoulette*>(it))
					it->Delete();
			}
		objects.push_back(obj);
	}
}

void CChunk::AddEnemy(LPGAMEOBJECT obj)
{
	if (obj != nullptr)
		enemies.push_back(obj);
}

void CChunk::RemoveObject(LPGAMEOBJECT obj)
{
	objects.erase(std::remove(objects.begin(), objects.end(), obj), objects.end());
	// RED ALERT: do i need to erase koopa from enemies too?
	enemies.erase(std::remove(enemies.begin(), enemies.end(), obj), enemies.end());
}

void CChunk::SetIsObjectDeleted(int id, int isDel)
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

void CChunk::SetIsObjectConsumed(int id, int isCon)
{
	if (isCon) {
		isConsumed[id] = true;
	}
	else {
		// Only insert false if id is not already present
		if (isConsumed.find(id) == isConsumed.end())
			isConsumed[id] = false;
	}
}

bool CChunk::IsObjectInChunk(LPGAMEOBJECT obj)
{
	if (obj == nullptr) return false;
	float objX, objY;
	obj->GetPosition(objX, objY);
	return (objX >= startX && objX <= endX);
}

void CChunk::RevealAllCoinBrick()
{
	for (LPGAMEOBJECT obj : objects)
	{
		if (obj != nullptr)
		{
			CCoinBrick* coinBrick = dynamic_cast<CCoinBrick*>(obj);
			if (coinBrick != nullptr && !coinBrick->IsRevealed())
			{
				coinBrick->SetState(COIN_BRICK_STATE_REVEALED);
			}
		}
	}
}

void CChunk::ResetChunkConsumableState()
{
	for (auto& it : isConsumed)
	{
		it.second = false;
	}
}

void CChunk::ResetChunkDeletedState()
{
	for (auto& it : isDeleted)
	{
		it.second = false;
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