/*==============================================================================

	Manage Action Mapping & Input [Input_Manager.h]

	Author : Choi HyungJoon

==============================================================================*/
#include "Project_Header.h"
#include "Input_Manager.h"
#include "KeyLogger.h"

Input_Manager* Input_Manager::GetInstance()
{
	static Input_Manager instance;
	return &instance;
}

// ---------------------------------------------------------//
//						   Movement							//
// ---------------------------------------------------------//
bool Input_Manager::Is_Up_Trigger() const
{
	return KeyLogger_IsTrigger(KK_W) || KeyLogger_IsTrigger(KK_UP) || XKeyLogger_IsPadTrigger(XINPUT_GAMEPAD_DPAD_UP);
}

bool Input_Manager::Is_Down_Trigger() const
{
	return KeyLogger_IsTrigger(KK_S) || KeyLogger_IsTrigger(KK_DOWN) || XKeyLogger_IsPadTrigger(XINPUT_GAMEPAD_DPAD_DOWN);
}

bool Input_Manager::Is_Left_Trigger() const
{
	return KeyLogger_IsTrigger(KK_A) || KeyLogger_IsTrigger(KK_LEFT) || XKeyLogger_IsPadTrigger(XINPUT_GAMEPAD_DPAD_LEFT);
}

bool Input_Manager::Is_Right_Trigger() const
{
	return KeyLogger_IsTrigger(KK_D) || KeyLogger_IsTrigger(KK_RIGHT) || XKeyLogger_IsPadTrigger(XINPUT_GAMEPAD_DPAD_RIGHT);
}

bool Input_Manager::Is_Up_Pressed() const
{
	return KeyLogger_IsPressed(KK_W) || KeyLogger_IsPressed(KK_UP) || XKeyLogger_IsPadPressed(XINPUT_GAMEPAD_DPAD_UP);
}

bool Input_Manager::Is_Down_Pressed() const
{
	return KeyLogger_IsPressed(KK_S) || KeyLogger_IsPressed(KK_DOWN) || XKeyLogger_IsPadPressed(XINPUT_GAMEPAD_DPAD_DOWN);
}

bool Input_Manager::Is_Left_Pressed() const
{
	return KeyLogger_IsPressed(KK_A) || KeyLogger_IsPressed(KK_LEFT) || XKeyLogger_IsPadPressed(XINPUT_GAMEPAD_DPAD_LEFT);
}

bool Input_Manager::Is_Right_Pressed() const
{
	return KeyLogger_IsPressed(KK_D) || KeyLogger_IsPressed(KK_RIGHT) || XKeyLogger_IsPadPressed(XINPUT_GAMEPAD_DPAD_RIGHT);
}

// ---------------------------------------------------------//
//						   Action							//
// ---------------------------------------------------------//
bool Input_Manager::Is_Confirm_Trigger() const
{
	return KeyLogger_IsTrigger(KK_ENTER) || XKeyLogger_IsPadTrigger(XINPUT_GAMEPAD_A);
}

bool Input_Manager::Is_Cancel_Trigger() const
{
	return KeyLogger_IsTrigger(KK_ESCAPE) || KeyLogger_IsTrigger(KK_BACK) || XKeyLogger_IsPadTrigger(XINPUT_GAMEPAD_B);
}

bool Input_Manager::Is_Escape_Trigger() const
{
	return KeyLogger_IsTrigger(KK_ESCAPE);
}

// ---------------------------------------------------------//
//						   Combat							//
// ---------------------------------------------------------//
bool Input_Manager::Is_Weapon_Changed() const
{
	return KeyLogger_IsTrigger(KK_E) || XKeyLogger_IsPadTrigger(XINPUT_GAMEPAD_Y);
}

// ---------------------------------------------------------//
//						Mouse Input							//
// ---------------------------------------------------------//
DirectX::XMFLOAT2 Input_Manager::Mouse_Position() const
{
	return { static_cast<float>(KeyLogger_GetMouse_MoveX()), static_cast<float>(KeyLogger_GetMouse_MoveY()) };
}

bool Input_Manager::Is_Mouse_Moved() const
{
	return KeyLogger_IsMouseMoved();
}

bool Input_Manager::Is_Mouse_In_Rect(float x, float y, float w, float h) const
{
	return Is_Mouse_In_RECT(Mouse_Position().x, Mouse_Position().y, x, y, w, h);
}

bool Input_Manager::Is_Mouse_Left_Trigger() const
{
	return KeyLogger_IsMouseTrigger(LEFT);
}

bool Input_Manager::Is_Mouse_Right_Trigger() const
{
	return KeyLogger_IsMouseTrigger(RIGHT);
}

bool Input_Manager::Is_Mouse_Left_Pressed() const
{
	return KeyLogger_IsMousePressed(LEFT);
}

bool Input_Manager::Is_Mouse_Right_Pressed() const
{
	return KeyLogger_IsMousePressed(RIGHT);
}

bool Input_Manager::Is_Mouse_Visible()
{
	return Mouse_IsVisible();
}

void Input_Manager::Set_Mouse_Visible_Mode(bool Visible)
{
	Mouse_SetVisible(Visible);
}

// ---------------------------------------------------------//
//					   Controller Input						//
// ---------------------------------------------------------//

DirectX::XMFLOAT2 Input_Manager::Controller_Input_L_Stick() const
{
	return XKeyLogger_GetLeftStick();
}

DirectX::XMFLOAT2 Input_Manager::Controller_Input_R_Stick() const
{
	return XKeyLogger_GetRightStick();
}

BYTE Input_Manager::Controlle_Left_Trigger() const
{
	return XKeyLogger_GetLeftTrigger();
}

BYTE Input_Manager::Controlle_Right_Trigger() const
{
	return XKeyLogger_GetRightTrigger();
}


// ---------------------------------------------------------//
//						   Debug							//
// ---------------------------------------------------------//
bool Input_Manager::Debug_Cancel_Trigger() const
{
	return KeyLogger_IsTrigger(KK_X);
}