/*==============================================================================

	Manage Game Screen [Game_Screen_Manager.cpp]

	Author : Choi HyungJoon

==============================================================================*/
#include "Game_Screen_Manager.h"

Game_Screen_Manager* Game_Screen_Manager::GetInstance()
{
	static Game_Screen_Manager instance;
	return &instance;
}

void Game_Screen_Manager::Change_Screen(Main_Screen M_Screen, Sub_Screen S_Screen, Game_Select_Screen G_Screen)
{
	Next_Main_Screen = M_Screen;
	Next_Sub_Screen = S_Screen;
	Next_Game_Screen = G_Screen;
}

void Game_Screen_Manager::Update_Main_Screen(Main_Screen Main_Input)
{
	Next_Main_Screen = Main_Input;
}

Main_Screen Game_Screen_Manager::Get_Current_Main_Screen() const
{
	return Current_Main_Screen;
}

void Game_Screen_Manager::Update_Sub_Screen(Sub_Screen Sub_Input)
{
	Next_Sub_Screen = Sub_Input;
}

Sub_Screen Game_Screen_Manager::Get_Current_Sub_Screen() const
{
	return Current_Sub_Screen;
}

void Game_Screen_Manager::Update_Game_Select_Screen(Game_Select_Screen Game_Select_Input)
{
	Next_Game_Screen = Game_Select_Input;
}

Game_Select_Screen Game_Screen_Manager::Get_Current_Game_Select_Screen() const
{
	return Current_Game_Screen;
}

bool Game_Screen_Manager::Is_Main_Screen_Active() const
{
	return Current_Main_Screen != Main_Screen::MAIN;
}

bool Game_Screen_Manager::Is_Sub_Screen_Active() const
{
	return Current_Sub_Screen != Sub_Screen::S_WAIT;
}

bool Game_Screen_Manager::Is_Pure_In_Game_State() const
{
	return (Current_Main_Screen == Main_Screen::SELECT_GAME && Current_Sub_Screen == Sub_Screen::S_WAIT &&
		Current_Game_Screen == Game_Select_Screen::GAME_PLAYING);
}

void Game_Screen_Manager::Apply_Screen_Changes()
{
	Current_Main_Screen = Next_Main_Screen;
	Current_Sub_Screen = Next_Sub_Screen;
	Current_Game_Screen = Next_Game_Screen;
}