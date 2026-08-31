/*==============================================================================

	Game UI Manager [Game_UI.h]

	Author : Choi HyungJoon

==============================================================================*/
#include "Project_Header.h"
#include "Game_UI.h"

// Assential Logic
#include "Fade.h"
#include "Input_Manager.h"
#include "Event_Manager.h"

// Other
#include "Enemy.h"
#include "Game_Screen_Manager.h"
#include "Weapon_System.h"
#include "debug_ostream.h"
#include "Billboard_Manager.h"

//---------------Texture Info---------------//
static int UI_Mouse = -1;

static int UI_Aim_MachineGun = -1;
static int UI_Aim_Missile = -1;
static int UI_LockOn = -1;

static int Current_Weapon_Aim_UI = -1;

//---------------Mouse State---------------//
static float Mouse_X = 0.0f, Mouse_Y = 0.0f;
static float Draw_X = 0.0f,  Draw_Y = 0.0f;
static bool Is_Pure_Main	= false;
static bool Is_Pure_In_Game = false;

static float Mouse_UI_Size = 0.0f;
static float Aim_Size = 0.0f;

static constexpr float Mouse_Size = 0.05f;

//---------------Private Logic---------------//
void Game_UI_Texture();

//-----------------Main Logic-----------------//
void Game_UI_Initialize()
{
	Game_UI_Texture();

	float screenH = static_cast<float>(Direct3D_GetBackBufferHeight());

	Mouse_UI_Size = screenH * Mouse_Size;
	Aim_Size = screenH * 0.08f;
}

void Game_UI_Finalize()
{
}

void Game_UI_Update(float dt)
{
	Mouse_X = M_INPUT->Mouse_Position().x;
	Mouse_Y = M_INPUT->Mouse_Position().y;

	Is_Pure_Main	= Game_Screen_Manager::GetInstance()->Is_Main_Screen_Active();
	Is_Pure_In_Game = Game_Screen_Manager::GetInstance()->Is_Pure_In_Game_State();

	// For In-Game Aim Draw Position
	Draw_X = Mouse_X - (Aim_Size * 0.5f);
	Draw_Y = Mouse_Y - (Aim_Size * 0.5f);

	WeaponType Current_Weapon = Weapon_Manager::GetInstance().Get_Current_Weapon();
	switch (Current_Weapon)
	{
	case WeaponType::MACHINE_GUN:
		Current_Weapon_Aim_UI = UI_Aim_MachineGun;
		break;
	case WeaponType::MISSILE:
		Current_Weapon_Aim_UI = UI_Aim_Missile;
		break;
	default:
		Debug::D_Out << "[Game UI] Warning : Current Weapon Missing. Weapon Info : " << static_cast<int>(Current_Weapon) << std::endl;
		break;
	}
}

void Game_UI_Draw()
{
	if (!Is_Pure_Main) return;

	// -------------------------------------------------------------
	// 3D Lock-On Draw Logic
	// -------------------------------------------------------------
	if (Is_Pure_In_Game)
	{
		Direct3D_SetDepthEnable(true);
		Shader_Manager::GetInstance()->Begin3D();

		if (UI_LockOn != -1)
		{
			for (const LockOn_Data& L : Weapon_Manager::GetInstance().Return_Lock_On_List())
			{
				if (L.Target_Ptr->IsActive() && L.Target_Ptr->GetUniqueID() == L.Target_ID)
				{
					Billboard_Draw(UI_LockOn, L.Target_Ptr->GetPosition(), 1.5f, 1.5f, { 0.5f, 0.5f }, { 1.0f, 1.0f, 1.0f, 1.0f }, Billboard_Facing::ALL_AXIS);
				}
			}
		}
	}

	// -------------------------------------------------------------
	// 2D UI Draw Logic
	// -------------------------------------------------------------
	Direct3D_SetDepthEnable(false);
	Shader_Manager::GetInstance()->Begin2D();

	if (Is_Pure_In_Game && Current_Weapon_Aim_UI != -1)
	{
		Sprite_Draw(Current_Weapon_Aim_UI, Draw_X, Draw_Y, Aim_Size, Aim_Size);
	}
	else if (!Is_Pure_In_Game && UI_Mouse != -1)
	{
		Sprite_Draw(UI_Mouse, Mouse_X, Mouse_Y, Mouse_UI_Size, Mouse_UI_Size);
	}
}

void Game_UI_Texture()
{
	//------------------Menu UI Texture------------------//
	UI_Mouse = Texture_Manager::GetInstance()->GetID("UI_Mouse_Cursor");

	//------------------Aim UI Texture------------------//
	UI_Aim_MachineGun = Texture_Manager::GetInstance()->GetID("Aim_MachineGun");
	UI_Aim_Missile = Texture_Manager::GetInstance()->GetID("Aim_Missile");

	//------------------In Game UI Texture------------------//
	UI_LockOn = Texture_Manager::GetInstance()->GetID("Aim_Lock_On");

	if (UI_Mouse == -1 || UI_Aim_MachineGun == -1 || UI_Aim_Missile == -1 || UI_LockOn == -1)
	{
		Debug::D_Out << "[Game UI] Texture Init Error" << std::endl;
		Debug::D_Out << "\tUI_Mouse : "	<< UI_Mouse
			<< "\tUI_Aim_MachineGun : "	<< UI_Aim_MachineGun
			<< "\tUI_Aim_Missile : "		<< UI_Aim_Missile
			<< "\tUI_LockOn : "			<< UI_LockOn << std::endl;
	}
}