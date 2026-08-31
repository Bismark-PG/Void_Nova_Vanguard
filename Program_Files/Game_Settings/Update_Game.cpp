/*==============================================================================

	Update Game Screen [Update_Game.cpp]

	Author : Choi HyungJoon

==============================================================================*/
#include "Project_Header.h"
#include "Update_Game.h"
#include "debug_ostream.h"
#include "System_Timer.h"
#include "mouse.h"
#include "KeyLogger.h"
#include "Main_Game.h"
#include "Main_Menu.h"
#include "Title.h"
#include "Setting.h"
#include "Stage_Select.h"
#include "Event_Manager.h"
#include "Game_UI.h"

//---------------Private Logic---------------//
void Sub_Game_Screen_Draw_Update();
void Game_Select_Screen_Draw_Update();

//-----------------Main Logic-----------------//
void Game_Logic_Initialize()
{
	Main_Game_Manager::GetIntance()->In_Game_Initialize();
	Title_Initialize();
	Main_Menu_Initialize();
	Setting_Initialize();
	Stage_Select_Initialize();
	Game_UI_Initialize();
}

void Game_Logic_Finalize()
{
	Game_UI_Finalize();
	Stage_Select_Finalize();
	Setting_Finalize();
	Main_Menu_Finalize();
	Title_Finalize();
	Main_Game_Manager::GetIntance()->In_Game_Finalize();
}

void Game_Logic_Update(double elapsed_time)
{
	float dt = static_cast<float>(elapsed_time);

	Game_UI_Update(dt);

	if (KeyLogger_IsTrigger(KK_BACK))
	{
		if (Game_Screen_Manager::GetInstance()->Get_Current_Main_Screen() == Main_Screen::SELECT_GAME)
		{
			EventManager::GetInstance().Fire(EventType::Go_From_In_Game_To_Main_Menu);
		}
	}

	Game_Screen_Manager::GetInstance()->Apply_Screen_Changes();

	Main_Screen M_State = Game_Screen_Manager::GetInstance()->Get_Current_Main_Screen();
	Sub_Screen S_State = Game_Screen_Manager::GetInstance()->Get_Current_Sub_Screen();
	Game_Select_Screen G_State = Game_Screen_Manager::GetInstance()->Get_Current_Game_Select_Screen();
	bool Is_Sub_Screen_Active = Game_Screen_Manager::GetInstance()->Is_Sub_Screen_Active();

	// If Sub Screen(Setting) Is Pop-Up, Do Update Setting Only
	if (Is_Sub_Screen_Active)
	{
		Game_Logic_Sub(S_State, dt);
	}
	// When Sub Scrren State Is Wait, It Will Be Setting Done.
	// So Do Update Main Logic(Menu, In-Game Logic)
	else
	{
		Game_Logic_Menu(M_State, dt);
		Game_Logic_InGame(G_State, dt);
	}
}

void Game_Logic_Menu(Main_Screen State, float elapsed_time)
{
	switch (State)
	{
	case Main_Screen::M_WAIT:
	case Main_Screen::MAIN:
		Title_Update(elapsed_time);
		break;

	case Main_Screen::MENU_SELECT:
		Main_Menu_Update(elapsed_time);
		break;

	case Main_Screen::SELECT_SETTINGS:
	case Main_Screen::SELECT_GAME:
	case Main_Screen::EXIT:
		break;
	}
}

void Game_Logic_Sub(Sub_Screen State, float elapsed_time)
{
	switch (State)
	{
	case Sub_Screen::S_WAIT:
		break;

	case Sub_Screen::STAGE_SELECT:
		Stage_Select_Update(elapsed_time);
		break;

	case Sub_Screen::SETTINGS:
		Setting_Update(elapsed_time);
		break;

	case Sub_Screen::S_DONE:
		break;
	}
}

void Game_Logic_InGame(Game_Select_Screen State, float elapsed_time)
{
	switch (State)
	{
	case Game_Select_Screen::G_WAIT:
		break;

	case Game_Select_Screen::GAME_MENU_SELECT:
		break;

	case Game_Select_Screen::GAME_PLAYING:
		if (KeyLogger_IsTrigger(KK_ESCAPE) || XKeyLogger_IsPadTrigger(XINPUT_GAMEPAD_START))
		{
			EventManager::GetInstance().Fire(EventType::Open_Settings);
		}

		Main_Game_Manager::GetIntance()->In_Game_Update(elapsed_time);
		break;

	case Game_Select_Screen::GAME_IN_GAME_MENU:
		break;

	case Game_Select_Screen::GAME_SETTING:
		break;

	case Game_Select_Screen::G_DONE:
		break;
	}
}

void Main_Game_Screen_Draw_Update()
{
	Main_Screen current_screen = Game_Screen_Manager::GetInstance()->Get_Current_Main_Screen();

	switch (current_screen)
	{
	case Main_Screen::M_WAIT:
		Mouse_SetMode(MOUSE_POSITION_MODE_ABSOLUTE);
		Game_Screen_Manager::GetInstance()->Update_Main_Screen(Main_Screen::MAIN);
		break;

	case Main_Screen::MAIN:
		Title_Draw();
		break;

	case Main_Screen::MENU_SELECT:
		Main_Menu_Draw();
		break;

	case Main_Screen::SELECT_GAME:
		Game_Select_Screen_Draw_Update();
		break;

	case Main_Screen::EXIT:
		Game_Screen_Manager::GetInstance()->Update_Main_Screen(Main_Screen::M_DONE);
		Main_Menu_Draw();
		Debug::D_Out << "Exiting Game..." << std::endl;
		break;

	case Main_Screen::M_DONE:
		Main_Menu_Draw();
		Debug::D_Out << "Programme Done" << std::endl;
		break;
	}

	Sub_Game_Screen_Draw_Update();

	// Draw In Game UI
	Game_UI_Draw();
}

void Sub_Game_Screen_Draw_Update()
{
	Sub_Screen current_screen = Game_Screen_Manager::GetInstance()->Get_Current_Sub_Screen();

	switch (current_screen)
	{
	case Sub_Screen::S_WAIT:
		break;

	case Sub_Screen::STAGE_SELECT:
		Stage_Select_Draw();
		break;

	case Sub_Screen::SETTINGS:
		Setting_Draw();
		break;

	case Sub_Screen::S_DONE:
		Game_Screen_Manager::GetInstance()->Update_Main_Screen(Main_Screen::MENU_SELECT);
		Game_Screen_Manager::GetInstance()->Update_Sub_Screen(Sub_Screen::S_WAIT);
		break;
	}
}

void Game_Select_Screen_Draw_Update()
{
	Game_Select_Screen current_screen = Game_Screen_Manager::GetInstance()->Get_Current_Game_Select_Screen();

	switch (current_screen)
	{
	case Game_Select_Screen::G_WAIT:
		break;

	case Game_Select_Screen::GAME_MENU_SELECT:
		break;

	case Game_Select_Screen::GAME_PLAYING:
		Main_Game_Manager::GetIntance()->In_Game_Draw();
		break;

	case Game_Select_Screen::GAME_IN_GAME_MENU:
		break;

	case Game_Select_Screen::GAME_SETTING:
		break;

	case Game_Select_Screen::G_DONE:
		Game_Screen_Manager::GetInstance()->Update_Main_Screen(Main_Screen::MENU_SELECT);
		Game_Screen_Manager::GetInstance()->Update_Game_Select_Screen(Game_Select_Screen::G_WAIT);
		break;
	}
}