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

//---------------UI Info---------------//
struct Setting_UI_Data
{
	int Tex_ID = -1;
	float X = 0.0f, Y = 0.0f, W = 0.0f, H = 0.0f;
};

struct Setting_Edit_UI_Data
{
	float Num_X = 0.0f, Num_Y = 0.0f;
	float Arrow_L_X = 0.0f, Arrow_R_X = 0.0f;
};

struct Setting_Number_UI_Data
{
	int Tex_Num_ID[11] = {};
	int Tex_Arrow_L_ID = -1, Tex_Arrow_R_ID = -1;
	float W = 0.0f, H = 0.0f;
};

//---------------UI struct Info---------------//
static Setting_UI_Data BG_Panel, UI_BGM, UI_SFX, UI_WinMode, UI_FullMode, UI_Back;
static Setting_Edit_UI_Data Edit_BGM, Edit_SFX;
static Setting_Number_UI_Data UI_Num;
static float UI_Mode_W, UI_Sound_W, UI_H = 0.0f;

//----------------State & Data----------------//
static Setting_Select_Buffer S_Buffer = Setting_Select_Buffer::None;
static Setting_Edit_State	 E_State = Setting_Edit_State::None;

static int BGM_Volume = 5;
static int SFX_Volume = 5;

//---------------Private Logic---------------//
void Setting_Texture();
void Setting_UI_Initialize();
void Draw_Menu();
void Draw_Edit_Menu();

//-----------------Main Logic-----------------//
void Setting_Initialize()
{
	Setting_Texture();
	Setting_UI_Initialize();

	// Get Volume Info
	BGM_Volume = static_cast<int>(Audio_Manager::GetInstance()->Get_Target_BGM_Volume() * 10.0f);
	SFX_Volume = static_cast<int>(Audio_Manager::GetInstance()->Get_Target_SFX_Volume() * 10.0f);

	// Reset Buffer & State
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
		float L_X = Is_BGM ? Edit_BGM.Arrow_L_X : Edit_SFX.Arrow_L_X;
		float R_X = Is_BGM ? Edit_BGM.Arrow_R_X : Edit_SFX.Arrow_R_X;
		float Y_Pos = Is_BGM ? Edit_BGM.Num_Y : Edit_SFX.Num_Y;

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
			if (M_INPUT->Is_Mouse_In_Rect(L_X, Y_Pos, UI_Num.W, UI_Num.H))
			{
				Current_Vol--;
				Vol_Changed = true;
			}
			else if (M_INPUT->Is_Mouse_In_Rect(R_X, Y_Pos, UI_Num.W, UI_Num.H))
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
			if (M_INPUT->Is_Mouse_In_Rect(UI_BGM.X, UI_BGM.Y, UI_Sound_W, UI_H))
				Target = Setting_Select_Buffer::BGM;
			else if (M_INPUT->Is_Mouse_In_Rect(UI_SFX.X, UI_SFX.Y, UI_Sound_W, UI_H))
				Target = Setting_Select_Buffer::SFX;
			else if (M_INPUT->Is_Mouse_In_Rect(UI_WinMode.X, UI_WinMode.Y, UI_Mode_W, UI_H))
				Target = Setting_Select_Buffer::WinMode;
			else if (M_INPUT->Is_Mouse_In_Rect(UI_FullMode.X, UI_FullMode.Y, UI_Mode_W, UI_H))
				Target = Setting_Select_Buffer::FullMode;
			else if (M_INPUT->Is_Mouse_In_Rect(UI_Back.X, UI_Back.Y, UI_Sound_W, UI_H))
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

	if (BG_Panel.Tex_ID == -1)	return;

	// Draw BG Panel
	Sprite_Draw(BG_Panel.Tex_ID, BG_Panel.X, BG_Panel.Y, BG_Panel.W, BG_Panel.H);

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

// ----------------------------------------------------------------------------------------------------------------
//													Private Logic
// ----------------------------------------------------------------------------------------------------------------
void Setting_Texture()
{
	//------------------BG Texture------------------//
	BG_Panel.Tex_ID	= Texture_Manager::GetInstance()->GetID("Panel_BG");

	//------------------Menu Texture------------------//
	UI_BGM.Tex_ID		= Texture_Manager::GetInstance()->GetID("Setting_BGM");
	UI_SFX.Tex_ID		= Texture_Manager::GetInstance()->GetID("Setting_SFX");
	UI_WinMode.Tex_ID	= Texture_Manager::GetInstance()->GetID("Setting_Win");
	UI_FullMode.Tex_ID	= Texture_Manager::GetInstance()->GetID("Setting_Full");
	UI_Back.Tex_ID		= Texture_Manager::GetInstance()->GetID("Setting_Done");

	//------------------Number Texture------------------//
	UI_Num.Tex_Num_ID[0]	= Texture_Manager::GetInstance()->GetID("UI_Num_MIN");
	UI_Num.Tex_Num_ID[1]	= Texture_Manager::GetInstance()->GetID("UI_Num_1");
	UI_Num.Tex_Num_ID[2]	= Texture_Manager::GetInstance()->GetID("UI_Num_2");
	UI_Num.Tex_Num_ID[3]	= Texture_Manager::GetInstance()->GetID("UI_Num_3");
	UI_Num.Tex_Num_ID[4]	= Texture_Manager::GetInstance()->GetID("UI_Num_4");
	UI_Num.Tex_Num_ID[5]	= Texture_Manager::GetInstance()->GetID("UI_Num_5");
	UI_Num.Tex_Num_ID[6]	= Texture_Manager::GetInstance()->GetID("UI_Num_6");
	UI_Num.Tex_Num_ID[7]	= Texture_Manager::GetInstance()->GetID("UI_Num_7");
	UI_Num.Tex_Num_ID[8]	= Texture_Manager::GetInstance()->GetID("UI_Num_8");
	UI_Num.Tex_Num_ID[9]	= Texture_Manager::GetInstance()->GetID("UI_Num_9");
	UI_Num.Tex_Num_ID[10]	= Texture_Manager::GetInstance()->GetID("UI_Num_MAX");

	UI_Num.Tex_Arrow_L_ID	= Texture_Manager::GetInstance()->GetID("UI_Num_Button_L");
	UI_Num.Tex_Arrow_R_ID = Texture_Manager::GetInstance()->GetID("UI_Num_Button_R");

	if (BG_Panel.Tex_ID == -1 || UI_BGM.Tex_ID == -1 || UI_SFX.Tex_ID == -1 ||
		UI_WinMode.Tex_ID == -1 || UI_FullMode.Tex_ID == -1 || UI_Back.Tex_ID == -1 ||
		UI_Num.Tex_Arrow_L_ID == -1 || UI_Num.Tex_Arrow_R_ID == -1)
	{
		Debug::D_Out << "[Setting] Texture Init Error" << std::endl;
		Debug::D_Out << "BG_Panel : "	<<	BG_Panel.Tex_ID			<<	"\tUI_BGM : "		<< UI_BGM.Tex_ID <<
			"\tUI_SFX : "				<< UI_SFX.Tex_ID			<<	"\tUI_WinMode : "	<< UI_WinMode.Tex_ID <<
			"\tUI_FullMode : "			<< UI_FullMode.Tex_ID		<< "\tUI_Back : "		<< UI_Back.Tex_ID <<
			"\tUI_Arrow_L : "			<< UI_Num.Tex_Arrow_L_ID	<< "\tUI_Arrow_R : "	<< UI_Num.Tex_Arrow_R_ID << std::endl;\

		if (UI_Num.Tex_Num_ID[0] == -1 || UI_Num.Tex_Num_ID[1] == -1 || UI_Num.Tex_Num_ID[2] == -1 || UI_Num.Tex_Num_ID[3] == -1 || 
			UI_Num.Tex_Num_ID[4] == -1 || UI_Num.Tex_Num_ID[5] == -1 || UI_Num.Tex_Num_ID[6] == -1 || UI_Num.Tex_Num_ID[7] == -1 ||
			UI_Num.Tex_Num_ID[8] == -1 || UI_Num.Tex_Num_ID[9] == -1 || UI_Num.Tex_Num_ID[10] == -1)
		{
			for (int i = 0; i < 11; ++i)
			{
				Debug::D_Out << "UI_Num[" << i << "] : " << UI_Num.Tex_Num_ID[i] << "\t" << std::endl;
			}
		}
	}
}

void Setting_UI_Initialize()
{
	float ScreenW = static_cast<float>(Direct3D_GetBackBufferWidth());
	float ScreenH = static_cast<float>(Direct3D_GetBackBufferHeight());

	// 1. Panel
	BG_Panel.W = ScreenW * A_Point_Nine;
	BG_Panel.H = ScreenH * A_Point_Nine;
	BG_Panel.X = ScreenW * A_Half - BG_Panel.W * A_Half;
	BG_Panel.Y = ScreenH * A_Half - BG_Panel.H * A_Half;

	// 2. Menu Text
	UI_Mode_W = BG_Panel.W * A_Point_Three;
	UI_Sound_W = UI_Mode_W * A_Half;
	UI_H = BG_Panel.H * A_One_Tenth;

	// 3. X Ratio
	float Sound_Base_X = (BG_Panel.X + (BG_Panel.W * A_Point_Three)) - (UI_Sound_W * A_Half);
	float Mobe_Base_X = (BG_Panel.X + (BG_Panel.W * A_Point_Three)) - (UI_Mode_W * A_Half);
	float Back_Base_X = BG_Panel.X + (BG_Panel.W * A_Half) - (UI_Sound_W * A_Half);
	float Menu_Half = UI_H * A_Half;

	// 4. Menu POS
	UI_BGM.X = Sound_Base_X;
	UI_BGM.Y = BG_Panel.Y + (BG_Panel.H * A_Fifth) - Menu_Half;

	UI_SFX.X = Sound_Base_X;
	UI_SFX.Y = BG_Panel.Y + (BG_Panel.H * A_P_Three_Five) - Menu_Half;

	UI_WinMode.X = Mobe_Base_X;
	UI_WinMode.Y = BG_Panel.Y + (BG_Panel.H * A_Half) - Menu_Half;

	UI_FullMode.X = Mobe_Base_X;
	UI_FullMode.Y = BG_Panel.Y + (BG_Panel.H * A_Six_Five) - Menu_Half;

	UI_Back.X = Back_Base_X;
	UI_Back.Y = BG_Panel.Y + (BG_Panel.H * A_P_Eight_Five) - Menu_Half;

	// 5. Number Size, POS
	UI_Num.W = UI_Sound_W * A_P_Three_Five;
	UI_Num.H = UI_H * A_P_Eight_Five;

	float Num_Base_X = (BG_Panel.X + (BG_Panel.W * A_Point_Seven)) - (UI_Num.W * A_Half);
	float Num_Y_Offset = (UI_Num.H - UI_H) * A_Half;

	Edit_BGM.Num_X = Num_Base_X;
	Edit_BGM.Num_Y = UI_BGM.Y - Num_Y_Offset;

	Edit_SFX.Num_X = Num_Base_X;
	Edit_SFX.Num_Y = UI_SFX.Y - Num_Y_Offset;

	// 6. Arrow POS
	float Arrow_Gap = UI_Num.W * A_One_A_Quarter;
	Edit_BGM.Arrow_L_X = Edit_BGM.Num_X - Arrow_Gap;
	Edit_BGM.Arrow_R_X = Edit_BGM.Num_X + Arrow_Gap;
	Edit_SFX.Arrow_L_X = Edit_SFX.Num_X - Arrow_Gap;
	Edit_SFX.Arrow_R_X = Edit_SFX.Num_X + Arrow_Gap;
}

void Draw_Menu()
{
	Sprite_Draw(UI_BGM.Tex_ID, UI_BGM.X, UI_BGM.Y, UI_Sound_W, UI_H, A_Zero,
		(Get_Setting_Menu_Buffer() == Setting_Select_Buffer::BGM) ? Alpha_Origin : Alpha_Half);

	Sprite_Draw(UI_SFX.Tex_ID, UI_SFX.X, UI_SFX.Y, UI_Sound_W, UI_H, A_Zero,
		(Get_Setting_Menu_Buffer() == Setting_Select_Buffer::SFX) ? Alpha_Origin : Alpha_Half);

	Sprite_Draw(UI_WinMode.Tex_ID, UI_WinMode.X, UI_WinMode.Y, UI_Mode_W, UI_H, A_Zero,
		(Get_Setting_Menu_Buffer() == Setting_Select_Buffer::WinMode) ? Alpha_Origin : Alpha_Half);

	Sprite_Draw(UI_FullMode.Tex_ID, UI_FullMode.X, UI_FullMode.Y, UI_Mode_W, UI_H, A_Zero,
		(Get_Setting_Menu_Buffer() == Setting_Select_Buffer::FullMode) ? Alpha_Origin : Alpha_Half);

	Sprite_Draw(UI_Back.Tex_ID, UI_Back.X, UI_Back.Y, UI_Sound_W, UI_H, A_Zero,
		(Get_Setting_Menu_Buffer() == Setting_Select_Buffer::Back) ? Alpha_Origin : Alpha_Half);
}

void Draw_Edit_Menu()
{
	XMFLOAT4 BGM_Alpha = (Get_Setting_Menu_Buffer() == Setting_Select_Buffer::BGM) ? Alpha_Origin : Alpha_Half;
	XMFLOAT4 SFX_Alpha = (Get_Setting_Menu_Buffer() == Setting_Select_Buffer::SFX) ? Alpha_Origin : Alpha_Half;

	Sprite_Draw(UI_Num.Tex_Num_ID[BGM_Volume], Edit_BGM.Num_X, Edit_BGM.Num_Y, UI_Num.W, UI_Num.H, A_Zero, BGM_Alpha);
	Sprite_Draw(UI_Num.Tex_Num_ID[SFX_Volume], Edit_SFX.Num_X, Edit_SFX.Num_Y, UI_Num.W, UI_Num.H, A_Zero, SFX_Alpha);

	// If Edit State Is Active, Draw Arrow
	if (E_State == Setting_Edit_State::BGM)
	{
		if (BGM_Volume > 0)
			Sprite_Draw(UI_Num.Tex_Arrow_L_ID, Edit_BGM.Arrow_L_X, Edit_BGM.Num_Y, UI_Num.W, UI_Num.H, A_Zero, Alpha_Origin);

		if (BGM_Volume < 10)
			Sprite_Draw(UI_Num.Tex_Arrow_R_ID, Edit_BGM.Arrow_R_X, Edit_BGM.Num_Y, UI_Num.W, UI_Num.H, A_Zero, Alpha_Origin);
	}
	else if (E_State == Setting_Edit_State::SFX)
	{
		if (SFX_Volume > 0)
			Sprite_Draw(UI_Num.Tex_Arrow_L_ID, Edit_SFX.Arrow_L_X, Edit_SFX.Num_Y, UI_Num.W, UI_Num.H, A_Zero, Alpha_Origin);

		if (SFX_Volume < 10)
			Sprite_Draw(UI_Num.Tex_Arrow_R_ID, Edit_SFX.Arrow_R_X, Edit_SFX.Num_Y, UI_Num.W, UI_Num.H, A_Zero, Alpha_Origin);
	}
}
