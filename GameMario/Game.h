#pragma once
#include <Windows.h>
#include <d3d10.h>
#include <d3dx10.h>
#include <unordered_map>
#include <string>

using namespace std;

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#include "Texture.h"
#include "KeyEventHandler.h"
#include "Scene.h"
#include "GameState.h"

#define MAX_FRAME_RATE 120
#define KEYBOARD_BUFFER_SIZE 1024
#define KEYBOARD_STATE_SIZE 256

/*
	Our simple game framework
*/
class CGame
{
	static CGame* __instance;
	HWND hWnd;									// Window handle

	int backBufferWidth = 0;					// Backbuffer width & height, will be set during Direct3D initialization
	int backBufferHeight = 0;
	int windowWidth = 0;
	int windowHeight = 0;

	bool isPaused = 0;

	ID3D10Device* pD3DDevice = nullptr;
	IDXGISwapChain* pSwapChain = nullptr;
	ID3D10RenderTargetView* pRenderTargetView = nullptr;
	ID3D10BlendState* pBlendStateAlpha = nullptr;			// To store alpha blending state

	LPD3DX10SPRITE spriteObject;						// Sprite handling object, BIG MYSTERY: it has to be in this place OR will lead to access violation in D3D11.dll ????

	LPDIRECTINPUT8       di;		// The DirectInput object         
	LPDIRECTINPUTDEVICE8 didv;		// The keyboard device 

	BYTE  keyStates[KEYBOARD_STATE_SIZE];			// DirectInput keyboard state buffer 
	DIDEVICEOBJECTDATA keyEvents[KEYBOARD_BUFFER_SIZE];		// Buffered keyboard data

	LPKEYEVENTHANDLER keyHandler;

	float cam_x = 0.0f;
	float cam_y = 0.0f;

	HINSTANCE hInstance;

	ID3D10SamplerState* pPointSamplerState;

	LPCWSTR gameFilePath = nullptr; // Path to the game file

	unordered_map<int, LPSCENE> scenes;
	int current_scene = -1;
	int next_scene = -1;
	float gameSpeed = 1.0f;
	float prevGameSpeed = 1.0f;
	bool isStopppingAnimation = false;
	LPGAMESTATE gameState = nullptr;

	void _ParseSection_SETTINGS(string line);
	void _ParseSection_SCENES(string line);

public:
	// Init DirectX, Sprite Handler
	void Init(HWND hWnd, HINSTANCE hInstance);

	//
	// Draw a portion or ALL the texture at position (x,y) on the screen. (x,y) is at the CENTER of the image
	// rect : if nullptr, the whole texture will be drawn
	//        if NOT nullptr, only draw that portion of the texture 
	void Draw(float x, float y, LPTEXTURE tex, RECT* rect = nullptr, float alpha = 1.0f, int sprite_width = 0, int sprite_height = 0);

	void Draw(float x, float y, LPTEXTURE tex, int l, int t, int r, int b, float alpha = 1.0f, int sprite_width = 0, int sprite_height = 0)
	{
		RECT rect;
		rect.left = l;
		rect.top = t;
		rect.right = r;
		rect.bottom = b;
		this->Draw(x, y, tex, &rect, alpha, sprite_width, sprite_height);
	}

	LPTEXTURE LoadTexture(LPCWSTR texturePath);

	// Keyboard related functions 
	void InitKeyboard();
	int IsKeyDown(int KeyCode);
	void ProcessKeyboard();
	void SetKeyHandler(LPKEYEVENTHANDLER handler) { keyHandler = handler; }
	void ResizeWindow();


	ID3D10Device* GetDirect3DDevice() { return this->pD3DDevice; }
	IDXGISwapChain* GetSwapChain() { return this->pSwapChain; }
	ID3D10RenderTargetView* GetRenderTargetView() { return this->pRenderTargetView; }

	ID3DX10Sprite* GetSpriteHandler() { return this->spriteObject; }

	ID3D10BlendState* GetAlphaBlending() { return pBlendStateAlpha; };

	int GetBackBufferWidth() { return backBufferWidth; }
	int GetBackBufferHeight() { return backBufferHeight; }

	static CGame* GetInstance();

	float GetGameSpeed() { return gameSpeed; }
	void DecreaseGameSpeed();
	void IncreaseGameSpeed();
	void PauseGame();
	bool IsPaused() { return isPaused; }
	void StopAnimation(bool isStoppping) { isStopppingAnimation = isStoppping; }
	bool IsStoppingAnimation() const { return isStopppingAnimation; }

	void SetPointSamplerState();

	void SetCamPos(float x, float y) { cam_x = x; cam_y = y; }
	void GetCamPos(float& x, float& y) { x = cam_x; y = cam_y; }

	LPSCENE GetCurrentScene() { return scenes[current_scene]; }
	LPGAMESTATE GetGameState() { return gameState; }
	void Load(LPCWSTR gameFile);
	void SwitchScene();
	void InitiateSwitchScene(int scene_id);
	void ReloadScene();
	void RestartScene();
	void ReloadAssets();

	void _ParseSection_TEXTURES(string line);


	~CGame();
};
typedef CGame* LPGAME;

