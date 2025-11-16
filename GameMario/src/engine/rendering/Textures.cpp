#include <Windows.h>

#include "debug/debug.h"
#include "core/Game.h"
#include "engine/rendering/Textures.h"

CTextures* CTextures::__instance = nullptr;

CTextures::CTextures()
{

}

CTextures* CTextures::GetInstance()
{
	if (__instance == nullptr) __instance = new CTextures();
	return __instance;
}

void CTextures::Add(int id, LPCWSTR filePath)
{
	textures[id] = CGame::GetInstance()->LoadTexture(filePath);
}

LPTEXTURE CTextures::Get(unsigned int i)
{
	LPTEXTURE t = textures[i];
	if (t == nullptr)
		DebugOut(L"[ERROR] Texture Id %d not found !\n", i);
	
	return t;
}

/*
	Clear all loaded textures
*/
void CTextures::Clear()
{
	for (auto x : textures)
	{
		LPTEXTURE tex = x.second;
		if (tex != nullptr) delete tex;
	}

	textures.clear();
}



