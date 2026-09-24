/*==============================================================================

	Make Camera [Player_Camera.cpp]

	Author : Choi HyungJoon

==============================================================================*/
#include "Project_Header.h"
#include "Player_Camera.h"
#include "Player.h"
#include "Shader_Manager.h"
#include "direct3d.h"
#include "KeyLogger.h"
#include "debug_ostream.h"
#include "Game_Window.h"
#include "Heapler_Logic.h"
using namespace DirectX;

// Controller Constants
constexpr float CONTROLLER_STICK_MAX = 32767.0f; // Max Value of Short
constexpr float CONTROLLER_STICK_WEIGHT = 800.0f;   // Pad Input Multiplier (Higher = Faster)
constexpr float CAMERA_ROTATION_SCALE = 0.005f;
#define XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE 8689

// Camera System Parameters
static float Camera_Near_Z = 0.1f, Camera_Far_z = 1000.0f;
static float Camera_Base_X = 0.0f;
static float Camera_Base_Y = 2.0f;
static float Camera_Base_Z = -10.0f;    // Default Camera POS

// Shack Parameters
static float Shake_Duration  = 0.0f;
static float Shake_Initial_Duration = 0.0f;
static float Shake_Magnitude = 0.0f;
static bool Is_Camera_Shaking = false;

// Sensitivity
static float Mouse_Sensitivity = 0.005f; // Base Setting (From Option)
static float Apply_Sensitivity = 0.005f; // Real used value

// Matrices & Pos
static DirectX::XMFLOAT4X4 Camera_View_mtx;
static DirectX::XMFLOAT4X4 Camera_Proj_mtx;
static XMFLOAT3 Current_Camera_Pos = { 0.0f, 0.0f, -10.0f };
static XMFLOAT3 Camera_Front = { 0.0f, 0.0f, 1.0f };

// Current Camera Rotation (Lerped)
static float Current_Pitch = 0.0f;
static float Current_Yaw = 0.0f;
static float Current_Roll = 0.0f;

// Max Rotation Limits (GUI Adjustable)
static float Max_Camera_Pitch = 20.0f;  // Up, Down
static float Max_Camera_Yaw = 15.0f;    // Left, Right
static float Max_Camera_Roll = 15.0f;   // Rotation
static float Camera_Lerp_Speed = 5.0f;

// Player Movement Limits (Shared World Rules)
static float Player_Limit_X     = 15.0f;
static float Player_Limit_Y_Min = 0.0f; // Ground
static float Player_Limit_Y_Max = 10.0f;  // Sky

// ----------------------------------------------------------
//				static Player Update Logic
// ----------------------------------------------------------
static void Do_A_Shake(XMVECTOR& POS, float dt);
// ----------------------------------------------------------
void Player_Camera_Initialize()
{
    Current_Camera_Pos = { Camera_Base_X, Camera_Base_Y, Camera_Base_Z };
    Camera_Front = { 0.0f, 0.0f, 1.0f };
	Is_Camera_Shaking = false;
}

void Player_Camera_Finalize()
{
}

void Player_Camera_Reset()
{
    Apply_Sensitivity = Mouse_Sensitivity;
}

void Player_Camera_Update(float elapsed_time)
{
    // --- Position Calculation ---
    // Get Player POS
    XMFLOAT3 Player_POS = Player_Get_POS();

    float Ratio_X = Player_POS.x / Player_Limit_X;
    float Ratio_Y = 0.0f;
    if (Player_POS.y > 0.0f) Ratio_Y = Player_POS.y / Player_Limit_Y_Max;
    else if (Player_POS.y < 0.0f) Ratio_Y = -(Player_POS.y / Player_Limit_Y_Min);

    Ratio_X = ClampFloat(Ratio_X, -1.0f, 1.0f);
    Ratio_Y = ClampFloat(Ratio_Y, -1.0f, 1.0f);

    // Get Target POS
    float Target_Pitch = -Ratio_Y * XMConvertToRadians(Max_Camera_Pitch);
    float Target_Yaw = Ratio_X * XMConvertToRadians(Max_Camera_Yaw);
    float Target_Roll = -Ratio_X * XMConvertToRadians(Max_Camera_Roll); // Rotation

    // Lerp Camera Position
    Current_Pitch += (Target_Pitch - Current_Pitch) * Camera_Lerp_Speed * elapsed_time;
    Current_Yaw += (Target_Yaw - Current_Yaw) * Camera_Lerp_Speed * elapsed_time;
    Current_Roll += (Target_Roll - Current_Roll) * Camera_Lerp_Speed * elapsed_time;

    // Update View Matrix
    XMMATRIX Rot_Matrix = XMMatrixRotationRollPitchYaw(Current_Pitch, Current_Yaw, Current_Roll);
    XMVECTOR Cam_Pos = XMVectorSet(Camera_Base_X, Camera_Base_Y, Camera_Base_Z, 1.0f); // X, Y Must Be "0"

    if (Is_Camera_Shaking)
    {
        Do_A_Shake(Cam_Pos, elapsed_time);
    }

    XMVECTOR Up_Vector = XMVector3TransformNormal(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), Rot_Matrix);
    XMVECTOR Forward_Vector = XMVector3TransformNormal(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), Rot_Matrix);

    XMStoreFloat3(&Camera_Front, Forward_Vector);
    XMStoreFloat3(&Current_Camera_Pos, Cam_Pos);

    XMMATRIX View_Matrix = XMMatrixLookAtLH(Cam_Pos, Cam_Pos + Forward_Vector, Up_Vector);
    XMStoreFloat4x4(&Camera_View_mtx, View_Matrix);
    Shader_Manager::GetInstance()->SetViewMatrix3D(View_Matrix);

    // Update Projection Matrix
    float w = static_cast<float>(Window_Manager::GetInstance()->GetWidth());
    float h = static_cast<float>(Window_Manager::GetInstance()->GetHeight());
    XMMATRIX Proj_Matrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(60.0f), w / h, Camera_Near_Z, Camera_Far_z);

    XMStoreFloat4x4(&Camera_Proj_mtx, Proj_Matrix);
    Shader_Manager::GetInstance()->SetProjectionMatrix3D(Proj_Matrix);
}

void Player_Camera_Shake(float M, float D)
{
    Shake_Duration = D;
    Shake_Initial_Duration = D;
    Shake_Magnitude = M;
    Is_Camera_Shaking = true;
}

// ----------------------------------------------------------------------------------------------------------------
//								        	Player Camera Getter Logic
// ----------------------------------------------------------------------------------------------------------------
const XMFLOAT3& Player_Camera_Get_POS()
{
    return Current_Camera_Pos; 
}

const XMFLOAT3& Player_Camera_Get_Front() 
{
    return Camera_Front;
}

float Player_Camera_Get_Far_Z()
{ 
    return Camera_Far_z; 
}

const XMFLOAT4X4& Player_Camera_Get_View_Matrix()
{
    return Camera_View_mtx;
}

const XMFLOAT4X4& Player_Camera_Get_Proj_Matrix()
{
    return Camera_Proj_mtx;
}

float Get_Player_Limit_X() 
{
    return Player_Limit_X; 
}

float Get_Player_Limit_Y_Min()
{ 
    return Player_Limit_Y_Min;
}

float Get_Player_Limit_Y_Max()
{ 
    return Player_Limit_Y_Max;
}

float Get_Player_Camera_Pitch() 
{ 
    return Current_Pitch;
}

float Get_Player_Camera_Yaw() 
{ 
    return Current_Yaw; 
}

float Get_Player_Camera_Roll() 
{
    return Current_Roll; 
}

// ----------------------------------------------------------------------------------------------------------------
//								        	Player Camera Setting Logic
// ----------------------------------------------------------------------------------------------------------------
void Set_Mouse_Sensitivity(float Sensitivity)
{
    Mouse_Sensitivity = Sensitivity;
}

float Get_Mouse_Sensitivity()
{
    return Mouse_Sensitivity;
}

// ----------------------------------------------------------------------------------------------------------------
//								        	Player Camera Debug Logic
// ----------------------------------------------------------------------------------------------------------------
void GUI_Set_Camera_Base_Pos(float x, float y, float z)
{
    Camera_Base_X = x;
    Camera_Base_Y = y;
    Camera_Base_Z = z;
}

void GUI_Set_Max_Camera_Rotations(float pitch, float yaw, float roll)
{ 
    Max_Camera_Pitch = pitch; 
    Max_Camera_Yaw = yaw; 
    Max_Camera_Roll = roll;
}

void GUI_Set_Camera_Lerp_Speed(float speed) 
{ 
    Camera_Lerp_Speed = speed; 
}

void GUI_Set_Player_Limits(float limitX, float limitYMin, float limitYMax) 
{ 
    Player_Limit_X = limitX; 
    Player_Limit_Y_Min = limitYMin; 
    Player_Limit_Y_Max = limitYMax; 
}

// ----------------------------------------------------------------------------------------------------------------
//								        	Player Camera Shake Logic
// ----------------------------------------------------------------------------------------------------------------
void Do_A_Shake(XMVECTOR& POS, float dt)
{
    if (Shake_Duration > 0.0f)
    {
        Shake_Duration -= dt;

        if (Shake_Duration <= 0.0f)
        {
            Shake_Duration = 0.0f;
            Is_Camera_Shaking = false;
        }
        else
        {
            float Current_Mag = Shake_Magnitude * (Shake_Duration / Shake_Initial_Duration);

            float Offset_X = (RandomFloatMinus1To1() * Current_Mag);
            float Offset_Y = (RandomFloatMinus1To1() * Current_Mag);

            XMVECTOR Shake_Offset = XMVectorSet(Offset_X, Offset_Y, 0.0f, 0.0f);
            POS += Shake_Offset;
        }
    }
}
