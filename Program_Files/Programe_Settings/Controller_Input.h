/*==============================================================================

	Controller Input Draw [Controller_Input.h]

	Author : Choi HyungJoon

==============================================================================*/
#ifndef CONTROLLER_INPUT_H
#define CONTROLLER_INPUT_H

enum class CONTROLLER_STATE
{
	NONE,
	INPUT,
	OUTPUT
};

void Controller_Set_Initialize();
void Controller_Set_Finalize();

void Controller_Set_Update();
void Controller_Set_Draw();

bool Controller_Set_UP();

void Controller_Texture();

#endif // CONTROLLER_INPUT_H