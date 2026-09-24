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
#include "Game_Window.h"
#include "Billboard_Manager.h"
#include "Game_Screen_Manager.h"

// Other
#include "Enemy.h"
#include "Weapon_System.h"
#include "debug_ostream.h"
#include "Player_Camera.h"

//---------------Texture Info---------------//
struct Text_UI_Data
{
	int Tex_ID = -1;
	float X = 0.0f, Y = 0.0f;
	float W = 0.0f, H = 0.0f;
};

//---------------UI Info---------------//
struct Dynamic_UI_Data
{
	int MG_Aim_ID = -1;
	int Missile_Aim_ID = -1;
	int LockOn_ID = -1;
	int Mouse_ID = -1;

	float MG_Size = 0.0f;
	float Missile_Size = 0.0f;
	float Mouse_Size = 0.0f;
};

//---------------UI struct Info---------------//
static Text_UI_Data UI_HighScore, UI_Score, UI_Stage, UI_Hit;
static Dynamic_UI_Data Dynamic_UI;

//---------------Mouse Info---------------//
static float Mouse_X = 0.0f, Mouse_Y = 0.0f;
static bool Is_Pure_Main = false;
static bool Is_Pure_In_Game = false;
static WeaponType Current_Weapon = WeaponType::MACHINE_GUN;
static constexpr float Mouse_Size = 0.05f;

//---------------Private Logic---------------//
void Game_UI_Texture();
void Aim_UI_Initialize(float H);
void In_Game_UI_Initialize(float W, float H);

void UI_Draw_Text();
void UI_Draw_Aim();

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

	Current_Weapon = Weapon_Manager::GetInstance().Get_Current_Weapon();
}

void Game_UI_Draw()
{
	if (!Is_Pure_Main) return;

	Direct3D_SetDepthEnable(false);
	Shader_Manager::GetInstance()->Begin2D();

	if (Is_Pure_In_Game)
	{
		UI_Draw_Aim();
		UI_Draw_Text();
	}
	else if (!Is_Pure_In_Game && Dynamic_UI.Mouse_ID != -1)
	{
		Sprite_Draw(Dynamic_UI.Mouse_ID, Mouse_X, Mouse_Y, Dynamic_UI.Mouse_Size, Dynamic_UI.Mouse_Size);
	}
}

// ----------------------------------------------------------------------------------------------------------------
//													Private Logic
// ----------------------------------------------------------------------------------------------------------------
void Game_UI_Texture()
{
	//------------------Aim UI Texture------------------//
	Dynamic_UI.Mouse_ID = Texture_Manager::GetInstance()->GetID("UI_Mouse_Cursor");
	Dynamic_UI.MG_Aim_ID = Texture_Manager::GetInstance()->GetID("Aim_MachineGun");
	Dynamic_UI.Missile_Aim_ID = Texture_Manager::GetInstance()->GetID("Aim_Missile");
	Dynamic_UI.LockOn_ID = Texture_Manager::GetInstance()->GetID("Aim_Lock_On");

	//------------------In Game UI Texture------------------//
	UI_Score.Tex_ID = Texture_Manager::GetInstance()->GetID("In_Game_Score");
	UI_HighScore.Tex_ID = Texture_Manager::GetInstance()->GetID("In_Game_High_Score");
	UI_Stage.Tex_ID = Texture_Manager::GetInstance()->GetID("In_Game_Stage");
	UI_Hit.Tex_ID = Texture_Manager::GetInstance()->GetID("In_Game_Destroyed");

	if (Dynamic_UI.Mouse_ID == -1 || Dynamic_UI.MG_Aim_ID == -1 || Dynamic_UI.Missile_Aim_ID == -1 || Dynamic_UI.LockOn_ID == -1 ||
		UI_Score.Tex_ID == -1 || UI_HighScore.Tex_ID == -1 || UI_Stage.Tex_ID == -1 || UI_Hit.Tex_ID == -1)
	{
		Debug::D_Out << "[Game UI] Texture Init Error" << std::endl;
		Debug::D_Out << "\t UI_Mouse : " << Dynamic_UI.Mouse_ID			<< "\t UI_Aim_MachineGun : " << Dynamic_UI.MG_Aim_ID
			<< "\t UI_Aim_Missile : "	 << Dynamic_UI.Missile_Aim_ID	<< "\t UI_LockOn : "		 << Dynamic_UI.LockOn_ID
			<< "\t UI_Score : "			 << UI_Score.Tex_ID				<< "\t UI_HighScore : "		 << UI_HighScore.Tex_ID
			<< "\t UI_Stage : "			 << UI_Stage.Tex_ID				<< "\t UI_Hit : "			 << UI_Hit.Tex_ID << std::endl;
	}
}

void Aim_UI_Initialize(float H)
{
	Dynamic_UI.Mouse_Size = H * Mouse_Size;
	Dynamic_UI.MG_Size = H * 0.16f;

	float Fov_Half = DirectX::XMConvertToRadians(30.0f);
	float Distance = 160.0f;
	float World_Radius = 4.0f;
	float Projected_Radius = (World_Radius / (Distance * std::tan(Fov_Half))) * H;
	Dynamic_UI.Missile_Size = Projected_Radius * 2.0f;
}

void In_Game_UI_Initialize(float W, float H)
{
	// Set In-Game UI Size
	UI_Hit.W = W * 0.1f;
	UI_Hit.H = H * 0.1f;

	UI_Score.W = UI_Hit.W;
	UI_Score.H = UI_Hit.H;

	UI_HighScore.W = UI_Hit.W * 0.5f;
	UI_HighScore.H = UI_Hit.H;

	UI_Stage.W = UI_Hit.W * 0.5f;
	UI_Stage.H = UI_Hit.H;

	// Set In-Game UI Position
	float Center_HighScore_X = W * 0.1f;
	float Center_HighScore_Y = H * 0.1f;
	UI_HighScore.X = Center_HighScore_X - (UI_HighScore.W * 0.5f);
	UI_HighScore.Y = Center_HighScore_Y - (UI_HighScore.H * 0.5f);

	float Center_Score_X = Center_HighScore_X;
	float Center_Score_Y = Center_HighScore_Y + UI_HighScore.H;
	UI_Score.X = Center_Score_X - (UI_Score.W * 0.5f);
	UI_Score.Y = Center_Score_Y - (UI_Score.H * 0.5f);

	float Center_Hit_X = W * 0.9f;
	float Center_Hit_Y = Center_HighScore_Y;
	UI_Hit.X = Center_Hit_X - (UI_Hit.W * 0.5f);
	UI_Hit.Y = Center_Hit_Y - (UI_Hit.H * 0.5f);

	float Center_Stage_X = Center_Hit_X;
	float Center_Stage_Y = Center_Hit_Y + UI_Hit.H;
	UI_Stage.X = Center_Stage_X - (UI_Stage.W * 0.5f);
	UI_Stage.Y = Center_Stage_Y - (UI_Stage.H * 0.5f);
}

void UI_Draw_Text()
{
	if (UI_HighScore.Tex_ID == -1 || UI_Score.Tex_ID == -1 || UI_Hit.Tex_ID == -1 || UI_Stage.Tex_ID == -1)
		return;

	Sprite_Draw(UI_HighScore.Tex_ID, UI_HighScore.X, UI_HighScore.Y, UI_HighScore.W, UI_HighScore.H);
	Sprite_Draw(UI_Score.Tex_ID, UI_Score.X, UI_Score.Y, UI_Score.W, UI_Score.H);
	Sprite_Draw(UI_Hit.Tex_ID, UI_Hit.X, UI_Hit.Y, UI_Hit.W, UI_Hit.H);
	Sprite_Draw(UI_Stage.Tex_ID, UI_Stage.X, UI_Stage.Y, UI_Stage.W, UI_Stage.H);
}

void UI_Draw_Aim()
{
	if (Dynamic_UI.LockOn_ID != -1)
	{
		float W = static_cast<float>(Window_Manager::GetInstance()->GetWidth());
		float H = static_cast<float>(Window_Manager::GetInstance()->GetHeight());

		DirectX::XMMATRIX View = DirectX::XMLoadFloat4x4(&Player_Camera_Get_View_Matrix());
		DirectX::XMMATRIX Proj = DirectX::XMLoadFloat4x4(&Player_Camera_Get_Proj_Matrix());
		DirectX::XMMATRIX World = DirectX::XMMatrixIdentity();

		for (const LockOn_Data& L : Weapon_Manager::GetInstance().Return_Lock_On_List())
		{
			if (L.Target_Ptr->IsActive() && L.Target_Ptr->GetUniqueID() == L.Target_ID)
			{
				// Get Lock-On Target Position
				DirectX::XMFLOAT3 Target_Pos = L.Target_Ptr->GetPosition();
				DirectX::XMVECTOR V_World_Pos = DirectX::XMLoadFloat3(&Target_Pos);

				// Transform 3D POS To 2D Screen POS
				DirectX::XMVECTOR V_Screen_Pos = DirectX::XMVector3Project(V_World_Pos, 0.0f, 0.0f, W, H, 0.0f, 1.0f, Proj, View, World);
				float Screen_X = DirectX::XMVectorGetX(V_Screen_Pos);
				float Screen_Y = DirectX::XMVectorGetY(V_Screen_Pos);

				float LockOn_Size = Dynamic_UI.Missile_Size;
				Sprite_Draw(Dynamic_UI.LockOn_ID, Screen_X - (LockOn_Size * 0.5f), Screen_Y - (LockOn_Size * 0.5f), LockOn_Size, LockOn_Size);
			}
		}
	}

	if (Current_Weapon == WeaponType::MACHINE_GUN && Dynamic_UI.MG_Aim_ID != -1)
	{
		float Draw_X = Mouse_X - (Dynamic_UI.MG_Size * 0.5f);
		float Draw_Y = Mouse_Y - (Dynamic_UI.MG_Size * 0.5f);
		Sprite_Draw(Dynamic_UI.MG_Aim_ID, Draw_X, Draw_Y, Dynamic_UI.MG_Size, Dynamic_UI.MG_Size);
	}
	else if (Current_Weapon == WeaponType::MISSILE && Dynamic_UI.Missile_Aim_ID != -1)
	{
		float Draw_X = Mouse_X - (Dynamic_UI.Missile_Size * 0.5f);
		float Draw_Y = Mouse_Y - (Dynamic_UI.Missile_Size * 0.5f);
		Sprite_Draw(Dynamic_UI.Missile_Aim_ID, Draw_X, Draw_Y, Dynamic_UI.Missile_Size, Dynamic_UI.Missile_Size);
	}
}
