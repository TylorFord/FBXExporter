#define _CRT_SECURE_NO_WARNINGS
#if _DEBUG
#define _CRTDBG_MAP_ALLOC  
#include <crtdbg.h>  
#endif
#include <stdlib.h>  
#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <tchar.h>
#include <iostream>
#include <time.h>
#include <fstream>
#include <vector>

#include "renderer.h"
#include "XTime.h"
#include "pools.h"
#include "frustum_culling.h"

#include <bitset>


using namespace end;

// Global variables
struct Particle
{
	XMFLOAT3 pos;
	XMFLOAT3 vel;
	XMFLOAT4 col;
};

struct meshData
{
	XMFLOAT3 norm;
	XMFLOAT3 center;
	int indices[3];
};

struct Emitter
{
	XMFLOAT3 spawnPos;
	XMFLOAT4 spawnCol;
	sorted_pool_t<int16_t, 256> indices;
};

XTime timer;
float gravity = 1;
XMFLOAT3 launch = { 1, 3, 1 };

bool keyPress = false;
std::bitset<4> keys;
std::bitset<1> mouseclick;
POINT mousepos;
POINT mouseprevpos;

// The main window class name.
static TCHAR szWindowClass[] = _T("DesktopApp");

// The string that appears in the application's title bar.
static TCHAR szTitle[] = _T("ENDRenderer");

HINSTANCE hInst;


float Lerp(float first, float second, float ratio)
{
	return (second - first) * ratio + first;
}

// Forward declarations of functions included in this code module:
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int CALLBACK WinMain(
	_In_ HINSTANCE hInstance,
	_In_ HINSTANCE hPrevInstance,
	_In_ LPSTR     lpCmdLine,
	_In_ int       nCmdShow
)
{
#if _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(397);
#endif
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

	if (!RegisterClassEx(&wcex))
	{
		MessageBox(NULL,
			_T("Call to RegisterClassEx failed!"),
			_T("Windows Desktop Guided Tour"),
			NULL);

		return 1;
	}

	// Store instance handle in our global variable
	hInst = hInstance;


#ifndef NDEBUG 
	AllocConsole(); FILE* new_std_in_out;
	freopen_s(&new_std_in_out, "CONOUT$", "w", stdout); freopen_s(&new_std_in_out, "CONIN$", "r", stdin);
	std::cout << "Hello, yes this is debug." << std::endl;

#endif
	// The parameters to CreateWindow explained:
	// szWindowClass: the name of the application
	// szTitle: the text that appears in the title bar
	// WS_OVERLAPPEDWINDOW: the type of window to create
	// CW_USEDEFAULT, CW_USEDEFAULT: initial position (x, y)
	// 500, 100: initial size (width, length)
	// NULL: the parent of this window
	// NULL: this application does not have a menu bar
	// hInstance: the first parameter from WinMain
	// NULL: not used in this application
	HWND hWnd = CreateWindow(
		szWindowClass,
		szTitle,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		1280, 720,
		NULL,
		NULL,
		hInstance,
		NULL
	);

	if (!hWnd)
	{
		MessageBox(NULL,
			_T("Call to CreateWindow failed!"),
			_T("Windows Desktop Guided Tour"),
			NULL);

		return 1;
	}

	// The parameters to ShowWindow explained:
	// hWnd: the value returned from CreateWindow
	// nCmdShow: the fourth parameter from WinMain
	ShowWindow(hWnd,
		nCmdShow);
	UpdateWindow(hWnd);
	
	srand(1);
	renderer render(hWnd);
	//							 look at						 turn to					 player
	XMMATRIX matrices[] = { XMMatrixTranslation(5, 2, 5), XMMatrixTranslation(-5, 2, -5), XMMatrixTranslation(0, 0.1f, 0) };

	bool play = false;
	// Main message loop:
	MSG msg;
	while (true)
	{
		// Process all messages, stop on WM_QUIT 
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			// WM_QUIT does not need to be // translated or dispatched 
			if (msg.message == WM_QUIT) break;
			// Translates messages and sends them to WndProc 
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			// In the future, do per frame/tick updates here... 
			WndProc(hWnd, msg.message, msg.wParam, msg.lParam);
			timer.Signal();
			float deltaT = (float)timer.Delta();
			float totaltT = (float)timer.TotalTimeExact();
			render.CameraMovement(deltaT);
			render.DebugGrid(totaltT);


			if (GetAsyncKeyState(VK_OEM_MINUS) & 0x1 && !play)
			{
				render.AnimationStep(-1);
			}
			if (GetAsyncKeyState(VK_OEM_PLUS) & 0x1 || play)
			{
				render.debug_lines(totaltT);
			}
			if (GetAsyncKeyState('P') & 0x1)
			{
				play = !play;
			}


			if (mouseclick.test(0))
			{
				POINT delta;
				delta.x = mousepos.x - mouseprevpos.x;
				delta.y = mousepos.y - mouseprevpos.y;
				mouseprevpos = mousepos;

				render.MouseRotation((int)delta.x, (int)delta.y, deltaT);
				//std::cout << delta.x << " " << delta.y << std::endl;
			}
			if (keys.test(0))
			{
				XMVECTOR pos = matrices[2].r[3];
				XMVECTOR reset = { 0,0,0 };
				matrices[2].r[3] = reset;
				matrices[2] *= XMMatrixRotationY(-deltaT * 2);
				matrices[2].r[3] = pos;
			}
			if (keys.test(1))
			{
				matrices[2] = XMMatrixTranslation(0, 0, deltaT * 5) * matrices[2];
			}
			if (keys.test(2))
			{
				XMVECTOR scale;
				XMVECTOR pos;
				XMVECTOR rot;
				XMVECTOR reset = { 0,0,0 };
				XMMatrixDecompose(&scale, &rot, &pos, matrices[2]);
				matrices[2].r[3] = reset;
				matrices[2] *= XMMatrixRotationY(deltaT * 2);
				matrices[2].r[3] = pos;
			}
			if (keys.test(3))
			{
				matrices[2] = XMMatrixTranslation(0, 0, -deltaT * 5) * matrices[2];
			}
			//XMVector3Transform

			//matrices[0] = render.LookAt(matrices[0].r[3], matrices[2].r[3], XMVECTOR{ 0,1,0 });
			//matrices[1] = render.TurnTo(matrices[1], matrices[2].r[3], deltaT * 100);
			//render.matricesAxis(matrices);

			render.draw();
			render.ClearDebugLines();
		//	std::cout << timer.TotalTime() << std::endl;
		}
	}
	//delete mesh;


#ifndef NDEBUG 
	FreeConsole();
#endif 

	return (int)msg.wParam;
}

//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	//	PAINTSTRUCT ps;
		//HDC hdc;
	TCHAR greeting[] = _T("Hello, Windows desktop!");

	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_KEYDOWN:
	{
		switch (wParam)
		{
		case VK_LEFT:
			keys.set(0, true);

			break;
		case VK_UP:
			keys.set(1, true);

			break;
		case VK_RIGHT:
			keys.set(2, true);

			break;
		case VK_DOWN:
			keys.set(3, true);

			break;

		default:
			break;
		}
		break;
	}
	case WM_KEYUP:
		switch (wParam)
		{
		case VK_LEFT:
			keys.set(0, false);

			break;
		case VK_UP:
			keys.set(1, false);

			break;
		case VK_RIGHT:
			keys.set(2, false);

			break;
		case VK_DOWN:
			keys.set(3, false);

			break;

		default:
			break;
		}
		break;
	case WM_LBUTTONDOWN:
		mouseclick.set(0, true);
		break;
	case WM_LBUTTONUP:
		mouseclick.set(0, false);
		break;
	case WM_MOUSEMOVE:
		GetCursorPos(&mousepos);
		if (!mouseclick.test(0))
		{
			mouseprevpos = mousepos;
		}
		break;
	case WM_SIZE:
	{
		int Width = LOWORD(lParam);
		int Height = HIWORD(lParam);
	}
	break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}

	return 0;
}