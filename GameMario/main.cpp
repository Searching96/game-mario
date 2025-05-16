/* =============================================================
	INTRODUCTION TO GAME PROGRAMMING SE102

	SAMPLE 05 - SCENE MANAGER

	This sample illustrates how to:

		1/ Read scene (textures, sprites, animations and objects) from files
		2/ Handle multiple scenes in game

	Key classes/functions:
		CScene
		CPlayScene


HOW TO INSTALL Microsoft.DXSDK.D3DX
===================================
1) Tools > NuGet package manager > Package Manager Console
2) execute command :  Install-Package Microsoft.DXSDK.D3DX


================================================================ */

#include <windows.h>
#include <d3d10.h>
#include <d3dx10.h>
#include <list>

#include "debug.h"
#include "Game.h"
#include "GameObject.h"
#include "Textures.h"
#include "Animation.h"
#include "Animations.h"

#include "Mario.h"
#include "Brick.h"
#include "Goomba.h"
#include "Coin.h"
#include "Platform.h"

#include "SampleKeyEventHandler.h"

#include "AssetIDs.h"

#define WINDOW_CLASS_NAME L"SampleWindow"
#define MAIN_WINDOW_TITLE L"Super Mario Bros 3"
#define WINDOW_ICON_PATH L"mario.ico"

#define BACKGROUND_COLOR D3DXCOLOR(181.0f/255, 235.0f/255, 242.0f/255, 0.0f)

#define SCREEN_WIDTH 256
#define SCREEN_HEIGHT 240

LRESULT CALLBACK WinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

/*
	Update world status for this frame
	dt: time period between beginning of last frame and beginning of this frame
*/
void Update(DWORD dt)
{
	if (dt == 0) return;

	CGame::GetInstance()->GetCurrentScene()->Update(dt);
	CGame::GetInstance()->GetGameState()->Update(dt);
}

/*
	Render a frame
*/
void Render()
{
	CGame* g = CGame::GetInstance();

	ID3D10Device* pD3DDevice = g->GetDirect3DDevice();
	IDXGISwapChain* pSwapChain = g->GetSwapChain();
	ID3D10RenderTargetView* pRenderTargetView = g->GetRenderTargetView();
	ID3DX10Sprite* spriteHandler = g->GetSpriteHandler();

	pD3DDevice->ClearRenderTargetView(pRenderTargetView, BACKGROUND_COLOR);

	spriteHandler->Begin(D3DX10_SPRITE_SORT_TEXTURE);

	FLOAT NewBlendFactor[4] = { 0,0,0,0 };
	pD3DDevice->OMSetBlendState(g->GetAlphaBlending(), NewBlendFactor, 0xffffffff);

	CGame::GetInstance()->GetCurrentScene()->Render();
	CGame::GetInstance()->GetGameState()->RenderHUD();

	spriteHandler->End();
	pSwapChain->Present(0, 0);
}

HWND CreateGameWindow(HINSTANCE hInstance, int nCmdShow, int ScreenWidth, int ScreenHeight)
{
	WNDCLASSEX wc;
	wc.cbSize = sizeof(WNDCLASSEX);

	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.hInstance = hInstance;

	RECT rect = { 0, 0, ScreenWidth, ScreenHeight };

	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

	// Calculate the total window width and height
	int totalWindowWidth = rect.right - rect.left - 1;
	int totalWindowHeight = rect.bottom - rect.top - 1;


	wc.lpfnWndProc = (WNDPROC)WinProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hIcon = (HICON)LoadImage(hInstance, WINDOW_ICON_PATH, IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = WINDOW_CLASS_NAME;
	wc.hIconSm = nullptr;

	RegisterClassEx(&wc);

	HWND hWnd =
		CreateWindow(
			WINDOW_CLASS_NAME,
			MAIN_WINDOW_TITLE,
			WS_OVERLAPPEDWINDOW, // WS_EX_TOPMOST | WS_VISIBLE | WS_POPUP,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			totalWindowWidth,
			totalWindowHeight,
			nullptr,
			nullptr,
			hInstance,
			nullptr);

	if (!hWnd)
	{
		OutputDebugString(L"[ERROR] CreateWindow failed");
		DWORD ErrCode = GetLastError();
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return hWnd;
}

int Run()
{
	MSG msg;
	int done = 0;

	// Initialize high-precision timer
	LARGE_INTEGER frequency, lastTime, currentTime;
	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&lastTime);

	// Game loop constants
	const double targetSecondsPerFrame = 1.0 / MAX_FRAME_RATE; // e.g., 1/60 for 60 FPS
	CGame* game = CGame::GetInstance();

	while (!done)
	{
		// Process messages
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				done = 1;
				break;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// Get elapsed time
		QueryPerformanceCounter(&currentTime);
		double elapsedSeconds = (currentTime.QuadPart - lastTime.QuadPart) / (double)frequency.QuadPart;

		// Always process keyboard and render, even when paused
		game->ProcessKeyboard();
		Render();

		float gameSpeed = game->GetGameSpeed();
		bool isPaused = game->IsPaused();

		if (isPaused || gameSpeed == 0.0f)
		{
			// When paused, set dt = 0 and update lastTime to avoid time accumulation
			lastTime = currentTime;
			DWORD dt = 0; // No time passes while paused

			// Update with dt = 0 (no movement, animations, etc.)
			Update(dt);
			game->SwitchScene();

			// Yield to avoid CPU hogging
			Sleep(1); // or SwitchToThread()
			continue;
		}

		// Target frame time adjusted by game speed
		double adjustedFrameTime = targetSecondsPerFrame / gameSpeed;

		if (elapsedSeconds >= adjustedFrameTime)
		{
			lastTime = currentTime;

			// Cap elapsed time to prevent large steps, then scale by gameSpeed
			double cappedSeconds = min(elapsedSeconds, 0.25 / gameSpeed);
			DWORD dt = (DWORD)(cappedSeconds * 1000.0); // dt in milliseconds

			Update(dt);
			game->SwitchScene();
		}
		else
		{
			// Calculate time to wait until next frame
			double timeToWait = adjustedFrameTime - elapsedSeconds;

			if (timeToWait > 0.002) // Sleep only for significant durations
			{
				Sleep((DWORD)(timeToWait * 1000.0 * 0.95)); // 95% to avoid oversleep
			}
			else
			{
				SwitchToThread(); // or Sleep(0)
			}
		}
	}

	return 1;
}

int WINAPI WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine,
	_In_ int nCmdShow
) {
	HWND hWnd = CreateGameWindow(hInstance, nCmdShow, SCREEN_WIDTH, SCREEN_HEIGHT);

	SetDebugWindow(hWnd);

	LPGAME game = CGame::GetInstance();
	game->Init(hWnd, hInstance);
	game->InitKeyboard();


	//IMPORTANT: this is the only place where a hardcoded file name is allowed ! 
	game->Load(L"mario-sample.txt");

	SetWindowPos(hWnd, 0, 0, 0, SCREEN_WIDTH * 3, SCREEN_HEIGHT * 3, SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER);

	Run();

	return 0;
}