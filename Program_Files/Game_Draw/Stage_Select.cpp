/*==============================================================================

	Game Stage Select [Stage_Select.h]

	Author : Choi HyungJoon

==============================================================================*/
#include "Project_Header.h"
#include "Stage_Select.h"

// Assential Logic
#include "Fade.h"
#include "Event_Manager.h"
#include "Input_Manager.h"

// Other
#include "Sound_Register.h"
#include "Game_Flow_Register.h"
#include "Shader_Manager.h"
using namespace DirectX;

//----------------UI Texture----------------//
static int Panel_Select_BG = -1;

static int UI_Train = -1, UI_Launch = -1, UI_Select = -1, UI_Stage_Title = -1, UI_Back = -1;
static int UI_Num[10];

static int UI_Arrow_L = -1, UI_Arrow_R = -1;
//----------------------POS----------------------//
static float ScreenW = 0.0f, ScreenH = 0.0f;

// 1st Panel - Deploy
static float Panel_Deploy_X = 0.0f, Panel_Deploy_Y = 0.0f, Panel_Deploy_W = 0.0f, Panel_Deploy_H = 0.0f;
static float Deploy_Menu_W = 0.0f, Deploy_Menu_H = 0.0f, Deploy_Menu_X = 0.0f;
static float Tutor_Y = 0.0f, D_Start_Y = 0.0f, Select_Y = 0.0f, D_Back_Y = 0.0f;

// 2nd Panel - Stage Select
static float Panel_Sector_X = 0.0f, Panel_Sector_Y = 0.0f, Panel_Sector_W = 0.0f, Panel_Sector_H = 0.0f;
static float Title_X = 0.0f, Title_Y = 0.0f, Title_W = 0.0f, Title_H = 0.0f;
static float Num_W = 0.0f, Num_H = 0.0f;
static float Num_Tens_X = 0.0f, Num_Ones_X = 0.0f, Num_Y = 0.0f;
static float Arrow_L_X = 0.0f, Arrow_R_X = 0.0f;
static float Sector_Menu_W = 0.0f, Sector_Menu_H = 0.0f, Sector_Menu_Y = 0.0f;
static float S_Start_X = 0.0f, S_Back_X = 0.0f;

//----------------State & Data----------------//
static Deploy_Select_Buffer D_Buffer = Deploy_Select_Buffer::None;
static Stage_Select_Buffer	S_Buffer = Stage_Select_Buffer::None;
static Stage_Menu_State   Menu_State = Stage_Menu_State::Deploy_Menu;
static Stage_Select_State Select_State = Stage_Select_State::Stage_Number;

static int Current_Stage = 1;
static constexpr int MIN_STAGE = 1;
static constexpr int MAX_STAGE = 15;

//---------------Private Logic---------------//
void Stage_Select_Texture();

void Update_Deploy_Menu();
void Update_Stage_Select_Menu();

void Draw_Deploy_Menu();
void Draw_Stage_Select_Menu();

//-----------------Main Logic-----------------//
void Stage_Select_Initialize()
{
	Stage_Select_Texture();

	ScreenW = static_cast<float>(Direct3D_GetBackBufferWidth());
	ScreenH = static_cast<float>(Direct3D_GetBackBufferHeight());

	// 1-1. Deploy Menu Panel
	Panel_Deploy_W = ScreenW * 0.35f;
	Panel_Deploy_H = ScreenH * 0.5f;
	Panel_Deploy_X = (ScreenW * 0.75f) - (Panel_Deploy_W * 0.5f);
	Panel_Deploy_Y = (ScreenH * 0.3f) - (Panel_Deploy_H * 0.5f);

	// 1-2. Deploy Menu Size
	Deploy_Menu_W = Panel_Deploy_W * 0.65f;
	Deploy_Menu_H = Panel_Deploy_H * 0.125f;
	Deploy_Menu_X = Panel_Deploy_X + (Panel_Deploy_W * 0.5f) - (Deploy_Menu_W * 0.5f);

	// 1-3. Deploy Menu POS
	float Btn_Half_H = Deploy_Menu_H * 0.5f;
	Tutor_Y	  = Panel_Deploy_Y + (Panel_Deploy_H * 0.275f) - Btn_Half_H;
	D_Start_Y = Panel_Deploy_Y + (Panel_Deploy_H * 0.425f) - Btn_Half_H;
	Select_Y  = Panel_Deploy_Y + (Panel_Deploy_H * 0.575f) - Btn_Half_H;
	D_Back_Y  = Panel_Deploy_Y + (Panel_Deploy_H * 0.725f) - Btn_Half_H;

	// 2-1. Sector Menu Panel
	Panel_Sector_W = ScreenW * 0.8f;
	Panel_Sector_H = ScreenH * 0.75f;
	Panel_Sector_X = (ScreenW * 0.5f) - (Panel_Sector_W * 0.5f);
	Panel_Sector_Y = (ScreenH * 0.5f) - (Panel_Sector_H * 0.5f);

	// 2-2. Sector Menu Size
	Title_W = Panel_Sector_W * 0.75f;
	Title_H = Panel_Sector_H * 0.175f;
	Title_X = Panel_Sector_X + (Panel_Sector_W * 0.5f) - (Title_W * 0.5f);
	Title_Y = Panel_Sector_Y + (Panel_Sector_H * 0.2f) - (Title_H * 0.5f);

	// 2-3. Sector Menu Number Size
	Num_W = Panel_Sector_W * 0.075f;
	Num_H = Num_W * 1.5f;
	Num_Y = Panel_Sector_Y + (Panel_Sector_H * 0.5f) - (Num_H * 0.5f);

	// 2-4. Sector Menu Number, Arrow POS
	float P2_Center_X = Panel_Sector_X + (Panel_Sector_W * 0.5f);
	float Num_Gap = Num_W * 0.5f;

	Num_Tens_X = P2_Center_X - Num_W - (Num_Gap * 0.5f);
	Num_Ones_X = P2_Center_X + (Num_Gap * 0.5f);

	Arrow_L_X = Num_Tens_X - (Num_W * 1.5f);
	Arrow_R_X = Num_Ones_X + (Num_W * 1.5f);

	// 2-5. Sector Menu Size
	Sector_Menu_W = Panel_Sector_W * 0.253f;
	Sector_Menu_H = Panel_Sector_H * 0.15f;
	S_Start_X = Panel_Sector_X + (Panel_Sector_W * 0.3f) - (Sector_Menu_W * 0.5f);
	S_Back_X = Panel_Sector_X + (Panel_Sector_W * 0.7f) - (Sector_Menu_W * 0.5f);
	Sector_Menu_Y = Panel_Sector_Y + (Panel_Sector_H * 0.8f) - (Sector_Menu_H * 0.5f);

	// 3. Reset Buffer & State
	Set_Deploy_Menu_Buffer(Deploy_Select_Buffer::None);
	Set_Stage_Menu_Buffer(Stage_Select_Buffer::None);
	Set_Stage_Menu_State(Stage_Menu_State::Deploy_Menu);
	Set_Stage_Select_State(Stage_Select_State::Stage_Number);

	Current_Stage = 1;
}

void Stage_Select_Finalize()
{
}

void Stage_Select_Update(float elapsed_time)
{
	if (Get_Stage_Menu_State() == Stage_Menu_State::Deploy_Menu)
	{
		Update_Deploy_Menu();
	}
	else
	{
		Update_Stage_Select_Menu();
	}
}

void Stage_Select_Draw()
{
	Direct3D_SetDepthEnable(false);
	Shader_Manager::GetInstance()->Begin2D();

	if (Panel_Select_BG == -1) return;

	Draw_Deploy_Menu();

	if (Get_Stage_Menu_State() == Stage_Menu_State::Stage_Select)
	{
		Draw_Stage_Select_Menu();
	}
}

Deploy_Select_Buffer Get_Deploy_Menu_Buffer()
{
	return D_Buffer;
}

void Set_Deploy_Menu_Buffer(Deploy_Select_Buffer Buffer)
{
	D_Buffer = Buffer;
}

Stage_Select_Buffer Get_Stage_Menu_Buffer()
{
	return S_Buffer;
}

void Set_Stage_Menu_Buffer(Stage_Select_Buffer Buffer)
{
	S_Buffer = Buffer;
}

Stage_Menu_State Get_Stage_Menu_State()
{
	return Menu_State;
}

void Set_Stage_Menu_State(Stage_Menu_State State)
{
	Menu_State = State;
}

Stage_Select_State Get_Stage_Select_State()
{
	return Select_State;
}

void Set_Stage_Select_State(Stage_Select_State State)
{
	Select_State = State;
}

//---------------Private Logic---------------//
void Stage_Select_Texture()
{
	//------------------UI Texture------------------//
	Panel_Select_BG = Texture_Manager::GetInstance()->GetID("Panel_BG");

	UI_Train		= Texture_Manager::GetInstance()->GetID("Select_Tutorial");
	UI_Launch		= Texture_Manager::GetInstance()->GetID("Select_Start");
	UI_Select		= Texture_Manager::GetInstance()->GetID("Select_Stage");
	UI_Stage_Title	= Texture_Manager::GetInstance()->GetID("Select_Secter_Title");
	UI_Back			= Texture_Manager::GetInstance()->GetID("Select_Back");

	UI_Num[0] = Texture_Manager::GetInstance()->GetID("UI_Num_MIN");
	UI_Num[1] = Texture_Manager::GetInstance()->GetID("UI_Num_1");
	UI_Num[2] = Texture_Manager::GetInstance()->GetID("UI_Num_2");
	UI_Num[3] = Texture_Manager::GetInstance()->GetID("UI_Num_3");
	UI_Num[4] = Texture_Manager::GetInstance()->GetID("UI_Num_4");
	UI_Num[5] = Texture_Manager::GetInstance()->GetID("UI_Num_5");
	UI_Num[6] = Texture_Manager::GetInstance()->GetID("UI_Num_6");
	UI_Num[7] = Texture_Manager::GetInstance()->GetID("UI_Num_7");
	UI_Num[8] = Texture_Manager::GetInstance()->GetID("UI_Num_8");
	UI_Num[9] = Texture_Manager::GetInstance()->GetID("UI_Num_9");

	UI_Arrow_L = Texture_Manager::GetInstance()->GetID("UI_Num_Button_L");
	UI_Arrow_R = Texture_Manager::GetInstance()->GetID("UI_Num_Button_R");

	if (Panel_Select_BG == -1 || UI_Train == -1 || UI_Launch == -1 || UI_Select == -1 || UI_Stage_Title == -1 || UI_Back == -1 ||
		UI_Arrow_L == -1 || UI_Arrow_R == -1)
	{
		Debug::D_Out << "[Setting] Texture Init Error" << std::endl;
		Debug::D_Out << "Panel_Select_BG : " << Panel_Select_BG << "\tUI_Train : " << UI_Train <<
			"\tUI_Launch : "		<< UI_Launch		<< "\tUI_Select : " << UI_Select <<
			"\tUI_Stage_Title : "	<< UI_Stage_Title	<< "\tUI_Back : "	<< UI_Back <<
			"\tUI_Arrow_L : "		<< UI_Arrow_L		<< "\tUI_Arrow_R : " << UI_Arrow_R << std::endl;


		if (UI_Num[0] == -1 || UI_Num[1] == -1 || UI_Num[2] == -1 || UI_Num[3] == -1 || UI_Num[4] == -1 ||
			UI_Num[5] == -1 || UI_Num[6] == -1 || UI_Num[7] == -1 || UI_Num[8] == -1 || UI_Num[9] == -1)
		{
			for (int i = 0; i < 10; ++i)
			{
				Debug::D_Out << "UI_Num[" << i << "] : " << UI_Num[i] << "\t" << std::endl;
			}
		}
	}
}

void Update_Deploy_Menu()
{
	// Get Mouse Status
	bool Mouse_Movement = M_INPUT->Is_Mouse_Moved();
	bool L_Click = M_INPUT->Is_Mouse_Left_Trigger();

	bool Up_Key = M_INPUT->Is_Up_Trigger(), Down_Key = M_INPUT->Is_Down_Trigger();
	bool Left_Key = M_INPUT->Is_Left_Trigger(), Right_Key = M_INPUT->Is_Right_Trigger();

	bool Confirm_Key = M_INPUT->Is_Confirm_Trigger();
	bool Back_Key = M_INPUT->Is_Cancel_Trigger() || M_INPUT->Is_Mouse_Right_Trigger();

	//------------------Game Mode Select Menu------------------//
	if (Back_Key)
	{
		Sound_SFX_Event_Data sfx_data(Sound_SFX_Tag::Buffer_Back);
		EventManager::GetInstance().Fire(EventType::Play_Audio_SFX, &sfx_data);

		EventManager::GetInstance().Fire(EventType::Close_Stage_Select);
		return;
	}

	if (Mouse_Movement)
	{
		Deploy_Select_Buffer Target = Deploy_Select_Buffer::Wait;

		if (M_INPUT->Is_Mouse_In_Rect(Deploy_Menu_X, Tutor_Y, Deploy_Menu_W, Deploy_Menu_H))
			Target = Deploy_Select_Buffer::Training;
		else if (M_INPUT->Is_Mouse_In_Rect(Deploy_Menu_X, D_Start_Y, Deploy_Menu_W, Deploy_Menu_H))
			Target = Deploy_Select_Buffer::Launch;
		else if (M_INPUT->Is_Mouse_In_Rect(Deploy_Menu_X, Select_Y, Deploy_Menu_W, Deploy_Menu_H))
			Target = Deploy_Select_Buffer::Select;
		else if (M_INPUT->Is_Mouse_In_Rect(Deploy_Menu_X, D_Back_Y, Deploy_Menu_W, Deploy_Menu_H))
			Target = Deploy_Select_Buffer::Back;

		// If Mouse Moved, Change Buffer
		if (Get_Deploy_Menu_Buffer() != Target)
		{
			Set_Deploy_Menu_Buffer(Target);

			// Playu Sounf Effect When Mouse Moved To Menu
			if (Target != Deploy_Select_Buffer::Wait)
			{
				Sound_SFX_Event_Data sfx_data(Sound_SFX_Tag::Buffer_Move);
				EventManager::GetInstance().Fire(EventType::Play_Audio_SFX, &sfx_data);
			}
		}
	}

	if (Up_Key)
	{
		Deploy_Select_Buffer Current = Get_Deploy_Menu_Buffer();

		if (Current == Deploy_Select_Buffer::None || Current == Deploy_Select_Buffer::Wait)
			Current = Deploy_Select_Buffer::Training;
		else if (Current == Deploy_Select_Buffer::Launch)
			Current = Deploy_Select_Buffer::Training;
		else if (Current == Deploy_Select_Buffer::Select)
			Current = Deploy_Select_Buffer::Launch;
		else if (Current == Deploy_Select_Buffer::Back)
			Current = Deploy_Select_Buffer::Select;

		if (Get_Deploy_Menu_Buffer() != Current)
		{
			Set_Deploy_Menu_Buffer(Current);

			Sound_SFX_Event_Data sfx_data(Sound_SFX_Tag::Buffer_Move);
			EventManager::GetInstance().Fire(EventType::Play_Audio_SFX, &sfx_data);
		}
	}
	else if (Down_Key)
	{
		Deploy_Select_Buffer Current = Get_Deploy_Menu_Buffer();

		if (Current == Deploy_Select_Buffer::None || Current == Deploy_Select_Buffer::Wait)
			Current = Deploy_Select_Buffer::Training;
		else if (Current == Deploy_Select_Buffer::Training)
			Current = Deploy_Select_Buffer::Launch;
		else if (Current == Deploy_Select_Buffer::Launch)
			Current = Deploy_Select_Buffer::Select;
		else if (Current == Deploy_Select_Buffer::Select)
			Current = Deploy_Select_Buffer::Back;

		if (Get_Deploy_Menu_Buffer() != Current)
		{
			Set_Deploy_Menu_Buffer(Current);

			Sound_SFX_Event_Data sfx_data(Sound_SFX_Tag::Buffer_Move);
			EventManager::GetInstance().Fire(EventType::Play_Audio_SFX, &sfx_data);
		}
	}

	if (L_Click && Get_Deploy_Menu_Buffer() != Deploy_Select_Buffer::None && Get_Deploy_Menu_Buffer() != Deploy_Select_Buffer::Wait)
	{
		Confirm_Key = true;
	}

	if (Confirm_Key && Get_Deploy_Menu_Buffer() != Deploy_Select_Buffer::None && Get_Deploy_Menu_Buffer() != Deploy_Select_Buffer::Wait)
	{
		// Send Sound Data
		if (Get_Deploy_Menu_Buffer() == Deploy_Select_Buffer::Back)
		{
			Sound_SFX_Event_Data sfx_data(Sound_SFX_Tag::Buffer_Back);
			EventManager::GetInstance().Fire(EventType::Play_Audio_SFX, &sfx_data);
		}
		else
		{
			Sound_SFX_Event_Data sfx_data(Sound_SFX_Tag::Buffer_Select);
			EventManager::GetInstance().Fire(EventType::Play_Audio_SFX, &sfx_data);
		}

		// Check Deploy Select Data
		switch (Get_Deploy_Menu_Buffer())
		{
		case Deploy_Select_Buffer::Training:
			// Need Tutorial Logic
			break;

		case Deploy_Select_Buffer::Launch:
		{
			Current_Stage = 1; // Just Start, Play Stage 1

			// Caal Spawn-Event With Stage Number
			Stage_Event_Data stage_data(Current_Stage);
			EventManager::GetInstance().Fire(EventType::Go_From_Main_Menu_To_In_Game, &stage_data);
		}
		break;

		case Deploy_Select_Buffer::Select:
			Set_Stage_Menu_State(Stage_Menu_State::Stage_Select);
			Set_Stage_Menu_Buffer(Stage_Select_Buffer::None);
			break;

		case Deploy_Select_Buffer::Back:
			EventManager::GetInstance().Fire(EventType::Close_Stage_Select);
			break;
		}
	}
}

void Update_Stage_Select_Menu()
{
	// Get Mouse Status
	bool Mouse_Movement = M_INPUT->Is_Mouse_Moved();
	bool L_Click = M_INPUT->Is_Mouse_Left_Trigger();

	bool Up_Key = M_INPUT->Is_Up_Trigger(), Down_Key = M_INPUT->Is_Down_Trigger();
	bool Left_Key = M_INPUT->Is_Left_Trigger(), Right_Key = M_INPUT->Is_Right_Trigger();

	bool Confirm_Key = M_INPUT->Is_Confirm_Trigger();
	bool Back_Key = M_INPUT->Is_Cancel_Trigger() || M_INPUT->Is_Mouse_Right_Trigger();

	//--------------------Stage Select Menu--------------------//
	// M_INPUT Back, Back Deploy Menu
	if (Back_Key)
	{
		Sound_SFX_Event_Data sfx_data(Sound_SFX_Tag::Buffer_Back);
		EventManager::GetInstance().Fire(EventType::Play_Audio_SFX, &sfx_data);

		Set_Stage_Menu_State(Stage_Menu_State::Deploy_Menu);
		return;
	}

	if (Mouse_Movement)
	{
		if (M_INPUT->Is_Mouse_In_Rect(Arrow_L_X, Num_Y, Num_W, Num_H) ||
			M_INPUT->Is_Mouse_In_Rect(Arrow_R_X, Num_Y, Num_W, Num_H) ||
			M_INPUT->Is_Mouse_In_Rect(Num_Tens_X, Num_Y, Num_W * 2.5f, Num_H))
		{
			if (Get_Stage_Select_State() != Stage_Select_State::Stage_Number)
			{
				Set_Stage_Select_State(Stage_Select_State::Stage_Number);
				Sound_SFX_Event_Data sfx_data(Sound_SFX_Tag::Buffer_Move);
				EventManager::GetInstance().Fire(EventType::Play_Audio_SFX, &sfx_data);
			}
		}
		else if (M_INPUT->Is_Mouse_In_Rect(S_Start_X, Sector_Menu_Y, Sector_Menu_W, Sector_Menu_H))
		{
			if (Get_Stage_Select_State() != Stage_Select_State::Bottom_Menu || Get_Stage_Menu_Buffer() != Stage_Select_Buffer::Launch)
			{
				Set_Stage_Select_State(Stage_Select_State::Bottom_Menu);
				Set_Stage_Menu_Buffer(Stage_Select_Buffer::Launch);
				Sound_SFX_Event_Data sfx_data(Sound_SFX_Tag::Buffer_Move);
				EventManager::GetInstance().Fire(EventType::Play_Audio_SFX, &sfx_data);
			}
		}
		else if (M_INPUT->Is_Mouse_In_Rect(S_Back_X, Sector_Menu_Y, Sector_Menu_W, Sector_Menu_H))
		{
			if (Get_Stage_Select_State() != Stage_Select_State::Bottom_Menu || Get_Stage_Menu_Buffer() != Stage_Select_Buffer::Back)
			{
				Set_Stage_Select_State(Stage_Select_State::Bottom_Menu);
				Set_Stage_Menu_Buffer(Stage_Select_Buffer::Back);
				Sound_SFX_Event_Data sfx_data(Sound_SFX_Tag::Buffer_Move);
				EventManager::GetInstance().Fire(EventType::Play_Audio_SFX, &sfx_data);
			}
		}
	}

	if (Get_Stage_Select_State() == Stage_Select_State::Stage_Number)
	{
		if (Left_Key && Current_Stage > MIN_STAGE)
		{
			Current_Stage--;
			Sound_SFX_Event_Data sfx_data(Sound_SFX_Tag::Buffer_Move);
			EventManager::GetInstance().Fire(EventType::Play_Audio_SFX, &sfx_data);
		}
		else if (Right_Key && Current_Stage < MAX_STAGE)
		{
			Current_Stage++;
			Sound_SFX_Event_Data sfx_data(Sound_SFX_Tag::Buffer_Move);
			EventManager::GetInstance().Fire(EventType::Play_Audio_SFX, &sfx_data);
		}
		else if (Down_Key)
		{
			Set_Stage_Select_State(Stage_Select_State::Bottom_Menu);
			if (Get_Stage_Menu_Buffer() == Stage_Select_Buffer::None || Get_Stage_Menu_Buffer() == Stage_Select_Buffer::Wait)
				Set_Stage_Menu_Buffer(Stage_Select_Buffer::Launch);

			Sound_SFX_Event_Data sfx_data(Sound_SFX_Tag::Buffer_Move);
			EventManager::GetInstance().Fire(EventType::Play_Audio_SFX, &sfx_data);
		}

		// Menu Button Click
		if (L_Click)
		{
			if (Current_Stage > MIN_STAGE && M_INPUT->Is_Mouse_In_Rect(Arrow_L_X, Num_Y, Num_W, Num_H))
			{
				Current_Stage--;
				Sound_SFX_Event_Data sfx_data(Sound_SFX_Tag::Buffer_Move);
				EventManager::GetInstance().Fire(EventType::Play_Audio_SFX, &sfx_data);
			}
			else if (Current_Stage < MAX_STAGE && M_INPUT->Is_Mouse_In_Rect(Arrow_R_X, Num_Y, Num_W, Num_H))
			{
				Current_Stage++;
				Sound_SFX_Event_Data sfx_data(Sound_SFX_Tag::Buffer_Move);
				EventManager::GetInstance().Fire(EventType::Play_Audio_SFX, &sfx_data);
			}
		}
	}
	else if (Get_Stage_Select_State() == Stage_Select_State::Bottom_Menu)
	{
		if (Up_Key)
		{
			Set_Stage_Select_State(Stage_Select_State::Stage_Number);
			Sound_SFX_Event_Data sfx_data(Sound_SFX_Tag::Buffer_Move);
			EventManager::GetInstance().Fire(EventType::Play_Audio_SFX, &sfx_data);
		}
		else if (Left_Key && Get_Stage_Menu_Buffer() == Stage_Select_Buffer::Back)
		{
			Set_Stage_Menu_Buffer(Stage_Select_Buffer::Launch);
			Sound_SFX_Event_Data sfx_data(Sound_SFX_Tag::Buffer_Move);
			EventManager::GetInstance().Fire(EventType::Play_Audio_SFX, &sfx_data);
		}
		else if (Right_Key && (Get_Stage_Menu_Buffer() == Stage_Select_Buffer::Launch || Get_Stage_Menu_Buffer() == Stage_Select_Buffer::None))
		{
			Set_Stage_Menu_Buffer(Stage_Select_Buffer::Back);
			Sound_SFX_Event_Data sfx_data(Sound_SFX_Tag::Buffer_Move);
			EventManager::GetInstance().Fire(EventType::Play_Audio_SFX, &sfx_data);
		}

		if (L_Click)
		{
			if (M_INPUT->Is_Mouse_In_Rect(S_Start_X, Sector_Menu_Y, Sector_Menu_W, Sector_Menu_H) ||
				M_INPUT->Is_Mouse_In_Rect(S_Back_X, Sector_Menu_Y, Sector_Menu_W, Sector_Menu_H))
			{
				Confirm_Key = true;
			}
		}

		if (Confirm_Key && Get_Stage_Menu_Buffer() != Stage_Select_Buffer::None && Get_Stage_Menu_Buffer() != Stage_Select_Buffer::Wait)
		{
			// Caal Spawn-Event With Stage Number
			if (Get_Stage_Menu_Buffer() == Stage_Select_Buffer::Launch)
			{
				Sound_SFX_Event_Data sfx_data(Sound_SFX_Tag::Buffer_Select);
				EventManager::GetInstance().Fire(EventType::Play_Audio_SFX, &sfx_data);

				Stage_Event_Data stage_data(Current_Stage);
				EventManager::GetInstance().Fire(EventType::Go_From_Main_Menu_To_In_Game, &stage_data);
			}
			else if (Get_Stage_Menu_Buffer() == Stage_Select_Buffer::Back)
			{
				Sound_SFX_Event_Data sfx_data(Sound_SFX_Tag::Buffer_Back);
				EventManager::GetInstance().Fire(EventType::Play_Audio_SFX, &sfx_data);

				Set_Stage_Menu_State(Stage_Menu_State::Deploy_Menu);
			}
		}
	}
}

void Draw_Deploy_Menu()
{
	Sprite_Draw(Panel_Select_BG, Panel_Deploy_X, Panel_Deploy_Y, Panel_Deploy_W, Panel_Deploy_H);

	Sprite_Draw(UI_Train, Deploy_Menu_X, Tutor_Y, Deploy_Menu_W, Deploy_Menu_H, A_Zero,
		(D_Buffer == Deploy_Select_Buffer::Training) ? Alpha_Origin : Alpha_Half);

	Sprite_Draw(UI_Launch, Deploy_Menu_X, D_Start_Y, Deploy_Menu_W, Deploy_Menu_H, A_Zero,
		(D_Buffer == Deploy_Select_Buffer::Launch) ? Alpha_Origin : Alpha_Half);

	Sprite_Draw(UI_Select, Deploy_Menu_X, Select_Y, Deploy_Menu_W, Deploy_Menu_H, A_Zero,
		(D_Buffer == Deploy_Select_Buffer::Select) ? Alpha_Origin : Alpha_Half);

	Sprite_Draw(UI_Back, Deploy_Menu_X, D_Back_Y, Deploy_Menu_W, Deploy_Menu_H, A_Zero,
		(D_Buffer == Deploy_Select_Buffer::Back) ? Alpha_Origin : Alpha_Half);
}

void Draw_Stage_Select_Menu()
{
	if (UI_Stage_Title == -1) return;

	Sprite_Draw(Panel_Select_BG, Panel_Sector_X, Panel_Sector_Y, Panel_Sector_W, Panel_Sector_H);

	Sprite_Draw(UI_Stage_Title, Title_X, Title_Y, Title_W, Title_H);

	// Get Stage Number
	int T = Current_Stage / 10;
	int O = Current_Stage % 10;

	// Number Draw
	Sprite_Draw(UI_Num[T], Num_Tens_X, Num_Y, Num_W, Num_H, A_Zero, Alpha_Origin);
	Sprite_Draw(UI_Num[O], Num_Ones_X, Num_Y, Num_W, Num_H, A_Zero, Alpha_Origin);

	// Arrow Draw
	if (Current_Stage > MIN_STAGE && UI_Arrow_L != -1)
	{
		Sprite_Draw(UI_Arrow_L, Arrow_L_X, Num_Y, Num_W, Num_H, A_Zero, Alpha_Origin);
	}

	if (Current_Stage < MAX_STAGE && UI_Arrow_R != -1)
	{
		Sprite_Draw(UI_Arrow_R, Arrow_R_X, Num_Y, Num_W, Num_H, A_Zero, Alpha_Origin);
	}

	// Button Draw
	Sprite_Draw(UI_Launch, S_Start_X, Sector_Menu_Y, Sector_Menu_W, Sector_Menu_H, A_Zero,
		(S_Buffer == Stage_Select_Buffer::Launch) ? Alpha_Origin : Alpha_Half);

	Sprite_Draw(UI_Back, S_Back_X, Sector_Menu_Y, Sector_Menu_W, Sector_Menu_H, A_Zero,
		(S_Buffer == Stage_Select_Buffer::Back) ? Alpha_Origin : Alpha_Half);
}