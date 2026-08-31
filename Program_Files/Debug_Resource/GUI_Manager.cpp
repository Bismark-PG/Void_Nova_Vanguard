/*==============================================================================

	Manage GUI Resource [GUI_Manager.h]

	Author : Choi HyungJoon

==============================================================================*/
#include "Project_Header.h"
#include "GUI_Manager.h"
#include "Light_Manager.h"
#include "Player.h"
#include "Debug_Camera.h"
#include "Player_Camera.h"
#include "Debug_Collision.h"
#include "Shader_Manager.h"
#include "Game_Screen_Manager.h"
#include "Main_Menu.h"
#include "Enemy_Spawner.h"
#include "Weapon_System.h"
#include "Setting.h"

using namespace DirectX;

void GUI_Init(HWND hwnd, ID3D11Device* device, ID3D11DeviceContext* context)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // keyboard control
    //io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;      // docking..?

    ImGui::StyleColorsDark();

    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(device, context);
}

void GUI_Final()
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

void GUI_Screen_Scene_Editor(double FPS)
{
    ImGui::Begin("Screen Debug Menu");
    ImGui::Text("FPS: %.1f", FPS);

    // ==========================================
    //           [Screen & State Debug]
    // ==========================================
    if (ImGui::CollapsingHeader("State & Screen Debug", ImGuiTreeNodeFlags_DefaultOpen))
    {
        const char* Main_Screen_Names[]
            = { "M_WAIT", "MAIN", "MENU_SELECT", "SELECT_GAME", "SELECT_SETTINGS", "EXIT", "M_DONE" };
        const char* Sub_Screen_Names[]
            = { "S_WAIT", "STAGE_SELECT", "SETTINGS", "S_DONE" };
        const char* Game_Select_Names[]
            = { "G_WAIT", "GAME_MENU_SELECT", "GAME_PLAYING", "GAME_IN_GAME_MENU", "GAME_SETTING", "G_DONE" };
        const char* Main_Buffer_Names[]
            = { "None", "Wait", "Start", "Setting", "Exit", "Done" };
        const char* Setting_Buffer_Names[]
            = { "None", "Wait", "BGM", "SFX", "WinMode", "FullMode", "Back"};
        const char* Edit_Buffer_Names[]
            = { "None", "BGM", "SFX" };

        int Current_Main = static_cast<int>(Game_Screen_Manager::GetInstance()->Get_Current_Main_Screen());
        if (ImGui::Combo("Main Screen", &Current_Main, Main_Screen_Names, IM_ARRAYSIZE(Main_Screen_Names)))
        {
            Game_Screen_Manager::GetInstance()->Update_Main_Screen(static_cast<Main_Screen>(Current_Main));
        }

        int Current_Sub = static_cast<int>(Game_Screen_Manager::GetInstance()->Get_Current_Sub_Screen());
        if (ImGui::Combo("Sub Screen", &Current_Sub, Sub_Screen_Names, IM_ARRAYSIZE(Sub_Screen_Names)))
        {
            Game_Screen_Manager::GetInstance()->Update_Sub_Screen(static_cast<Sub_Screen>(Current_Sub));
        }

        int Current_Game = static_cast<int>(Game_Screen_Manager::GetInstance()->Get_Current_Game_Select_Screen());
        if (ImGui::Combo("Game Screen", &Current_Game, Game_Select_Names, IM_ARRAYSIZE(Game_Select_Names)))
        {
            Game_Screen_Manager::GetInstance()->Update_Game_Select_Screen(static_cast<Game_Select_Screen>(Current_Game));
        }

        ImGui::Separator();

        int Current_Main_Buffer = static_cast<int>(Get_Main_Menu_Buffer());
        if (ImGui::Combo("Main Buffer", &Current_Main_Buffer, Main_Buffer_Names, IM_ARRAYSIZE(Main_Buffer_Names)))
        {
            Set_Main_Menu_Buffer(static_cast<Main_Select_Buffer>(Current_Main_Buffer));
        }

        int Current_Setting_Buffer = static_cast<int>(Get_Setting_Menu_Buffer());
        if (ImGui::Combo("Setting Buffer", &Current_Setting_Buffer, Setting_Buffer_Names, IM_ARRAYSIZE(Setting_Buffer_Names)))
        {
            Set_Setting_Menu_Buffer(static_cast<Setting_Select_Buffer>(Current_Setting_Buffer));
        }

        int Current_Sound_Buffer = static_cast<int>(Get_Setting_Edit_Buffer());
        if (ImGui::Combo("Edit Buffer", &Current_Sound_Buffer, Edit_Buffer_Names, IM_ARRAYSIZE(Edit_Buffer_Names)))
        {
            Set_Setting_Edit_Buffer(static_cast<Setting_Edit_State>(Current_Sound_Buffer));
        }
    }
    ImGui::End();
}

void GUI_Player_Editor()
{
    ImGui::Begin("Player Debug Menu");

    // ==========================================
    //           [Player Camera Debug]
    // ==========================================
    if (ImGui::CollapsingHeader("Camera & World Illusion Setup", ImGuiTreeNodeFlags_DefaultOpen))
    {
        // 1. Camera Base Position
        static float Cam_Pos_X = 0.0f, Cam_Pos_Y = 2.0f, Cam_Pos_Z = -10.0f;
        ImGui::Text("Camera Anchor Position X");
        bool CX = ImGui::SliderFloat("##CamPosX", &Cam_Pos_X, -20.0f, 20.0f);
        ImGui::Text("Camera Anchor Position Y");
        bool CY = ImGui::SliderFloat("##CamPosY", &Cam_Pos_Y, -10.0f, 20.0f);
        ImGui::Text("Camera Anchor Position Z");
        bool CZ = ImGui::SliderFloat("##CamPosZ", &Cam_Pos_Z, -30.0f, -1.0f);
        if (CX || CY || CZ) GUI_Set_Camera_Base_Pos(Cam_Pos_X, Cam_Pos_Y, Cam_Pos_Z);

        ImGui::Separator();

        // 2. Camera Max Rotations
        static float Max_Pitch = 20.0f, Max_Yaw = 15.0f, Max_Roll = 15.0f;
        ImGui::Text("Max Pitch (Up/Down)");
        bool Pitch = ImGui::SliderFloat("##MaxPitch", &Max_Pitch, 0.0f, 45.0f);
        ImGui::Text("Max Yaw (Left/Right Pan)");
        bool Yaw = ImGui::SliderFloat("##MaxYaw", &Max_Yaw, 0.0f, 45.0f);
        ImGui::Text("Max Roll (Banking)");
        bool Roll = ImGui::SliderFloat("##MaxRoll", &Max_Roll, 0.0f, 45.0f);
        if (Pitch || Yaw || Roll) GUI_Set_Max_Camera_Rotations(Max_Pitch, Max_Yaw, Max_Roll);

        static float Camera_Lerp_Speed = 5.0f;
        ImGui::Text("Camera Rotation Speed (Lerp)");
        if (ImGui::SliderFloat("##CamLerp", &Camera_Lerp_Speed, 1.0f, 20.0f))
            GUI_Set_Camera_Lerp_Speed(Camera_Lerp_Speed);

        ImGui::Separator();

        // 3. Player Position
        static float Player_Pos_X = 0.0f, Player_Pos_Y = 0.0f, Player_Pos_Z = 0.0f;
        ImGui::Text("Player Position X");
        bool PX = ImGui::SliderFloat("##POSX", &Player_Pos_X, -15.0f, 15.0f);
        ImGui::Text("Player Position Y");
        bool PY = ImGui::SliderFloat("##POSY", &Player_Pos_Y, 0.0f, 10.0f);
        ImGui::Text("Player Position Z");
        bool PZ = ImGui::SliderFloat("##POSZ", &Player_Pos_Z, -5.0f, 5.0f);
        if (PX || PY || PZ) GUI_Set_Player_POS(Player_Pos_X, Player_Pos_Y, Player_Pos_Z);

        ImGui::Separator();

        // 4. Player Movement Limits
        static float Limit_X = 15.0f, Limit_Y_Min = 0.0f, Limit_Y_Max = 10.0f;
        ImGui::Text("Player Limit X (Width)");
        bool LX = ImGui::SliderFloat("##LimitX", &Limit_X, 1.0f, 50.0f);
        ImGui::Text("Player Limit Y Min (Ground)");
        bool LY_Min = ImGui::SliderFloat("##LimitYMin", &Limit_Y_Min, -20.0f, 0.0f);
        ImGui::Text("Player Limit Y Max (Sky)");
        bool LY_Max = ImGui::SliderFloat("##LimitYMax", &Limit_Y_Max, 0.0f, 30.0f);
        if (LX || LY_Min || LY_Max) GUI_Set_Player_Limits(Limit_X, Limit_Y_Min, Limit_Y_Max);

        ImGui::Separator();

        // 5. Enemy Spawner Z
        static float Spawn_Z = 150.0f;
        ImGui::Text("Enemy Spawn Z POS");
        if (ImGui::SliderFloat("##SpawnZ", &Spawn_Z, 50.0f, 300.0f))
            Enemy_Spawner::GetInstance().Set_Z_Depth(Spawn_Z);

        // 6. Player Missile Random Logic
        static float X = 2.0f, Y_Min = 0.5f, Y_Max = 2.0f, Z = 1.0f;
        ImGui::Text("Missile Ratio X");
        bool MX = ImGui::SliderFloat("##RatioX", &X, 1.0f, 5.0f);
        ImGui::Text("Missile Range Y Min");
        bool MY_Min = ImGui::SliderFloat("##RangeYMin", &Y_Min, 0.0f, 1.0f);
        ImGui::Text("Missile Range Y Man");
        bool MY_Max = ImGui::SliderFloat("##RangeYMax", &Y_Max, 1.0f, 5.0f);
        ImGui::Text("Missile Start Z POS");
        bool MZ = ImGui::SliderFloat("##StartZ", &Z, 0.0f, 10.0f);
        if (MX || MY_Min || MY_Max || MZ) 
            Weapon_Manager::GetInstance().Set_Missile_Random_POS(X, MY_Min, MY_Max, MZ);
    }
    ImGui::End();
}