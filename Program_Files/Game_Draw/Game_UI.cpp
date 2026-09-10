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

static int UI_Aim_MachineGun = -1, UI_Aim_Missile = -1, UI_LockOn = -1;
static int Current_Weapon_Aim_UI = -1;

static int UI_Score = -1, UI_HighScore = -1, UI_Stage = -1, UI_Hit = -1;

//---------------UI Rect Info---------------//
static float Score_X = 0, Score_Y = 0, Score_W = 0, Score_H = 0;
static float HighScore_X = 0, HighScore_Y = 0, HighScore_W = 0, HighScore_H = 0;
static float Stage_X = 0, Stage_Y = 0, Stage_W = 0, Stage_H = 0;
static float Hit_X = 0, Hit_Y = 0, Hit_W = 0, Hit_H = 0;

//---------------Mouse State---------------//
static float Mouse_X = 0.0f, Mouse_Y = 0.0f;
static float Draw_X = 0.0f,  Draw_Y = 0.0f;
static bool Is_Pure_Main	= false;
static bool Is_Pure_In_Game = false;

static float Mouse_UI_Size = 0.0f;

static float Missile_Aim_Size = 0.0f, MG_Aim_Size = 0.0f;

static constexpr float Mouse_Size = 0.05f;

//---------------Private Logic---------------//
void Game_UI_Texture();
void Aim_UI_Initialize(float H);
void In_Game_UI_Initialize(float W, float H);

//-----------------Main Logic-----------------//
void Game_UI_Initialize()
{
	Game_UI_Texture();

	float screenW = static_cast<float>(Direct3D_GetBackBufferWidth());
	float screenH = static_cast<float>(Direct3D_GetBackBufferHeight());

	Aim_UI_Initialize(screenH);
	In_Game_UI_Initialize(screenW, screenH);
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

	WeaponType Current_Weapon = Weapon_Manager::GetInstance().Get_Current_Weapon();
	float Current_Aim_Size = (Current_Weapon == WeaponType::MACHINE_GUN) ? MG_Aim_Size : Missile_Aim_Size;

	// For In-Game Aim Draw Position
	Draw_X = Mouse_X - (Current_Aim_Size * 0.5f);
	Draw_Y = Mouse_Y - (Current_Aim_Size * 0.5f);

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
					Billboard_Draw(UI_LockOn, L.Target_Ptr->GetPosition(), 3.0f, 3.0f, { 0.5f, 0.5f }, { 1.0f, 1.0f, 1.0f, 1.0f }, Billboard_Facing::ALL_AXIS);
				}
			}
		}
	}

	// -------------------------------------------------------------
	// 2D UI Draw Logic
	// -------------------------------------------------------------
	Direct3D_SetDepthEnable(false);
	Shader_Manager::GetInstance()->Begin2D();

	if (Is_Pure_In_Game && UI_HighScore != -1 && UI_Score != -1 && UI_Hit != -1 && UI_Stage != -1
		&& Current_Weapon_Aim_UI != -1)
	{
		Sprite_Draw(UI_HighScore, HighScore_X, HighScore_Y, HighScore_W, HighScore_H);
		Sprite_Draw(UI_Score, Score_X, Score_Y, Score_W, Score_H);
		Sprite_Draw(UI_Hit, Hit_X, Hit_Y, Hit_W, Hit_H);
		Sprite_Draw(UI_Stage, Stage_X, Stage_Y, Stage_W, Stage_H);

		float Current_Aim_Size = (Weapon_Manager::GetInstance().Get_Current_Weapon() == WeaponType::MACHINE_GUN) ? MG_Aim_Size : Missile_Aim_Size;
		Sprite_Draw(Current_Weapon_Aim_UI, Draw_X, Draw_Y, Current_Aim_Size, Current_Aim_Size);
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
	UI_LockOn = Texture_Manager::GetInstance()->GetID("Aim_Lock_On");

	//------------------In Game UI Texture------------------//
	UI_Score = Texture_Manager::GetInstance()->GetID("In_Game_Score");
	UI_HighScore = Texture_Manager::GetInstance()->GetID("In_Game_High_Score");
	UI_Stage = Texture_Manager::GetInstance()->GetID("In_Game_Stage");
	UI_Hit = Texture_Manager::GetInstance()->GetID("In_Game_Destroyed");

	if (UI_Mouse == -1 || UI_Aim_MachineGun == -1 || UI_Aim_Missile == -1 || UI_LockOn == -1 ||
		UI_Score == -1 || UI_HighScore == -1 || UI_Stage == -1 || UI_Hit == -1)
	{
		Debug::D_Out << "[Game UI] Texture Init Error" << std::endl;
		Debug::D_Out << "\t UI_Mouse : "	<< UI_Mouse			<< "\t UI_Aim_MachineGun : "	<< UI_Aim_MachineGun
			<< "\t UI_Aim_Missile : "		<< UI_Aim_Missile	<< "\t UI_LockOn : "			<< UI_LockOn
			<< "\t UI_Score : "				<< UI_Score			<< "\t UI_HighScore : "			<< UI_HighScore
			<< "\t UI_Stage : "				<< UI_Stage			<< "\t UI_Hit : "				<< UI_Hit << std::endl;
	}
}

void Aim_UI_Initialize(float H)
{
	Mouse_UI_Size = H * Mouse_Size;

	MG_Aim_Size = H * 0.16f;

	float Fov_Half = DirectX::XMConvertToRadians(30.0f);
	float Distance = 160.0f;
	float World_Radius = 4.0f;
	float Projected_Radius = (World_Radius / (Distance * std::tan(Fov_Half))) * H;
	Missile_Aim_Size = Projected_Radius * 2.0f;
}

void In_Game_UI_Initialize(float W, float H)
{
	// Set In-Game UI Size
	Hit_W = W * 0.1f;
	Hit_H = H * 0.1f;

	Score_W = Hit_W;
	Score_H = Hit_H;

	HighScore_W = Hit_W * 0.5f;
	HighScore_H = Hit_H;

	Stage_W = Hit_W * 0.5f;
	Stage_H = Hit_H;

	// Set In-Game UI Position
	float Center_HighScore_X = W * 0.1f;
	float Center_HighScore_Y = H * 0.1f;
	HighScore_X = Center_HighScore_X - (HighScore_W * 0.5f);
	HighScore_Y = Center_HighScore_Y - (HighScore_H * 0.5f);

	float Center_Score_X = Center_HighScore_X;
	float Center_Score_Y = Center_HighScore_Y + HighScore_H;
	Score_X = Center_Score_X - (Score_W * 0.5f);
	Score_Y = Center_Score_Y - (Score_H * 0.5f);

	float Center_Hit_X = W * 0.9f;
	float Center_Hit_Y = Center_HighScore_Y;
	Hit_X = Center_Hit_X - (Hit_W * 0.5f);
	Hit_Y = Center_Hit_Y - (Hit_H * 0.5f);

	float Center_Stage_X = Center_Hit_X;
	float Center_Stage_Y = Center_Hit_Y + Hit_H;
	Stage_X = Center_Stage_X - (Stage_W * 0.5f);
	Stage_Y = Center_Stage_Y - (Stage_H * 0.5f);
}
