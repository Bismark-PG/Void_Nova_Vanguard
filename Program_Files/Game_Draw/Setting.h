/*==============================================================================

	Game Setting [Setting.h]

	Author : Choi HyungJoon

==============================================================================*/
#ifndef SETTING_H
#define SETTING_H

enum class Setting_Select_Buffer
{
	None,
	Wait,
	BGM,
	SFX,
	WinMode,
	FullMode,
	Back
};

enum class Setting_Edit_State
{
	None,
	BGM,
	SFX
};

void Setting_Initialize();
void Setting_Finalize();

void Setting_Update(float dt);
void Setting_Draw();

Setting_Select_Buffer Get_Setting_Menu_Buffer();
void Set_Setting_Menu_Buffer(Setting_Select_Buffer Buffer);

Setting_Edit_State Get_Setting_Edit_Buffer();
void Set_Setting_Edit_Buffer(Setting_Edit_State Buffer);
bool Is_Setting_Edit_Active();

#endif // SETTING_H