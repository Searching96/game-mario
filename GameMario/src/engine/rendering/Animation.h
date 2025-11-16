#pragma once

#include <Windows.h>
#include <unordered_map>

#include "engine/rendering/Sprites.h"
#include "engine/rendering/AnimationFrame.h"

using namespace std;

class CAnimation
{
	LARGE_INTEGER lastFrameTime;
	int defaultTime;
	int currentFrame;
	vector<LPANIMATION_FRAME> frames;
public:
	CAnimation(int defaultTime = 100) { this->defaultTime = defaultTime; lastFrameTime.QuadPart = -1; currentFrame = -1; }
	void Add(int spriteId, DWORD time = 0);
	void Render(float x, float y);
};

typedef CAnimation* LPANIMATION;