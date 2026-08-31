/*==============================================================================

	Project Main CPP [Main.cpp]

	Project : DirectX 11 / 2.5D Rail Shooting Game
	Project Name : Void Nova - Vanguard

	Author : Choi HyungJoon
	Date : 2026. 08. 21 ~ Ing
	Version : Alpha Ver.5
	Note : Add In-Game Details, UI Layout Adjustment, Make Look Like A "Game"

	Version List : 
			Date : 2026. 06. 19
			Version : Prototype
			Note : Main Logic & Game Loop Completed

			Date : 2026. 07. 12
			Version : Alpha Ver.1
			Note : Make Intrio And Menu Detail

			Date : 2026. 08. 14
			Version : Alpha Ver.2
			Note : Make Event Manager, And Project Reorganization To OOP

			Date : 2026. 08. 21
			Version : Alpha Ver.4
			Note : Enemy Spawn Logic Modularization, Make Main/Setting/In-Game Menu UI

==============================================================================*/
#include "Project_Header.h"
#include "System_Logic_Manager.h"
#include "Main_Header.h"
using namespace std;
using namespace DirectX;

int APIENTRY WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR IpCmdline, _In_ int nCmdShow)
{
	// Do Not Show Error For Not Used Resource, Not Necessary
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(IpCmdline);

	// Make Game Loop & Message Loop
	MSG Message = {};

	// Initialize COM Library
	HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	if (FAILED(hr))
	{
		MessageBoxW(nullptr, L"Failed To Set Function In Main.cpp : [CoInitializeEx]", L">> FATAL ERROR <<", MB_OK | MB_ICONERROR);
		return static_cast<int>(Message.wParam);
	}

	// Set DPI Aware
	SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

	// Create Window
	HWND hWnd = Window_Manager::GetInstance()->Init(hInstance, nCmdShow);
	if (!hWnd)
	{
		MessageBoxW(nullptr, L"Failed To Set Window : [HWND]", L">> FATAL ERROR <<", MB_OK | MB_ICONERROR);
		return static_cast<int>(Message.wParam);
	}

	// Initialize Direct 3D
	Direct3D_Initialize(hWnd);
	ID3D11Device* Device = Direct3D_GetDevice();
	if (!Device)
	{
		MessageBoxW(nullptr, L"Failed To Set DirectX System : [Device]", L">> FATAL ERROR <<", MB_OK | MB_ICONERROR);
		return static_cast<int>(Message.wParam);
	}

	ID3D11DeviceContext* Context = Direct3D_GetContext();
	if (!Context)
	{
		MessageBoxW(nullptr, L"Failed To Set DirectX System : [Context]", L">> FATAL ERROR <<", MB_OK | MB_ICONERROR);
		return static_cast<int>(Message.wParam);
	}

	// Initialize System Logic
	System_Manager::GetInstance().Initialize(hWnd, Device, Context);

	// Show Mouse (True = Show // False = Don`t Show)
	Mouse_SetVisible(false);
	// Mouse Postion Mode
	Mouse_SetMode(MOUSE_POSITION_MODE_RELATIVE);

	// Show Window
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	// Set Frame
	double Exec_Last_Time = SystemTimer_GetTime();
	double FPS_Last_Time = Exec_Last_Time;
	double Current_Time = 0.0;
	ULONG Frame_Count = 0;
	double FPS = 0.0; // Frame Per Second
	bool IS_Controller_Set = false;

	do
	{
		if (PeekMessage(&Message, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&Message);
			DispatchMessage(&Message);
		}

		// Set Game
		else
		{
			if (IF_IS_Game_Done())
			{
				// Game Done (EXIT)
				if (Fade_GetState() == FADE_STATE::FINISHED_OUT)
				{
					PostQuitMessage(0);
				}
			}

			// Time Set
			Current_Time = SystemTimer_GetTime(); // Get System Time
			double Elapsed_Time = Current_Time - FPS_Last_Time; // Get Time For FPS

			//  Set FPS
			if (Elapsed_Time >= 1.0)
			{
				FPS = Frame_Count / Elapsed_Time;
				FPS_Last_Time = Current_Time; // Save FPS
				Frame_Count = 0; // Clear Count
			}

			// Set Elapsed Time
			Elapsed_Time = Current_Time - Exec_Last_Time;
			// If you want to limit FPS, set condition here
			if (Elapsed_Time >= (1.0 / 60.0)) // 60 FPS
			{
				Exec_Last_Time = Current_Time; // Save Last Time

				System_Manager::GetInstance().Update(Elapsed_Time, IS_Controller_Set);
				System_Manager::GetInstance().Draw(FPS);

				Frame_Count++;
			}
		}
	} while (Message.message != WM_QUIT);

	System_Manager::GetInstance().Finalize();

	return static_cast<int>(Message.wParam);
}