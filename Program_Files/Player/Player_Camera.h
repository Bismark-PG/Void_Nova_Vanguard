/*==============================================================================

	Make Camera [Player_Camera.h]

	Author : Choi HyungJoon

==============================================================================*/
#ifndef PLAYER_CAMERA_H
#define PLAYER_CAMERA_H

#include <DirectXMath.h>

void Player_Camera_Initialize();
void Player_Camera_Finalize();

void Player_Camera_Reset();
void Player_Camera_Update(float elapsed_time);

void Player_Camera_Shake(float M, float D);

void Set_Mouse_Sensitivity(float Sensitivity);
float Get_Mouse_Sensitivity();

// --- Getter ---
const DirectX::XMFLOAT3& Player_Camera_Get_POS();
const DirectX::XMFLOAT3& Player_Camera_Get_Front();
float Player_Camera_Get_Far_Z();
const DirectX::XMFLOAT4X4& Player_Camera_Get_View_Matrix();
const DirectX::XMFLOAT4X4& Player_Camera_Get_Proj_Matrix();

float Get_Player_Limit_X();
float Get_Player_Limit_Y_Min();
float Get_Player_Limit_Y_Max();

float Get_Player_Camera_Pitch();
float Get_Player_Camera_Yaw();
float Get_Player_Camera_Roll();

// --- Debug Function ---
void GUI_Set_Camera_Base_Pos(float x, float y, float z);
void GUI_Set_Max_Camera_Rotations(float pitch, float yaw, float roll);
void GUI_Set_Camera_Lerp_Speed(float speed);
void GUI_Set_Player_Limits(float limitX, float limitYMin, float limitYMax);

#endif //PLAYER_CAMERA_H