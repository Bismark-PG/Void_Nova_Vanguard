/*==============================================================================

	Game Stage Select [Stage_Select.h]

	Author : Choi HyungJoon

==============================================================================*/
#ifndef STAGE_SELECT_H
#define STAGE_SELECT_H

// Enum For 1st Panel Buffer (Deploy Menu)
enum class Deploy_Select_Buffer
{
	None,
	Wait,
	Training,
	Launch,
	Select,
	Back
};

// Enum For 2nd Panel Buffer (Stage Select Menu)
enum class Stage_Select_Buffer
{
	None,
	Wait,
	Launch,
	Back
};

enum class Stage_Menu_State
{
	Deploy_Menu, 
	Stage_Select 
};

enum class Stage_Select_State
{
	Stage_Number,
	Bottom_Menu 
};

void Stage_Select_Initialize();
void Stage_Select_Finalize();

void Stage_Select_Update(float elapsed_time);
void Stage_Select_Draw();

Deploy_Select_Buffer Get_Deploy_Menu_Buffer();
void Set_Deploy_Menu_Buffer(Deploy_Select_Buffer Buffer);

Stage_Select_Buffer Get_Stage_Menu_Buffer();
void Set_Stage_Menu_Buffer(Stage_Select_Buffer Buffer);

Stage_Menu_State Get_Stage_Menu_State();
void Set_Stage_Menu_State(Stage_Menu_State State);

Stage_Select_State Get_Stage_Select_State();
void Set_Stage_Select_State(Stage_Select_State State);

#endif // STAGE_SELECT_H