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
#include "Sound_Register.h"
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

// Player Combat State
enum class PlayerState
{
	Normal,
	Dodging,
	Dead
};
static PlayerState P_State = PlayerState::Normal;

// Dodge resource
static float Dodge_Timer = 0.0f;
static DirectX::XMVECTOR Dodge_Dir = {};		// Direction For Dodge Movement
static constexpr float DODGE_DURATION = 1.0f;	// Move Duration For Dodge
static constexpr float DODGE_INVINCIBLE = 3.0f; // Invincible Duration For Dodge
static constexpr float DODGE_SPEED = 1.5f;

// Afterimage resource
struct AfterImageNode
{
	DirectX::XMFLOAT3 Pos;
	int TextureID;
	float LifeTime; // Image Life Time
};
static std::vector<AfterImageNode> m_AfterImages;
static constexpr float AFTERIMAGE_MAX_LIFE = 0.25f;  // Life Time For Afterimage
static constexpr float AFTERIMAGE_INTERVAL = 0.075f; // Interval For Afterimage Creation

// Overload resource
static int Overload_Count = 3;
static float Overload_Cooldown_Timer = 0.0f;
static constexpr float OVERLOAD_COOLDOWN = 5.0f;

// Player resource
static XMFLOAT2 Player_Size = { 2.0f, 1.0f };
static float Player_HP = 12.0f;    
static float Player_MaxHP = 12.0f; 
static float Player_ATK = 10.0f;

static bool Is_Input_Moving = false;

static float Invincible_Timer = 0.0f;
static constexpr float INVINCIBLE_TIME = 1.0f;
static float Death_Timer = 0.0f;
static constexpr float DEATH_COOLTIME = 3.0f;

static float Player_DeltaTime = 0.0f;

// ----------------------------------------------------------
//				static Player Update Logic
// ----------------------------------------------------------
// --- Movement And Physics System ---
static XMVECTOR Player_Update_Movement_Input();
static XMVECTOR Player_Movement_Update(float dt, XMVECTOR Current_Pos);

// --- Visual ---
static void Player_Texture();
static void Player_Update_Visual_State();
static int	Player_Update_Visual_Resources();
static void Draw_AfterImages();               

// --- Combat ---
static void Player_Update_Aim_Input(float dt);
static void Player_Update_Weapon_Logic(float dt);
static void Player_Update_Dodge_Logic(float dt);
static void Player_Update_Overload_Logic();

// ----------------------------------------------------------

void Player_Initialize()
{
	Player_Texture();

	Player_Pos	= FIRST_POS;
	Aim_Pos		= FIRST_POS;

	P_State = PlayerState::Normal;
	Player_HP = Player_MaxHP;

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
	// Update Delta Time
	Player_DeltaTime = elapsed_time;

	// If Player Is Dead, Wait For 3 Sec, For Debug
	if (P_State == PlayerState::Dead)
	{
		Death_Timer += elapsed_time;
		if (Death_Timer >= DEATH_COOLTIME)
		{
			EventManager::GetInstance().Fire(EventType::Go_From_In_Game_To_Main_Menu);
			P_State = PlayerState::Normal; // Safety Code
		}
		return;
	}

	// Update Invincible, Dodge Timer
	if (Invincible_Timer > 0.0f) Invincible_Timer -= elapsed_time;
	if (Dodge_Timer > 0.0f) Dodge_Timer -= elapsed_time;
	if (Overload_Cooldown_Timer > 0.0f) Overload_Cooldown_Timer -= elapsed_time;

	// ----------------------------------------------------------------
	//						Player Combat Update
	// ----------------------------------------------------------------
	
	// Aim Update
	XMVECTOR Target_Aim = XMLoadFloat3(&Aim_Pos);
	Player_Update_Aim_Input(elapsed_time);

	// Overload Update
	Player_Update_Overload_Logic();

	XMVECTOR Current_Pos = Player_Movement_Update(elapsed_time, XMLoadFloat3(&Player_Pos));

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
	// If Player Is Dead, Skip Draw
	if (P_State == PlayerState::Dead) return;

	// Draw Afterimages First
	Draw_AfterImages();

	int ID = Player_Update_Visual_Resources();

	if (ID != -1)
	{
		// If Player Is Invincible Or Dodging, Set Alpha To 0.75f
		XMFLOAT4 Color = Alpha_Origin;
		if (Invincible_Timer > 0.0f || Dodge_Timer > 0.0f) { Color = Alpha_T_Quarter; }

		Billboard_Draw(ID, Player_Pos, Player_Size.x, Player_Size.y, { 0.5f, 0.5f }, Color);
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
	// Skip Damage If Player Is Invincible, Dodging, Or Dead
	if (Invincible_Timer > 0.0f || Dodge_Timer > 0.0f || P_State == PlayerState::Dead) return;

	Player_HP -= damage;

	if (Player_HP <= 0)
	{
		Player_HP = 0;
		P_State = PlayerState::Dead;
		Death_Timer = 0.0f;

		// Effect
		Billboard_Manager::GetInstance().Create_Effect(Player_Pos, 3.0f, Effect_Type::EXPLOSION);
	}
	else
	{
		// When Damaged, Set Invincible Timer
		Invincible_Timer = INVINCIBLE_TIME;
	}
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

bool Player_Is_Dead()
{
	return false;
}

AABB Player_Get_AABB()
{
	float Radius_X = Player_Size.x * 0.5f;
	float Radius_Y = Player_Size.y * 0.5f;
	float Depth = 1.0f;

	return AABB
	{
		{ Player_Pos.x + Radius_X, Player_Pos.y + Radius_Y, Player_Pos.z + Depth },
		{ Player_Pos.x - Radius_X, Player_Pos.y - Radius_Y, Player_Pos.z - Depth }
	};
}

// ----------------------------------------------------------------------------------------------------------------
//													Combat System 
// ----------------------------------------------------------------------------------------------------------------

void Player_Change_Overload_Count(bool is_increase)
{
	if (is_increase)
	{
		Overload_Count++;
	}
	else if (Overload_Count > 0)
	{
		Overload_Count--;
	}
}

int Player_Get_Overload_Count()
{
	return Overload_Count;
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
	//Player_DEF = def;
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

XMVECTOR Player_Movement_Update(float dt, XMVECTOR Current_Pos)
{
	// Clamped Player Movement Limit
	float Limit_X = Get_Player_Limit_X();
	float Limit_Y_Min = Get_Player_Limit_Y_Min();
	float Limit_Y_Max = Get_Player_Limit_Y_Max();

	// Movement Update
	if (Dodge_Timer > (DODGE_INVINCIBLE - DODGE_DURATION))
	{
		// If Dodge, Do Not Update Input Movement, Only Do Forced Movement
		P_State = PlayerState::Dodging;
		Player_Update_Dodge_Logic(dt);
		Current_Pos = XMLoadFloat3(&Player_Pos);

		// If Dodge Forced Movement Done, Get Clamped POS
		float Clamped_X = ClampFloat(XMVectorGetX(Current_Pos), -Limit_X, Limit_X);
		float Clamped_Y = ClampFloat(XMVectorGetY(Current_Pos), Limit_Y_Min, Limit_Y_Max);
		Current_Pos = XMVectorSet(Clamped_X, Clamped_Y, 0.0f, 0.0f);
	}
	else
	{
		// If Not Dodge, Update Input Movement
		P_State = PlayerState::Normal;

		// Get Input Movement Direction
		XMVECTOR Move_Dir = Player_Update_Movement_Input();

		// Check Dodge Input
		if (M_INPUT->Is_Player_Dodged() && Dodge_Timer <= 0.0f)
		{
			Dodge_Timer = DODGE_INVINCIBLE;
			// If Have Input Movement, Dodge In That Direction
			// If Not, Move No Where
			Dodge_Dir = Is_Input_Moving ? Move_Dir : XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

			Sound_SFX_Event_Data sfx_data(Sound_SFX_Tag::Player_Dodge);
			EventManager::GetInstance().Fire(EventType::Play_Audio_SFX, &sfx_data);

			return Current_Pos;
		}

		if (Is_Input_Moving)
		{
			// If Input, Move In Limit
			Current_Pos += Move_Dir * Player_Move_Speed * dt;

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
			Current_Pos = XMVectorLerp(Current_Pos, Target_Pos, Player_Tension_Speed * dt);
		}
	}

	return Current_Pos;
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

void Draw_AfterImages()
{
	float dt = Player_DeltaTime;

	auto it = m_AfterImages.begin();
	while (it != m_AfterImages.end())
	{
		it->LifeTime -= dt;
		if (it->LifeTime <= 0.0f)
		{
			it = m_AfterImages.erase(it);
		}
		else
		{
			// Mapping LifeTime to Alpha
			float alpha = 0.25f + (0.25f * (it->LifeTime / AFTERIMAGE_MAX_LIFE));
			XMFLOAT4 color = { 1.0f, 1.0f, 1.0f, alpha };

			Billboard_Draw(it->TextureID, it->Pos, Player_Size.x, Player_Size.y, { 0.5f, 0.5f }, color);
			++it;
		}
	}
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

void Player_Update_Dodge_Logic(float dt)
{
	// Move Player To Dodge Direction
	XMVECTOR Current_Pos = XMLoadFloat3(&Player_Pos);
	Current_Pos += Dodge_Dir * (Player_Move_Speed * DODGE_SPEED) * dt;
	XMStoreFloat3(&Player_Pos, Current_Pos);

	// Make Afterimage
	static float trail_timer = 0.0f;
	trail_timer += dt;
	if (trail_timer >= AFTERIMAGE_INTERVAL) // Create Every 0.05 Seconds
	{
		trail_timer = 0.0f;
		AfterImageNode node;
		node.Pos = Player_Pos;
		node.TextureID = Player_Update_Visual_Resources();
		node.LifeTime = AFTERIMAGE_MAX_LIFE;
		m_AfterImages.push_back(node);
	}
}

void Player_Update_Overload_Logic()
{
	if (M_INPUT->Is_Player_Overload() && Overload_Count > 0 && Overload_Cooldown_Timer <= 0.0f)
	{
		Overload_Count--;
		Overload_Cooldown_Timer = OVERLOAD_COOLDOWN;

		EventManager::GetInstance().Fire(EventType::Player_Overload_Triggered);

		Sound_SFX_Event_Data sfx_data(Sound_SFX_Tag::Player_Overload);
		EventManager::GetInstance().Fire(EventType::Play_Audio_SFX, &sfx_data);
	}
}
