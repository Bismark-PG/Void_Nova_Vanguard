/*==============================================================================

	Game Setting [Setting.cpp]

	Author : Choi HyungJoon

==============================================================================*/
#include "Project_Header.h"
#include "Setting.h"

// Assential Logic
#include "Fade.h"
#include "Input_Manager.h"
#include "Event_Manager.h"
 
// Other
#include "Shader_Manager.h"
#include "Sound_Register.h"
#include "Audio_Manager.h"

using namespace DirectX;
using namespace PALETTE;

//----------------UI Texture----------------//
static int Setting_BG = -1;

static int UI_BGM = -1;
static int UI_SFX = -1;
static int UI_WinMode = -1;
static int UI_FullMode = -1;
static int UI_Back = -1;

static int UI_Num[11];
static int UI_Arrow_L = -1;
static int UI_Arrow_R = -1;

//----------------------POS----------------------//
static float ScreenW = 0.0f;
static float ScreenH = 0.0f;

static float Panel_X = 0.0f, Panel_Y = 0.0f;
static float Panel_W = 0.0f, Panel_H = 0.0f;

static float Sound_Menu_W = 0.0f, Mode_Menu_W = 0.0f, Menu_H = 0.0f;

static float BGM_X = 0.0f, BGM_Y = 0.0f;
static float SFX_X = 0.0f, SFX_Y = 0.0f;
static float Win_X = 0.0f, Win_Y = 0.0f;
static float Full_X = 0.0f, Full_Y = 0.0f;
static float Back_X = 0.0f, Back_Y = 0.0f;

static float Num_W = 0.0f, Num_H = 0.0f;
static float BGM_Num_X = 0.0f, BGM_Num_Y = 0.0f;
static float SFX_Num_X = 0.0f, SFX_Num_Y = 0.0f;
static float BGM_L_X = 0.0f, BGM_R_X = 0.0f;
static float SFX_L_X = 0.0f, SFX_R_X = 0.0f;

//----------------State & Data----------------//
static Setting_Select_Buffer S_Buffer = Setting_Select_Buffer::None;
static Setting_Edit_State	 E_State = Setting_Edit_State::None;

static int BGM_Volume = 5;
static int SFX_Volume = 5;

//---------------Private Logic---------------//
void Setting_Texture();
void Draw_Menu();
void Draw_Edit_Menu();

//-----------------Main Logic-----------------//
void Setting_Initialize()
{
	Setting_Texture();

	ScreenW = static_cast<float>(Direct3D_GetBackBufferWidth());
	ScreenH = static_cast<float>(Direct3D_GetBackBufferHeight());

	// 1. Panel
	Panel_W = ScreenW * A_Point_Nine;
	Panel_H = ScreenH * A_Point_Nine;
	Panel_X = ScreenW * A_Half - Panel_W * A_Half;
	Panel_Y = ScreenH * A_Half - Panel_H * A_Half;

	// 2. Menu Text
	Mode_Menu_W = Panel_W * A_Point_Three;
	Sound_Menu_W = Mode_Menu_W * A_Half;
	Menu_H = Panel_H * A_One_Tenth;

	// 3. X Ratio
	float Sound_Base_X	= (Panel_X + (Panel_W * A_Point_Three)) - (Sound_Menu_W * A_Half);
	float Mobe_Base_X	= (Panel_X + (Panel_W * A_Point_Three)) - (Mode_Menu_W * A_Half);
	float Back_Base_X	= Panel_X + (Panel_W * A_Half) - (Sound_Menu_W * A_Half);
	float Menu_Half		= Menu_H * A_Half;

	// 4. Menu POS
	BGM_X	= Sound_Base_X;
	BGM_Y	= Panel_Y + (Panel_H * A_Fifth) - Menu_Half;

	SFX_X	= Sound_Base_X;
	SFX_Y	= Panel_Y + (Panel_H * A_P_Three_Five) - Menu_Half;

	Win_X	= Mobe_Base_X;
	Win_Y	= Panel_Y + (Panel_H * A_Half) - Menu_Half;

	Full_X	= Mobe_Base_X;
	Full_Y	= Panel_Y + (Panel_H * A_Six_Five) - Menu_Half;

	Back_X	= Back_Base_X;
	Back_Y	= Panel_Y + (Panel_H * A_P_Eight_Five) - Menu_Half;

	// 5. Number Size, POS
	Num_W	= Sound_Menu_W * A_P_Three_Five;
	Num_H	= Menu_H * A_P_Eight_Five;

	float Num_Base_X = (Panel_X + (Panel_W * A_Point_Seven)) - (Num_W * A_Half);
	float Num_Y_Offset = (Num_H - Menu_H) * A_Half;

	BGM_Num_X = Num_Base_X;
	BGM_Num_Y = BGM_Y - Num_Y_Offset;

	SFX_Num_X = Num_Base_X;
	SFX_Num_Y = SFX_Y - Num_Y_Offset;

	// 6. Arrow POS
	float Arrow_Gap = Num_W * A_One_A_Quarter;
	BGM_L_X = BGM_Num_X - Arrow_Gap;
	BGM_R_X = BGM_Num_X + Arrow_Gap;
	SFX_L_X = SFX_Num_X - Arrow_Gap;
	SFX_R_X = SFX_Num_X + Arrow_Gap;

	// 7. Get Volume Info
	BGM_Volume = static_cast<int>(Audio_Manager::GetInstance()->Get_Target_BGM_Volume() * 10.0f);
	SFX_Volume = static_cast<int>(Audio_Manager::GetInstance()->Get_Target_SFX_Volume() * 10.0f);

	// 8. Reset Buffer & State
	S_Buffer = Setting_Select_Buffer::None;
	E_State = Setting_Edit_State::None;
}

void Setting_Finalize()
{

}

void Setting_Update(float elapsed_time)
{
	// Input Logic
	bool Mouse_Movement = M_INPUT->Is_Mouse_Moved();
	bool L_Click = M_INPUT->Is_Mouse_Left_Trigger(), R_Click = M_INPUT->Is_Mouse_Right_Trigger();

	bool Up_Key = M_INPUT->Is_Up_Trigger(), Down_Key = M_INPUT->Is_Down_Trigger();
	bool Left_Key = M_INPUT->Is_Left_Trigger(), Right_Key = M_INPUT->Is_Right_Trigger();

	bool Confirm_Key = M_INPUT->Is_Confirm_Trigger(), Cancel_Key = M_INPUT->Is_Cancel_Trigger() || R_Click;

	// For Debug
	if (M_INPUT->Debug_Cancel_Trigger())
	{
		Sound_SFX_Event_Data sfx_data(Sound_SFX_Tag::Buffer_Back);
		EventManager::GetInstance().Fire(EventType::Play_Audio_SFX, &sfx_data);

		Set_Setting_Edit_Buffer(Setting_Edit_State::None);
		Set_Setting_Menu_Buffer(Setting_Select_Buffer::None);
		EventManager::GetInstance().Fire(EventType::Close_Settings);
	}

	// Is Sound Edit Active, Do Not Allow Menu Change
	if (Is_Setting_Edit_Active())
	{
		bool Is_BGM = (E_State == Setting_Edit_State::BGM);
		int& Current_Vol = Is_BGM ? BGM_Volume : SFX_Volume;
		float L_X = Is_BGM ? BGM_L_X : SFX_L_X;
		float R_X = Is_BGM ? BGM_R_X : SFX_R_X;
		float Y_Pos = Is_BGM ? BGM_Num_Y : SFX_Num_Y;

		bool Vol_Changed = false;
		bool Exit_Edit = Cancel_Key;

		// Edit Exit Trigger
		if (M_INPUT->Is_Cancel_Trigger())
		{
			Exit_Edit = true;
		}

		// Sound Volume Change Trigger
		if (Left_Key)
		{
			Current_Vol--;
			Vol_Changed = true;
		}

		if (Right_Key) 
		{
			Current_Vol++; 
			Vol_Changed = true;
		}

		// Mouse Input Logic for Volume Change
		if (L_Click)
		{
			if (M_INPUT->Is_Mouse_In_Rect(L_X, Y_Pos, Num_W, Num_H))
			{
				Current_Vol--;
				Vol_Changed = true;
			}
			else if (M_INPUT->Is_Mouse_In_Rect(R_X, Y_Pos, Num_W, Num_H))
			{
				Current_Vol++;
				Vol_Changed = true;
			}
			else
			{
				Exit_Edit = true; // Out Of Rect Click, Exit Edit Mode
			}
		}
		else if (R_Click)
		{
			Exit_Edit = true; // Right Click, Exit Edit Mode
		}

		if (Vol_Changed)
		{
			Current_Vol = std::max(0, std::min(Current_Vol, 10)); // Clamp Volume
			float Vol_F = Current_Vol / 10.0f;

			Sound_SFX_Event_Data sfx_data(Sound_SFX_Tag::Buffer_Move);
			EventManager::GetInstance().Fire(EventType::Play_Audio_SFX, &sfx_data);

			Sound_Volume_Event_Data vol_data(Is_BGM, Vol_F);
			EventManager::GetInstance().Fire(EventType::Change_Audio_Volume, &vol_data);
		}

		if (Exit_Edit)
		{
			Sound_SFX_Event_Data sfx_data(Sound_SFX_Tag::Buffer_Select);
			EventManager::GetInstance().Fire(EventType::Play_Audio_SFX, &sfx_data);
			Set_Setting_Edit_Buffer(Setting_Edit_State::None);
		}
	}
	// Sellect Menu Change Logic
	else
	{
		if (Mouse_Movement)
		{
			Setting_Select_Buffer Current = Get_Setting_Menu_Buffer();
			Setting_Select_Buffer Target = Setting_Select_Buffer::Wait;

			// Check Menu Rect
			if (M_INPUT->Is_Mouse_In_Rect(BGM_X, BGM_Y, Sound_Menu_W, Menu_H))
				Target = Setting_Select_Buffer::BGM;
			else if (M_INPUT->Is_Mouse_In_Rect(SFX_X, SFX_Y, Sound_Menu_W, Menu_H))
				Target = Setting_Select_Buffer::SFX;
			else if (M_INPUT->Is_Mouse_In_Rect(Win_X, Win_Y, Mode_Menu_W, Menu_H))
				Target = Setting_Select_Buffer::WinMode;
			else if (M_INPUT->Is_Mouse_In_Rect(Full_X, Full_Y, Mode_Menu_W, Menu_H))
				Target = Setting_Select_Buffer::FullMode;
			else if (M_INPUT->Is_Mouse_In_Rect(Back_X, Back_Y, Sound_Menu_W, Menu_H))
				Target = Setting_Select_Buffer::Back;

			// If Mouse Moved, Change Buffer
			if (Current != Target)
			{
				Set_Setting_Menu_Buffer(Target);

				// Playu Sounf Effect When Mouse Moved To Menu
				if (Target != Setting_Select_Buffer::Wait)
				{
					Sound_SFX_Event_Data sfx_data(Sound_SFX_Tag::Buffer_Move);
					EventManager::GetInstance().Fire(EventType::Play_Audio_SFX, &sfx_data);
				}
			}
		}

		// Keyboard & Gamepad Input Logic
		if (Up_Key)
		{
			Setting_Select_Buffer Current = Get_Setting_Menu_Buffer();

			if (Current == Setting_Select_Buffer::None || Current == Setting_Select_Buffer::Wait)
				Current = Setting_Select_Buffer::BGM;
			else if (Current == Setting_Select_Buffer::SFX)
				Current = Setting_Select_Buffer::BGM;
			else if (Current == Setting_Select_Buffer::WinMode)
				Current = Setting_Select_Buffer::SFX;
			else if (Current == Setting_Select_Buffer::FullMode)
				Current = Setting_Select_Buffer::WinMode;
			else if (Current == Setting_Select_Buffer::Back)
				Current = Setting_Select_Buffer::FullMode;

			if (Current != Get_Setting_Menu_Buffer())
			{
				Set_Setting_Menu_Buffer(Current);

				Sound_SFX_Event_Data sfx_data(Sound_SFX_Tag::Buffer_Move);
				EventManager::GetInstance().Fire(EventType::Play_Audio_SFX, &sfx_data);
			}
		}
		else if (Down_Key)
		{
			Setting_Select_Buffer Current = Get_Setting_Menu_Buffer();

			if (Current == Setting_Select_Buffer::None || Current == Setting_Select_Buffer::Wait)
				Current = Setting_Select_Buffer::BGM;
			else if (Current == Setting_Select_Buffer::BGM)
				Current = Setting_Select_Buffer::SFX;
			else if (Current == Setting_Select_Buffer::SFX)
				Current = Setting_Select_Buffer::WinMode;
			else if (Current == Setting_Select_Buffer::WinMode)
				Current = Setting_Select_Buffer::FullMode;
			else if (Current == Setting_Select_Buffer::FullMode)
				Current = Setting_Select_Buffer::Back;

			if (Current != Get_Setting_Menu_Buffer())
			{
				Set_Setting_Menu_Buffer(Current);

				Sound_SFX_Event_Data sfx_data(Sound_SFX_Tag::Buffer_Move);
				EventManager::GetInstance().Fire(EventType::Play_Audio_SFX, &sfx_data);
			}
		}

		// Confirm Input Logic
		if (L_Click && Get_Setting_Menu_Buffer() != Setting_Select_Buffer::None && Get_Setting_Menu_Buffer() != Setting_Select_Buffer::Wait)
		{
			Confirm_Key = true;
		}

		if (Cancel_Key)
		{
			Sound_SFX_Event_Data sfx_data(Sound_SFX_Tag::Buffer_Back);
			EventManager::GetInstance().Fire(EventType::Play_Audio_SFX, &sfx_data);

			EventManager::GetInstance().Fire(EventType::Close_Settings);
			Set_Setting_Menu_Buffer(Setting_Select_Buffer::None);
			return;
		}

		if (Confirm_Key && Get_Setting_Menu_Buffer() != Setting_Select_Buffer::None && Get_Setting_Menu_Buffer() != Setting_Select_Buffer::Wait)
		{
			// Send Sound Data
			if (S_Buffer == Setting_Select_Buffer::Back)
			{
				Sound_SFX_Event_Data sfx_data(Sound_SFX_Tag::Buffer_Back);
				EventManager::GetInstance().Fire(EventType::Play_Audio_SFX, &sfx_data);
			}
			else
			{
				Sound_SFX_Event_Data sfx_data(Sound_SFX_Tag::Buffer_Select);
				EventManager::GetInstance().Fire(EventType::Play_Audio_SFX, &sfx_data);
			}

			// Check Setting Change Data
			switch (S_Buffer)
			{
			case Setting_Select_Buffer::BGM:
				Set_Setting_Edit_Buffer(Setting_Edit_State::BGM);
				break;
			case Setting_Select_Buffer::SFX:
				Set_Setting_Edit_Buffer(Setting_Edit_State::SFX);
				break;

			case Setting_Select_Buffer::WinMode:
				// Need Win Mode Logic
				break;

			case Setting_Select_Buffer::FullMode:
				// Need Full Mode Logic
				break;

			case Setting_Select_Buffer::Back:
				EventManager::GetInstance().Fire(EventType::Close_Settings);
				Set_Setting_Menu_Buffer(Setting_Select_Buffer::None);
				break;
			}
		}
	}
}

void Setting_Draw()
{
	Direct3D_SetDepthEnable(false);
	Shader_Manager::GetInstance()->Begin2D();

	if (Setting_BG == -1)	return;

	// Draw BG Panel
	Sprite_Draw(Setting_BG, Panel_X, Panel_Y, Panel_W, Panel_H);

	// Draw Menu
	Draw_Menu();
	Draw_Edit_Menu();
}

Setting_Select_Buffer Get_Setting_Menu_Buffer()
{
	return S_Buffer;
}

void Set_Setting_Menu_Buffer(Setting_Select_Buffer Buffer)
{
	S_Buffer = Buffer;
}

Setting_Edit_State Get_Setting_Edit_Buffer()
{
	return E_State;
}

void Set_Setting_Edit_Buffer(Setting_Edit_State Buffer)
{
	E_State = Buffer;

}

bool Is_Setting_Edit_Active()
{
	return E_State != Setting_Edit_State::None;
}

//---------------Private Logic---------------//
void Setting_Texture()
{
	//------------------BG Texture------------------//
	Setting_BG	= Texture_Manager::GetInstance()->GetID("Panel_BG");

	//------------------Menu Texture------------------//
	UI_BGM		= Texture_Manager::GetInstance()->GetID("Setting_BGM");
	UI_SFX		= Texture_Manager::GetInstance()->GetID("Setting_SFX");
	UI_WinMode	= Texture_Manager::GetInstance()->GetID("Setting_Win");
	UI_FullMode = Texture_Manager::GetInstance()->GetID("Setting_Full");
	UI_Back		= Texture_Manager::GetInstance()->GetID("Setting_Done");

	//------------------Number Texture------------------//
	UI_Num[0]	= Texture_Manager::GetInstance()->GetID("UI_Num_MIN");
	UI_Num[1]	= Texture_Manager::GetInstance()->GetID("UI_Num_1");
	UI_Num[2]	= Texture_Manager::GetInstance()->GetID("UI_Num_2");
	UI_Num[3]	= Texture_Manager::GetInstance()->GetID("UI_Num_3");
	UI_Num[4]	= Texture_Manager::GetInstance()->GetID("UI_Num_4");
	UI_Num[5]	= Texture_Manager::GetInstance()->GetID("UI_Num_5");
	UI_Num[6]	= Texture_Manager::GetInstance()->GetID("UI_Num_6");
	UI_Num[7]	= Texture_Manager::GetInstance()->GetID("UI_Num_7");
	UI_Num[8]	= Texture_Manager::GetInstance()->GetID("UI_Num_8");
	UI_Num[9]	= Texture_Manager::GetInstance()->GetID("UI_Num_9");
	UI_Num[10]	= Texture_Manager::GetInstance()->GetID("UI_Num_MAX");

	UI_Arrow_L	= Texture_Manager::GetInstance()->GetID("UI_Num_Button_L");
	UI_Arrow_R	= Texture_Manager::GetInstance()->GetID("UI_Num_Button_R");

	if (Setting_BG == -1 || UI_BGM == -1 || UI_SFX == -1 || UI_WinMode == -1 || UI_FullMode == -1 || UI_Back == -1 ||
		UI_Num[0] == -1 || UI_Num[1] == -1 || UI_Num[2] == -1 || UI_Num[3] == -1 || UI_Num[4] == -1 || UI_Num[5] == -1 ||
		UI_Num[6] == -1 || UI_Num[7] == -1 || UI_Num[8] == -1 || UI_Num[9] == -1 || UI_Num[10] == -1 ||
		UI_Arrow_L == -1 || UI_Arrow_R == -1)
	{
		Debug::D_Out << "[Setting] Texture Init Error" << std::endl;
		Debug::D_Out << "Setting_BG : "	<<	Setting_BG	<<	"\tUI_BGM : "	<< UI_BGM <<
			"\tUI_SFX : "			<< UI_SFX		<<	"\tUI_WinMode : "	<< UI_WinMode <<
			"\tUI_FullMode : "		<< UI_FullMode	<<	"\tUI_Back : "		<< UI_Back <<
			"\tUI_Arrow_L : "		<< UI_Arrow_L	<<	"\tUI_Arrow_R : "	<< UI_Arrow_R << std::endl;

		if (UI_Num[0] == -1 || UI_Num[1] == -1 || UI_Num[2] == -1 || UI_Num[3] == -1 || UI_Num[4] == -1 || UI_Num[5] == -1 ||
			UI_Num[6] == -1 || UI_Num[7] == -1 || UI_Num[8] == -1 || UI_Num[9] == -1 || UI_Num[10] == -1)
		{
			for (int i = 0; i < 11; ++i)
			{
				Debug::D_Out << "UI_Num[" << i << "] : " << UI_Num[i] << "\t" << std::endl;
			}
		}
	}
}

void Draw_Menu()
{
	Sprite_Draw(UI_BGM, BGM_X, BGM_Y, Sound_Menu_W, Menu_H, A_Zero,
		(Get_Setting_Menu_Buffer() == Setting_Select_Buffer::BGM) ? Alpha_Origin : Alpha_Half);

	Sprite_Draw(UI_SFX, SFX_X, SFX_Y, Sound_Menu_W, Menu_H, A_Zero,
		(Get_Setting_Menu_Buffer() == Setting_Select_Buffer::SFX) ? Alpha_Origin : Alpha_Half);

	Sprite_Draw(UI_WinMode, Win_X, Win_Y, Mode_Menu_W, Menu_H, A_Zero,
		(Get_Setting_Menu_Buffer() == Setting_Select_Buffer::WinMode) ? Alpha_Origin : Alpha_Half);

	Sprite_Draw(UI_FullMode, Full_X, Full_Y, Mode_Menu_W, Menu_H, A_Zero,
		(Get_Setting_Menu_Buffer() == Setting_Select_Buffer::FullMode) ? Alpha_Origin : Alpha_Half);

	Sprite_Draw(UI_Back, Back_X, Back_Y, Sound_Menu_W, Menu_H, A_Zero,
		(Get_Setting_Menu_Buffer() == Setting_Select_Buffer::Back) ? Alpha_Origin : Alpha_Half);
}

void Draw_Edit_Menu()
{
	XMFLOAT4 BGM_Alpha = (Get_Setting_Menu_Buffer() == Setting_Select_Buffer::BGM) ? Alpha_Origin : Alpha_Half;
	XMFLOAT4 SFX_Alpha = (Get_Setting_Menu_Buffer() == Setting_Select_Buffer::SFX) ? Alpha_Origin : Alpha_Half;

	Sprite_Draw(UI_Num[BGM_Volume], BGM_Num_X, BGM_Num_Y, Num_W, Num_H, A_Zero, BGM_Alpha);
	Sprite_Draw(UI_Num[SFX_Volume], SFX_Num_X, SFX_Num_Y, Num_W, Num_H, A_Zero, SFX_Alpha);

	// If Edit State Is Active, Draw Arrow
	if (E_State == Setting_Edit_State::BGM)
	{
		if (BGM_Volume > 0)
			Sprite_Draw(UI_Arrow_L, BGM_L_X, BGM_Num_Y, Num_W, Num_H, A_Zero, Alpha_Origin);

		if (BGM_Volume < 10)
			Sprite_Draw(UI_Arrow_R, BGM_R_X, BGM_Num_Y, Num_W, Num_H, A_Zero, Alpha_Origin);
	}
	else if (E_State == Setting_Edit_State::SFX)
	{
		if (SFX_Volume > 0)
			Sprite_Draw(UI_Arrow_L, SFX_L_X, SFX_Num_Y, Num_W, Num_H, A_Zero, Alpha_Origin);

		if (SFX_Volume < 10)
			Sprite_Draw(UI_Arrow_R, SFX_R_X, SFX_Num_Y, Num_W, Num_H, A_Zero, Alpha_Origin);
	}
}
