/*==============================================================================

	Game Main Menu [Main_Menu.cpp]

	Author : Choi HyungJoon

==============================================================================*/
#include "Project_Header.h"
#include "Main_Menu.h"

// Assential Logic
#include "Fade.h"
#include "Input_Manager.h"
#include "Event_Manager.h"

// Other
#include "Sound_Register.h"
#include "Shader_Manager.h"
#include "Player_Camera.h"
#include "Light_Manager.h"

using namespace DirectX;

//---------------UI Info---------------//
struct Main_BG_Data
{
	int Tex_BG = -1;
	int Tex_Title = -1;
	float BG_W = 0.0f, BG_H = 0.0f;
	float Title_X = 0.0f, Title_Y = 0.0f, Title_W = 0.0f, Title_H = 0.0f;
};

struct Main_UI_Data
{
	int Tex_Wait = -1;
	int Tex_Glow = -1;
	float X = 0.0f, Y = 0.0f, W = 0.0f;
};

//---------------UI struct Info---------------//
static Main_BG_Data  BG_Info;
static Main_UI_Data UI_Start, UI_Set, UI_Rank, UI_Exit;
static float UI_H = 0.0f;

//----------------State & Data----------------//
// State Info
Main_Select_Buffer M_Buffer = Main_Select_Buffer::None;

// Sound Scale Info
static float Current_Volume = 0.0f;
static double BGM_Fade_Timer = 0.0;
constexpr double BGM_FADE_TIME = 3.0;
bool Is_BGM_Playing;
bool Is_BGM_Fading_In;

// Fade Info
static bool EXIT_STATE = false;
static constexpr double FADE_OUT_TIME = 2.0;
static double Fade_Out_Timer = 0.0;

// Bool Info
static bool Menu_Selected = false;
static bool Controller_Alert = false;
static bool Wait_For_Release = false;

//---------------Private Logic---------------//
void Main_Menu_Texture();
void Main_Menu_UI_Initialize();

//-----------------Main Logic-----------------//
void Main_Menu_Initialize()
{
	Main_Menu_Texture();
	M_INPUT->Set_Mouse_Visible_Mode(false);

	Main_Menu_UI_Initialize();
	Set_Main_Menu_Buffer(Main_Select_Buffer::None);
}

void Main_Menu_Finalize()
{
}

void Main_Menu_Update(float elapsed_time)
{
	// Input Logic
	bool Mouse_Movement = M_INPUT->Is_Mouse_Moved();
	bool L_Click = M_INPUT->Is_Mouse_Left_Trigger();
	bool Up_Key = M_INPUT->Is_Up_Trigger(), Down_Key = M_INPUT->Is_Down_Trigger();

	if (Mouse_Movement)
	{
		Main_Select_Buffer Current = Get_Main_Menu_Buffer();
		Main_Select_Buffer Target  = Main_Select_Buffer::Wait;

		// Check Menu Rect
		if (M_INPUT->Is_Mouse_In_Rect(UI_Start.X, UI_Start.Y, UI_Start.W, UI_H))
			Target = Main_Select_Buffer::Start;
		else if (M_INPUT->Is_Mouse_In_Rect(UI_Set.X, UI_Set.Y, UI_Set.W, UI_H))
			Target = Main_Select_Buffer::Setting;
		else if (M_INPUT->Is_Mouse_In_Rect(UI_Rank.X, UI_Rank.Y, UI_Rank.W, UI_H))
			Target = Main_Select_Buffer::Ranking;
		else if (M_INPUT->Is_Mouse_In_Rect(UI_Exit.X, UI_Exit.Y, UI_Exit.W, UI_H))
			Target = Main_Select_Buffer::Exit;

		// If Mouse Moved, Change Buffer
		if (Current != Target)
		{
			Set_Main_Menu_Buffer(Target);

			// Playu Sounf Effect When Mouse Moved To Menu
			if (Target != Main_Select_Buffer::Wait)
			{
				Sound_SFX_Event_Data sfx_data(Sound_SFX_Tag::Buffer_Move);
				EventManager::GetInstance().Fire(EventType::Play_Audio_SFX, &sfx_data);
			}
		}
	}

	// Keyboard & Gamepad Input Logic
	if (Up_Key)
	{
		Main_Select_Buffer Current = Get_Main_Menu_Buffer();

		if (Current == Main_Select_Buffer::None || Current == Main_Select_Buffer::Wait)
			Set_Main_Menu_Buffer(Main_Select_Buffer::Start);
		else if (Current == Main_Select_Buffer::Setting)
			Set_Main_Menu_Buffer(Main_Select_Buffer::Start);
		else if (Current == Main_Select_Buffer::Ranking)
			Set_Main_Menu_Buffer(Main_Select_Buffer::Setting); 
		else if (Current == Main_Select_Buffer::Exit)
			Set_Main_Menu_Buffer(Main_Select_Buffer::Ranking);

		if (Current != Get_Main_Menu_Buffer())
		{
			Sound_SFX_Event_Data sfx_data(Sound_SFX_Tag::Buffer_Move);
			EventManager::GetInstance().Fire(EventType::Play_Audio_SFX, &sfx_data);
		}
	}
	else if (Down_Key)
	{
		Main_Select_Buffer Current = Get_Main_Menu_Buffer();

		if (Current == Main_Select_Buffer::None || Current == Main_Select_Buffer::Wait)
			Set_Main_Menu_Buffer(Main_Select_Buffer::Start);
		else if (Current == Main_Select_Buffer::Start)
			Set_Main_Menu_Buffer(Main_Select_Buffer::Setting);
		else if (Current == Main_Select_Buffer::Setting)
			Set_Main_Menu_Buffer(Main_Select_Buffer::Ranking);
		else if (Current == Main_Select_Buffer::Ranking)
			Set_Main_Menu_Buffer(Main_Select_Buffer::Exit);

		if (Current != Get_Main_Menu_Buffer())
		{
			Sound_SFX_Event_Data sfx_data(Sound_SFX_Tag::Buffer_Move);
			EventManager::GetInstance().Fire(EventType::Play_Audio_SFX, &sfx_data);
		}
	}

	// Confirm Input Logic
	bool Confirm_Input = M_INPUT->Is_Confirm_Trigger();
	
	if (L_Click && Get_Main_Menu_Buffer() != Main_Select_Buffer::None && Get_Main_Menu_Buffer() != Main_Select_Buffer::Wait)
	{
		Confirm_Input = true;
	}

	if (Confirm_Input)
	{
		// Send Sound Data
		if (Get_Main_Menu_Buffer() == Main_Select_Buffer::Exit)
		{
			Sound_SFX_Event_Data sfx_data(Sound_SFX_Tag::Buffer_Back);
			EventManager::GetInstance().Fire(EventType::Play_Audio_SFX, &sfx_data);
		}
		else
		{
			Sound_SFX_Event_Data sfx_data(Sound_SFX_Tag::Buffer_Select);
			EventManager::GetInstance().Fire(EventType::Play_Audio_SFX, &sfx_data);
		}

		// Send Scene Change Data
		switch (Get_Main_Menu_Buffer())
		{
		case Main_Select_Buffer::Start:
			EventManager::GetInstance().Fire(EventType::Open_Stage_Select);
			Set_Main_Menu_Buffer(Main_Select_Buffer::None);
			break;

		case Main_Select_Buffer::Setting:
			EventManager::GetInstance().Fire(EventType::Open_Settings);
			Set_Main_Menu_Buffer(Main_Select_Buffer::None);
			break;

		case Main_Select_Buffer::Ranking:
			// Need Ranking Screen Implementation
			EventManager::GetInstance().Fire(EventType::Open_Ranking);
			Set_Main_Menu_Buffer(Main_Select_Buffer::None);
			break;

		case Main_Select_Buffer::Exit:
			EventManager::GetInstance().Fire(EventType::Exit_Game);
			Set_Main_Menu_Buffer(Main_Select_Buffer::Done);

			Fade_Start(FADE_OUT_TIME, true);
			EXIT_STATE = true;
			Fade_Out_Timer = 0.0;
			break;
		}
	}
}

void Main_Menu_Draw()
{
	Direct3D_SetDepthEnable(false);
	Shader_Manager::GetInstance()->Begin2D();

	Main_Menu_BG_Draw();
	Main_Menu_UI_Draw();
}

void Main_Menu_BG_Draw()
{
	Sprite_Draw(BG_Info.Tex_BG, A_Zero, A_Zero, BG_Info.BG_W, BG_Info.BG_H);
	Sprite_Draw(BG_Info.Tex_Title, BG_Info.Title_X, BG_Info.Title_Y, BG_Info.Title_W, BG_Info.Title_H);
}

void Main_Menu_UI_Draw()
{
	Sprite_Draw((Get_Main_Menu_Buffer() == Main_Select_Buffer::Start) ? UI_Start.Tex_Glow : UI_Start.Tex_Wait,
		UI_Start.X, UI_Start.Y, UI_Start.W, UI_H, A_Zero);
	
	Sprite_Draw((Get_Main_Menu_Buffer() == Main_Select_Buffer::Setting) ? UI_Set.Tex_Glow : UI_Set.Tex_Wait,
		UI_Set.X, UI_Set.Y, UI_Set.W, UI_H, A_Zero);

	Sprite_Draw((Get_Main_Menu_Buffer() == Main_Select_Buffer::Ranking) ? UI_Rank.Tex_Glow : UI_Rank.Tex_Wait,
		UI_Rank.X, UI_Rank.Y, UI_Rank.W, UI_H, A_Zero);

	Sprite_Draw((Get_Main_Menu_Buffer() == Main_Select_Buffer::Exit) ? UI_Exit.Tex_Glow : UI_Exit.Tex_Wait,
		UI_Exit.X, UI_Exit.Y, UI_Exit.W, UI_H, A_Zero);
}

Main_Select_Buffer Get_Main_Menu_Buffer()
{
	return M_Buffer;
}

void Set_Main_Menu_Buffer(Main_Select_Buffer Buffer)
{
	M_Buffer = Buffer;
}

bool IF_IS_Game_Done()
{
	return EXIT_STATE;
}

// ----------------------------------------------------------------------------------------------------------------
//													Private Logic
// ----------------------------------------------------------------------------------------------------------------
void Main_Menu_Texture()
{
	//---------------Main Menu Texture---------------//
	BG_Info.Tex_BG		= Texture_Manager::GetInstance()->GetID("K");
	BG_Info.Tex_Title	= Texture_Manager::GetInstance()->GetID("Title");

	UI_Start.Tex_Wait	= Texture_Manager::GetInstance()->GetID("Start_N");
	UI_Set.Tex_Wait		= Texture_Manager::GetInstance()->GetID("Settings_N");
	UI_Rank.Tex_Wait	= Texture_Manager::GetInstance()->GetID("Ranking_N");
	UI_Exit.Tex_Wait	= Texture_Manager::GetInstance()->GetID("Exit_N");

	UI_Start.Tex_Glow	= Texture_Manager::GetInstance()->GetID("Start_G");
	UI_Set.Tex_Glow		= Texture_Manager::GetInstance()->GetID("Settings_G");
	UI_Rank.Tex_Glow	= Texture_Manager::GetInstance()->GetID("Ranking_G");
	UI_Exit.Tex_Glow	= Texture_Manager::GetInstance()->GetID("Exit_G");

	if (BG_Info.Tex_BG == -1 || BG_Info.Tex_Title == -1
		|| UI_Start.Tex_Wait == -1 || UI_Set.Tex_Wait == -1 || UI_Rank.Tex_Wait == -1 || UI_Exit.Tex_Wait == -1
		|| UI_Start.Tex_Glow == -1 || UI_Set.Tex_Glow == -1 || UI_Rank.Tex_Glow == -1 || UI_Exit.Tex_Glow == -1)
	{
		Debug::D_Out << "[Main Menu] Texture Init Error" << std::endl;
		Debug::D_Out << "\tMain_BG : "	<< BG_Info.Tex_BG		<< "\tMain_Title : "	<< BG_Info.Tex_Title
			<< "\tUI_Start_Wait : "		<< UI_Start.Tex_Wait	<< "\tUI_Start_Glow : " << UI_Start.Tex_Glow
			<< "\tUI_Set_Wait : "		<< UI_Set.Tex_Wait		<< "\tUI_Set_Glow : "	<< UI_Set.Tex_Glow
			<< "\tUI_Rank_Wait : "		<< UI_Rank.Tex_Wait		<< "\tUI_Rank_Glow : "	<< UI_Rank.Tex_Glow
			<< "\tUI_Exit_Wait : "		<< UI_Exit.Tex_Wait		<< "\tUI_Exit_Glow : "	<< UI_Exit.Tex_Glow << std::endl;
	}
}

void Main_Menu_UI_Initialize()
{
	BG_Info.BG_W = static_cast<float>(Direct3D_GetBackBufferWidth());
	BG_Info.BG_H = static_cast<float>(Direct3D_GetBackBufferHeight());

	BG_Info.Title_H = BG_Info.BG_H * 0.25f;
	BG_Info.Title_W = Texture_Manager::GetInstance()->Get_Proportional_Width(BG_Info.Tex_Title, BG_Info.Title_H);
	BG_Info.Title_X = BG_Info.BG_W * 0.1f;
	BG_Info.Title_Y = BG_Info.BG_H * 0.175f;

	UI_H = BG_Info.BG_H * 0.05f;
	float Base_X = BG_Info.BG_W * 0.85f;

	// Start Menu
	UI_Start.W = Texture_Manager::GetInstance()->Get_Proportional_Width(UI_Start.Tex_Wait, UI_H);
	UI_Start.X = Base_X - (UI_Start.W * 0.5f);
	UI_Start.Y = (BG_Info.BG_H * 0.675f) - (UI_H * 0.5f);

	// Setting Menu
	UI_Set.W = Texture_Manager::GetInstance()->Get_Proportional_Width(UI_Set.Tex_Wait, UI_H);
	UI_Set.X = Base_X - (UI_Set.W * 0.5f);
	UI_Set.Y = (BG_Info.BG_H * 0.75f) - (UI_H * 0.5f);

	// Ranking Menu
	UI_Rank.W = Texture_Manager::GetInstance()->Get_Proportional_Width(UI_Rank.Tex_Wait, UI_H);
	UI_Rank.X = Base_X - (UI_Rank.W * 0.5f);
	UI_Rank.Y = (BG_Info.BG_H * 0.825f) - (UI_H * 0.5f);

	// Exit Menu
	UI_Exit.W = Texture_Manager::GetInstance()->Get_Proportional_Width(UI_Exit.Tex_Wait, UI_H);
	UI_Exit.X = Base_X - (UI_Exit.W * 0.5f);
	UI_Exit.Y = (BG_Info.BG_H * 0.9f) - (UI_H * 0.5f);
}