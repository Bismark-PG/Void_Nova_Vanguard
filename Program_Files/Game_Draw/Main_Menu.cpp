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

//----------------UI Texture----------------//
static int Main_BG = -1;
static int Main_Title = -1;
static int UI_Start[2] = {-1, -1}, UI_Set[2] = {-1, -1}, UI_Ranking[2] = { -1, -1 },  UI_Exit[2] = { -1, -1 };

//----------------------POS----------------------/
static float BG_W, BG_H;
static float Title_X, Title_Y, Title_W, Title_H;

static float UI_H;

static float Start_W, Start_X, Start_Y;
static float Set_W, Set_X, Set_Y;
static float Rank_W, Rank_X, Rank_Y;
static float Exit_W, Exit_X, Exit_Y;

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

//-----------------Main Logic-----------------//
void Main_Menu_Initialize()
{
	Main_Menu_Texture();
	M_INPUT->Set_Mouse_Visible_Mode(false);

	BG_W = static_cast<float>(Direct3D_GetBackBufferWidth());
	BG_H = static_cast<float>(Direct3D_GetBackBufferHeight());

	Title_H = BG_H * 0.25f;
	Title_W = Texture_Manager::GetInstance()->Get_Proportional_Width(Main_Title, Title_H);
	Title_X = BG_W * 0.1f;
	Title_Y = BG_H * 0.175f;

	UI_H = BG_H * 0.05f;
	float Base_X = BG_W * 0.85f;

	// Start Menu
	Start_W = Texture_Manager::GetInstance()->
		Get_Proportional_Width(UI_Start[static_cast<int>(Menu_State::Wait)], UI_H);
	Start_X = Base_X - (Start_W * 0.5f);
	Start_Y = (BG_H * 0.675f) - (UI_H * 0.5f);

	// Setting Menu
	Set_W = Texture_Manager::GetInstance()->
		Get_Proportional_Width(UI_Set[static_cast<int>(Menu_State::Wait)], UI_H);
	Set_X = Base_X - (Set_W * 0.5f);
	Set_Y = (BG_H * 0.75f) - (UI_H * 0.5f);

	// Ranking Menu
	Rank_W = Texture_Manager::GetInstance()->
		Get_Proportional_Width(UI_Ranking[static_cast<int>(Menu_State::Wait)], UI_H);
	Rank_X = Base_X - (Rank_W * 0.5f);
	Rank_Y = (BG_H * 0.825f) - (UI_H * 0.5f);

	// Exit Menu
	Exit_W = Texture_Manager::GetInstance()->
		Get_Proportional_Width(UI_Exit[static_cast<int>(Menu_State::Wait)], UI_H);
	Exit_X = Base_X - (Exit_W * 0.5f);
	Exit_Y = (BG_H * 0.9f) - (UI_H * 0.5f);

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
		if (M_INPUT->Is_Mouse_In_Rect(Start_X, Start_Y, Start_W, UI_H))
			Target = Main_Select_Buffer::Start;
		else if (M_INPUT->Is_Mouse_In_Rect(Set_X, Set_Y, Set_W, UI_H))
			Target = Main_Select_Buffer::Setting;
		else if (M_INPUT->Is_Mouse_In_Rect(Rank_X, Rank_Y, Rank_W, UI_H))
			Target = Main_Select_Buffer::Ranking;
		else if (M_INPUT->Is_Mouse_In_Rect(Exit_X, Exit_Y, Exit_W, UI_H))
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
	Sprite_Draw(Main_BG, A_Zero, A_Zero, BG_W, BG_H);
	Sprite_Draw(Main_Title, Title_X, Title_Y, Title_W, Title_H);
}

void Main_Menu_UI_Draw()
{
	int State_Wait = static_cast<int>(Menu_State::Wait);
	int State_Glow = static_cast<int>(Menu_State::Glow);

	Sprite_Draw((Get_Main_Menu_Buffer() == Main_Select_Buffer::Start) ? UI_Start[State_Glow] : UI_Start[State_Wait],
		Start_X, Start_Y, Start_W, UI_H, A_Zero);
	
	Sprite_Draw((Get_Main_Menu_Buffer() == Main_Select_Buffer::Setting) ? UI_Set[State_Glow] : UI_Set[State_Wait],
		Set_X, Set_Y, Set_W, UI_H, A_Zero);

	Sprite_Draw((Get_Main_Menu_Buffer() == Main_Select_Buffer::Ranking) ? UI_Ranking[State_Glow] : UI_Ranking[State_Wait],
		Rank_X, Rank_Y, Rank_W, UI_H, A_Zero);

	Sprite_Draw((Get_Main_Menu_Buffer() == Main_Select_Buffer::Exit) ? UI_Exit[State_Glow] : UI_Exit[State_Wait],
		Exit_X, Exit_Y, Exit_W, UI_H, A_Zero);
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

void Main_Menu_Texture()
{
	int State_Wait = static_cast<int>(Menu_State::Wait);
	int State_Glow = static_cast<int>(Menu_State::Glow);

	//---------------Main Menu Texture---------------//
	Main_BG		= Texture_Manager::GetInstance()->GetID("K");
	Main_Title  = Texture_Manager::GetInstance()->GetID("Title");

	UI_Start	[State_Wait]	= Texture_Manager::GetInstance()->GetID("Start_N");
	UI_Set		[State_Wait]	= Texture_Manager::GetInstance()->GetID("Settings_N");
	UI_Ranking	[State_Wait]	= Texture_Manager::GetInstance()->GetID("Ranking_N");
	UI_Exit		[State_Wait]	= Texture_Manager::GetInstance()->GetID("Exit_N");

	UI_Start	[State_Glow]	= Texture_Manager::GetInstance()->GetID("Start_G");
	UI_Set		[State_Glow]	= Texture_Manager::GetInstance()->GetID("Settings_G");
	UI_Ranking	[State_Glow]	= Texture_Manager::GetInstance()->GetID("Ranking_G");
	UI_Exit		[State_Glow]	= Texture_Manager::GetInstance()->GetID("Exit_G");

	if (Main_BG == -1 || Main_Title == -1
		|| UI_Start[State_Wait] == -1 || UI_Set[State_Wait] == -1 || UI_Ranking[State_Wait] == -1 || UI_Exit[State_Wait] == -1
		|| UI_Start[State_Glow] == -1 || UI_Set[State_Glow] == -1 || UI_Ranking[State_Glow] == -1 || UI_Exit[State_Glow] == -1)
	{
		Debug::D_Out << "[Main Menu] Texture Init Error" << std::endl;
		Debug::D_Out << "\tMain_BG : " << Main_BG 
			<< "\tMain_Title : " << Main_Title 
			<< "\tUI_Start : " << UI_Start 
			<< "\tUI_Set : " << UI_Set 
			<< "\tUI_Exit : " << UI_Exit << std::endl;
	}
}
