/*==============================================================================

	Manage System Logic [System_Logic_Manager.cpp]

	Author : Choi HyungJoon

==============================================================================*/
#include "Project_Header.h"
#include "System_Logic_Manager.h"
#include "Main_Header.h"
#include "Debug_Collision.h"
#include "Event_Register.h"

#if defined(DEBUG) || defined(_DEBUG)
#include "Debug_Camera.h"
static bool Is_Debug_Camera_On = false;
#endif	

void System_Manager::Initialize(HWND hWnd, ID3D11Device* Device, ID3D11DeviceContext* Context)
{
	// Initialize System Tools
	SystemTimer_Initialize();
	KeyLogger_Initialize(hWnd);

	// Initialize Audio Tools
	Audio_Manager::GetInstance()->Init();
	Game_Audio_Initialize();

	// Initialize Shader Tools
	Shader_Manager::GetInstance()->Init(Device, Context);
	Light_Manager::GetInstance().Init();

	// Initialize Texture Tools
	Grid_Initialize(Device, Context);
	Cube_Initialize(Device, Context);
	Texture_Manager::GetInstance()->Init(Device, Context);
	Game_Texture_Initialize();
	Sprite_Initialize(Device, Context);
	SpriteAni_Initialize();
	Billboard_Manager::GetInstance().Init();
	Particle_Manager::GetInstance().Init();
	Map_Manager::GetInstance().Init();

	// Initialize Game Setting Tools
	Fade_Initialize();

	// Initialize Event System
	Register_All_Events();

	// Initialize Game Logic
	Enemy_Manager::GetInstance().Init();
	Bullet_Manager::GetInstance().Init();

	// Initialize Debug Tools
	GUI_Init(hWnd, Device, Context);
	Debug_Camera_Initialize();
	Debug_Collision_Initialize(Device);

	// Initialize Game Logic
	Game_Logic_Initialize();
}

void System_Manager::Update(double elapsed_time, bool IS_Controller_Set)
{
	Fade_Update(elapsed_time);

	// Set Key Logger With FPS
	KeyLogger_Update();
	Controller_Set_Update();
	SpriteAni_Update(elapsed_time);

	IS_Controller_Set = Controller_Set_UP();

	// When Controller Is Not Set, Update Main Logic (Player, Enemy, etc.)
	if (!IS_Controller_Set)
	{
#if defined(DEBUG) || defined(_DEBUG)
		static bool IS_DEBUG_MODE = false;
		if (KeyLogger_IsTrigger(KK_F1))
		{
			IS_DEBUG_MODE = !IS_DEBUG_MODE;
		}

		if (IS_DEBUG_MODE)
		{
			Debug_Camera_Update(elapsed_time);
		}
		else
		{
			Game_Logic_Update(elapsed_time);
		}
#else
		// Update Main Logic For Release
		Game_Logic_Update(elapsed_time);
#endif
	}
}

void System_Manager::Draw(double FPS)
{
	// Draw Texture
	Direct3D_Clear();
	Sprite_Begin();

	// Real Draw Start
	Main_Game_Screen_Draw_Update();

	// Controller Input Alert
	Controller_Set_Draw();

	// Fade Draw
	Fade_Draw();

#if defined(DEBUG) || defined(_DEBUG)
	// Draw GUI
	// Start the Dear ImGui frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	ImGui::GetIO().MouseDrawCursor = true;

	// Game Scene Editor
	GUI_Screen_Scene_Editor(FPS);

	// Player, Camera Editor
	GUI_Player_Editor();

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
#endif	

	Direct3D_Present();
}

void System_Manager::Finalize()
{
	// Finalize Main Logic
	Game_Logic_Finalize();

	// Finalize Debug Tools
	Debug_Collision_Finalize();
	Debug_Camera_Finalize();
	GUI_Final();

	// Initialize Game Logic
	Bullet_Manager::GetInstance().Final();
	Enemy_Manager::GetInstance().Final();

	// Finalize Game Setting Tools
	Fade_Finalize();

	// Finalize Texture Tools
	Map_Manager::GetInstance().Final();
	Particle_Manager::GetInstance().Final();
	Billboard_Manager::GetInstance().Final();
	SpriteAni_Finalize();
	Sprite_Finalize();
	Game_Texture_Finalize();
	Texture_Manager::GetInstance()->Final();
	Cube_Finalize();
	Grid_Finalize();

	// Finalize Shader Tools
	Shader_Manager::GetInstance()->Final();

	// Finalize Audio Tools
	Game_Audio_Finalize();
	Audio_Manager::GetInstance()->Final();

	// Finalize System Tools
	Direct3D_Finalize();
	Mouse_Finalize();
}