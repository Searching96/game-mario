#include "Animation.h"
#include "Game.h"
#include "debug.h"

void CAnimation::Add(int spriteId, DWORD time)
{
	int t = time;
	if (time == 0) t = this->defaultTime;

	LPSPRITE sprite = CSprites::GetInstance()->Get(spriteId);
	if (sprite == nullptr)
		DebugOut(L"[ERROR] Sprite ID %d not found!\n", spriteId);

	LPANIMATION_FRAME frame = new CAnimationFrame(sprite, t);
	frames.push_back(frame);
}

void CAnimation::Render(float x, float y)
{
    // Use high-precision timer
    LARGE_INTEGER currentTime;
    QueryPerformanceCounter(&currentTime);

    // Initialize on first render
    if (currentFrame == -1)
    {
        currentFrame = 0;
        lastFrameTime = currentTime;
    }
    else
    {
        // Get game state
        bool isPaused = CGame::GetInstance()->IsPaused();
        float gameSpeed = CGame::GetInstance()->GetGameSpeed();
		bool isStoppingAnimation = CGame::GetInstance()->IsStoppingAnimation();

        if (!isPaused && gameSpeed > 0.0f && !isStoppingAnimation)
        {
            // Calculate elapsed time
            static LARGE_INTEGER frequency;
            static bool frequencyInitialized = false;
            if (!frequencyInitialized)
            {
                QueryPerformanceFrequency(&frequency);
                frequencyInitialized = true;
            }

            double elapsedSeconds = (currentTime.QuadPart - lastFrameTime.QuadPart) / (double)frequency.QuadPart;
            double frameDurationSeconds = frames[currentFrame]->GetTime() / 1000.0 / gameSpeed; // Convert ms to seconds

            while (elapsedSeconds >= frameDurationSeconds && gameSpeed > 0.0f)
            {
                currentFrame++;
                elapsedSeconds -= frameDurationSeconds;
                lastFrameTime.QuadPart += (LONGLONG)(frameDurationSeconds * frequency.QuadPart);

                if (currentFrame >= frames.size())
                    currentFrame = 0;

                // Update frame duration for next frame
                frameDurationSeconds = frames[currentFrame]->GetTime() / 1000.0 / gameSpeed;
            }
        }
    }

    // Safety check for valid frame
    if (!frames.empty() && currentFrame >= 0 && currentFrame < frames.size())
    {
        frames[currentFrame]->GetSprite()->Draw(x, y);
    }
}

