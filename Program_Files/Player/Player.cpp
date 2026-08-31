/*==============================================================================

	Manage Player Logic [Player.cpp]

	Author : Choi HyungJoon

==============================================================================*/
#include "Project_Header.h"
#include "Player.h"

// Assential Logic
#include "Palette.h"
#include "Event_Manager.h"
#include "Input_Manager.h"

// Other
#include "Shader_Manager.h"
#include "Player_Camera.h" 
#include "Debug_Collision.h"
#include "Billboard_Manager.h"
#include "Cube.h"
#include "Heapler_Logic.h"
#include "Game_Window.h"
#include "Bullet_Manager.h"
#include "Enemy_Manager.h"
#include "Weapon_System.h"
#include "Combat_Register.h"
#include "debug_ostream.h"
using namespace DirectX;
using namespace PALETTE;

#define XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE  7849
#define XINPUT_GAMEPAD_TRIGGER_THRESHOLD    30

constexpr XMFLOAT3 FIRST_POS = { 0, 0, 0 };

// Player Physics Constants
static XMFLOAT3 Player_Pos = {};
static XMFLOAT3 Aim_Pos = {};
static float Player_Move_Speed = 25.0f;
static float Aim_Move_Speed = 40.0f;

static float Player_Tension_Speed = 6.0f;
static float Safe_Zone_Ratio = 0.7f;

// Player Visual State 
enum class Player_Visual_State
{
	Bot_Left, Bot_Center, Bot_Right,
	Mid_Left, Mid_Center, Mid_Right,
	Top_Left, Top_Center, Top_Right 
};
static Player_Visual_State Visual_State = Player_Visual_State::Bot_Center;

// Player Visual Resources 
static int Player_Bot_C = -1, Player_Bot_L = -1, Player_Bot_R = -1;
static int Player_Mid_C = -1, Player_Mid_L = -1, Player_Mid_R = -1;
static int Player_Top_C = -1, Player_Top_L = -1, Player_Top_R = -1;

// Player resource
static XMFLOAT2 Player_Size = { 2.0f, 1.0f };
static float Player_HP = 100;
static float Player_MaxHP = 100;
static float Player_ATK = 10;
static float Player_DEF = 10;
static bool Is_Input_Moving = false;

// ----------------------------------------------------------
//				static Player Update Logic
// ----------------------------------------------------------
// --- Movement And Physics System ---
static XMVECTOR Player_Update_Movement_Input();

// --- Visual ---
static void Player_Texture();
static void Player_Update_Visual_State();
static int	Player_Update_Visual_Resources();

// --- Combat ---
static void Player_Update_Aim_Input(float dt);
static void Player_Update_Weapon_Logic(float dt);
// ----------------------------------------------------------

void Player_Initialize()
{
	Player_Texture();

	Player_Pos	= FIRST_POS;
	Aim_Pos		= FIRST_POS;

	float ScreenW = static_cast<float>(Direct3D_GetBackBufferWidth());
	float ScreenH = static_cast<float>(Direct3D_GetBackBufferHeight());

	// Player_Size.x = ScreenW * 0.1;
	// Player_Size.y = ScreenW * 0.1;
}

void Player_Finalize()
{
}

void Player_Update(float elapsed_time)
{
	// Aim Update
	Player_Update_Aim_Input(elapsed_time);

	// Movement Update
	XMVECTOR Move_Dir = Player_Update_Movement_Input();

	XMVECTOR Current_Pos = XMLoadFloat3(&Player_Pos);
	XMVECTOR Target_Aim = XMLoadFloat3(&Aim_Pos);

	// Clamped Player Movement Limit
	float Limit_X = Get_Player_Limit_X();
	float Limit_Y_Min = Get_Player_Limit_Y_Min();
	float Limit_Y_Max = Get_Player_Limit_Y_Max();

	if (Is_Input_Moving)
	{
		// If Input, Move In Limit
		Current_Pos += Move_Dir * Player_Move_Speed * elapsed_time;

		float Clamped_X = ClampFloat(XMVectorGetX(Current_Pos), -Limit_X, Limit_X);
		float Clamped_Y = ClampFloat(XMVectorGetY(Current_Pos), Limit_Y_Min, Limit_Y_Max);
		Current_Pos = XMVectorSet(Clamped_X, Clamped_Y, 0.0f, 0.0f);
	}
	else
	{
		// If Not Input, Move Safe Zone
		float Safe_X = Limit_X * Safe_Zone_Ratio;
		float Safe_Y_Min = Limit_Y_Min * Safe_Zone_Ratio;
		float Safe_Y_Max = Limit_Y_Max * Safe_Zone_Ratio;

		float Target_X = ClampFloat(XMVectorGetX(Current_Pos), -Safe_X, Safe_X);
		float Target_Y = ClampFloat(XMVectorGetY(Current_Pos), Safe_Y_Min, Safe_Y_Max);

		XMVECTOR Target_Pos = XMVectorSet(Target_X, Target_Y, 0.0f, 0.0f);
		Current_Pos = XMVectorLerp(Current_Pos, Target_Pos, Player_Tension_Speed * elapsed_time);
	}

	XMStoreFloat3(&Player_Pos, Current_Pos);

	Player_Update_Visual_State();
	Player_Update_Weapon_Logic(elapsed_time);
}

void Player_Reset() 
{
	// 1. Movement Reset
	Player_Pos	= FIRST_POS;
	Aim_Pos		= FIRST_POS;

	// 2. Resource Reset
}

void Player_Draw()
{
	int ID = Player_Update_Visual_Resources();

	if (ID != -1)
	{
		Billboard_Draw(ID, Player_Pos, Player_Size.x, Player_Size.y, { 0.5f, 0.5f });
	}
}

// ----------------------------------------------------------------------------------------------------------------
//											   Player Parameter Setter 
// ----------------------------------------------------------------------------------------------------------------
void Player_Set_POS(XMFLOAT3& POS)
{
	Player_Pos = POS;
}

void Player_Damaged(int damage)
{
	Player_HP -= damage;
	if (Player_HP < 0) Player_HP = 0;
}

// ----------------------------------------------------------------------------------------------------------------
//											    Player Parameter Geter 
// ----------------------------------------------------------------------------------------------------------------
const XMFLOAT3& Player_Get_POS()
{ 
	return Player_Pos; 
}

const XMFLOAT3& Player_Get_Aim_POS() 
{
	return Aim_Pos; 
}

float Player_Get_HP()
{
	return Player_HP;
}

float Player_Get_MaxHP()
{
	return Player_MaxHP;
}

// ----------------------------------------------------------------------------------------------------------------
//								        		 Player Debug Logic
// ----------------------------------------------------------------------------------------------------------------

void GUI_Set_Player_POS(float x, float y, float z)
{
	Player_Pos = { x, y, z };
}

void GUI_Set_Player_Health(float hp, float max)
{
	Player_HP = hp;
	Player_MaxHP = max;
}

void GUI_Set_Player_Stats(float atk, float def)
{
	Player_ATK = atk;
	Player_DEF = def;
}

// ----------------------------------------------------------------------------------------------------------------
//												Player Update Logic
//											  --- Movement System ---
// ----------------------------------------------------------------------------------------------------------------

// ----------------------------------------------------------------------------------------------------------------
//												--- Input ---
// ----------------------------------------------------------------------------------------------------------------
XMVECTOR Player_Update_Movement_Input()
{
	Is_Input_Moving = false;

	// Get Input Directions (Based on Camera Front)
	XMVECTOR Flat_Front = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f); // Map W/S to Y Axis
	XMVECTOR Flat_Right = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f); // Map A/D to X Axis
	XMVECTOR Input_Dir = XMVectorZero();

	// Get Keyboard, D-Pad Input

	if (M_INPUT->Is_Up_Pressed())
		Input_Dir += Flat_Front;
	if (M_INPUT->Is_Down_Pressed())
		Input_Dir -= Flat_Front;
	if (M_INPUT->Is_Left_Pressed())
		Input_Dir -= Flat_Right;
	if (M_INPUT->Is_Right_Pressed())
		Input_Dir += Flat_Right;

	// Get Left Stick Input
	XMFLOAT2 Stick = M_INPUT->Controller_Input_L_Stick();
	float Stick_Magnitude = sqrtf(Stick.x * Stick.x + Stick.y * Stick.y);

	if (Stick_Magnitude > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
	{
		XMVECTOR Direction = (Flat_Front * (Stick.y / 32767.0f)) + (Flat_Right * (Stick.x / 32767.0f));
		Input_Dir += Direction;
	}

	// Normalize Input Direction And Check If Moving
	XMVECTOR Length_SQ = XMVector3LengthSq(Input_Dir);
	float Sqrt = XMVectorGetX(Length_SQ);

	if (Sqrt > 0.0001f)
	{
		Is_Input_Moving = true;
		return XMVector3Normalize(Input_Dir);
	}

	return Input_Dir;
}

// ----------------------------------------------------------------------------------------------------------------
//												--- Visual ---
// ----------------------------------------------------------------------------------------------------------------
void Player_Texture()
{
	//---------------Bottom---------------//
	Player_Bot_L = Texture_Manager::GetInstance()->GetID("Player_Bottom_Left");
	Player_Bot_C = Texture_Manager::GetInstance()->GetID("Player_Bottom_Center");
	Player_Bot_R = Texture_Manager::GetInstance()->GetID("Player_Bottom_Right");

	//----------------Mid----------------//
	Player_Mid_L = Texture_Manager::GetInstance()->GetID("Player_Middle_Left");
	Player_Mid_C = Texture_Manager::GetInstance()->GetID("Player_Middle_Center");
	Player_Mid_R = Texture_Manager::GetInstance()->GetID("Player_Middle_Right");

	//----------------Top----------------//
	Player_Top_L = Texture_Manager::GetInstance()->GetID("Player_Top_Left");
	Player_Top_C = Texture_Manager::GetInstance()->GetID("Player_Top_Center");
	Player_Top_R = Texture_Manager::GetInstance()->GetID("Player_Top_Right");

    if (Player_Bot_C == -1 || Player_Bot_L == -1 || Player_Bot_R == -1
        || Player_Mid_C == -1 || Player_Top_C == -1 || Player_Top_L == -1 || Player_Top_R == -1)
    {
        Debug::D_Out << "[Player] Texture Init Error" << std::endl;
        Debug::D_Out << "Player Bottom Center : "   << Player_Bot_C << "\tPlayer Bottom Left : "     << Player_Bot_L
            << "\tPlayer Bottom Right : "			<< Player_Bot_R << "\tPlayer Miidle Center : "	<< Player_Mid_C
            << "\tPlayer Miidle Left : "			<< Player_Mid_L << "\tPlayer Miidle Right : "	<< Player_Mid_R
            << "\tPlayer Top Center : "				<< Player_Top_C << "\tPlayer Top Left : "		<< Player_Top_L
            << "\tPlayer Top Right : "				<< Player_Top_R << std::endl;
    }
}

void Player_Update_Visual_State()
{
	// Set Trigger POS
	float Safe_Y_Max = Get_Player_Limit_Y_Max() * Safe_Zone_Ratio;
	float Limit_X = Get_Player_Limit_X();

	// Set Threshold Y
	float Threshold_Y_High = Safe_Y_Max * 0.5f;
	float Threshold_Y_Mid = Safe_Y_Max * 0.25f; 

	// Set Threshold X
	float Threshold_X = Limit_X * 0.2f;

	// Check Y Axis, After Check X Axis
	if (Player_Pos.y >= Threshold_Y_High)
	{
		// Upper Axis
		if (Player_Pos.x <= -Threshold_X)      Visual_State = Player_Visual_State::Top_Left;
		else if (Player_Pos.x >= Threshold_X)  Visual_State = Player_Visual_State::Top_Right;
		else                                   Visual_State = Player_Visual_State::Top_Center;
	}
	else if (Player_Pos.y >= Threshold_Y_Mid)
	{
		// Middle Axis
		if (Player_Pos.x <= -Threshold_X)      Visual_State = Player_Visual_State::Mid_Left;
		else if (Player_Pos.x >= Threshold_X)  Visual_State = Player_Visual_State::Mid_Right;
		else                                   Visual_State = Player_Visual_State::Mid_Center;
	}
	else
	{
		// Bottom Axis
		if (Player_Pos.x <= -Threshold_X)      Visual_State = Player_Visual_State::Bot_Left;
		else if (Player_Pos.x >= Threshold_X)  Visual_State = Player_Visual_State::Bot_Right;
		else                                   Visual_State = Player_Visual_State::Bot_Center;
	}
}

int Player_Update_Visual_Resources()
{
	int Draw_ID = -1;

	switch (Visual_State)
	{
		// Bottom Axis
	case Player_Visual_State::Bot_Left:		Draw_ID = Player_Bot_L; break;
	case Player_Visual_State::Bot_Center:	Draw_ID = Player_Bot_C; break;
	case Player_Visual_State::Bot_Right:	Draw_ID = Player_Bot_R; break;

		// Middle Axis
	case Player_Visual_State::Mid_Left:		Draw_ID = Player_Mid_L; break;
	case Player_Visual_State::Mid_Center:	Draw_ID = Player_Mid_C; break;
	case Player_Visual_State::Mid_Right:	Draw_ID = Player_Mid_R; break;

		// Top Axis
	case Player_Visual_State::Top_Left:		Draw_ID = Player_Top_L; break;
	case Player_Visual_State::Top_Center:	Draw_ID = Player_Top_C; break;
	case Player_Visual_State::Top_Right:	Draw_ID = Player_Top_R; break;
	}

	return Draw_ID;
}

// ----------------------------------------------------------------------------------------------------------------
//												--- Combat ---
// ----------------------------------------------------------------------------------------------------------------
void Player_Update_Aim_Input(float dt)
{
	float Limit_X = Get_Player_Limit_X();
	float Limit_Y_Min = Get_Player_Limit_Y_Min();
	float Limit_Y_Max = Get_Player_Limit_Y_Max();

	// Gamepad R-Stick
	XMFLOAT2 Stick = M_INPUT->Controller_Input_R_Stick();
	float Stick_Mag = sqrtf(Stick.x * Stick.x + Stick.y * Stick.y);

	if (Stick_Mag > 8689.0f) // Gamepad R-Stick Aiming
	{
		Aim_Pos.x += (Stick.x / 32767.0f) * Aim_Move_Speed * dt;
		Aim_Pos.y += (Stick.y / 32767.0f) * Aim_Move_Speed * dt;

		float Pad_Limit_X = Get_Player_Limit_X() * 1.5f;
		Aim_Pos.x = ClampFloat(Aim_Pos.x, -Pad_Limit_X, Pad_Limit_X);
	}
	else // Mouse Aiming
	{
		XMFLOAT2 Mouse = M_INPUT->Mouse_Position();

		float w = static_cast<float>(Window_Manager::GetInstance()->GetWidth());
		float h = static_cast<float>(Window_Manager::GetInstance()->GetHeight());

		if (w > 0 && h > 0)
		{
			// Change 2D Pixel To 3D Ray
			XMMATRIX View = XMLoadFloat4x4(&Player_Camera_Get_View_Matrix());
			XMMATRIX Proj = XMLoadFloat4x4(&Player_Camera_Get_Proj_Matrix());
			XMMATRIX World = XMMatrixIdentity();

			XMVECTOR V_Near = XMVectorSet(static_cast<float>(Mouse.x), static_cast<float>(Mouse.y), 0.0f, 1.0f);
			XMVECTOR V_Far  = XMVectorSet(static_cast<float>(Mouse.x), static_cast<float>(Mouse.y), 1.0f, 1.0f);

			XMVECTOR V_Origin = XMVector3Unproject(V_Near, 0, 0, w, h, 0.0f, 1.0f, Proj, View, World);
			XMVECTOR V_Dest = XMVector3Unproject(V_Far, 0, 0, w, h, 0.0f, 1.0f, Proj, View, World);

			XMVECTOR V_Dir = XMVector3Normalize(V_Dest - V_Origin);

			if (abs(XMVectorGetZ(V_Dir)) > 0.0001f)
			{
				float T_Hit = -XMVectorGetZ(V_Origin) / XMVectorGetZ(V_Dir);
				XMVECTOR V_Hit = V_Origin + V_Dir * T_Hit;

				Aim_Pos.x = XMVectorGetX(V_Hit);
				Aim_Pos.y = XMVectorGetY(V_Hit);
				Aim_Pos.z = 0.0f; 
			}
		}
	}
}

void Player_Update_Weapon_Logic(float dt)
{
	// Weapon Change Request Logic
	if (M_INPUT->Is_Weapon_Changed())
	{
		EventManager::GetInstance().Fire(EventType::Player_Weapon_Change_Requested);
	}

	// 2. Fire Request Logic
	bool Is_Firing = M_INPUT->Is_Mouse_Left_Pressed() || (M_INPUT->Controlle_Right_Trigger() > XINPUT_GAMEPAD_TRIGGER_THRESHOLD);

	if (Is_Firing)
	{
		Combat_Fire_Request_Data fire_data(Player_Pos, Player_ATK);
		EventManager::GetInstance().Fire(EventType::Player_Fire_Requested, &fire_data);
	}
}
