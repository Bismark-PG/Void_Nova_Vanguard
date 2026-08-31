/*==============================================================================

	Game Main Menu [Main_Menu.h]

	Author : Choi HyungJoon

==============================================================================*/
#ifndef MAIN_MENU_H
#define MAIN_MENU_H

enum class Main_Select_Buffer
{
	None,
	Wait,
	Start,
	Setting,
	Ranking,
	Exit,
	Done
};

enum class Menu_State : int
{
	Wait = 0,
	Glow = 1
};

void Main_Menu_Initialize();
void Main_Menu_Finalize();

void Main_Menu_Update(float elapsed_time);
void Main_Menu_Draw();
void Main_Menu_BG_Draw();
void Main_Menu_UI_Draw();

Main_Select_Buffer Get_Main_Menu_Buffer();
void Set_Main_Menu_Buffer(Main_Select_Buffer Buffer);

bool IF_IS_Game_Done();

#endif // MAIN_MENU_H