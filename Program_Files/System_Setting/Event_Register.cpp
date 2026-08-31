/*==============================================================================

	Event Type For Event System [Event_Register.h]

	Author : Choi HyungJoon

==============================================================================*/
#include "Project_Header.h"

// Event Register
#include "Event_Manager.h"
#include "Event_Register.h"
#include "Sound_Register.h"
#include "Combat_Register.h"
#include "Game_Flow_Register.h"

// Manager Include
#include "KeyLogger.h"
#include "Shader_Manager.h"
#include "Light_Manager.h"
#include "Audio_Manager.h"
#include "Bullet_Manager.h"
#include "Particle_Manager.h"
#include "Game_Screen_Manager.h"

// Game Draw Include
#include "Fade.h"
#include "Main_Game.h"
#include "Main_Menu.h"

// Enemy Spawner Include
#include "Enemy_Spawner.h"

// Debug Output
#include "debug_ostream.h"

// ----------------------------------------------------------------------------------------------------------------
//									            Event Register
// ----------------------------------------------------------------------------------------------------------------
void Scene_Change_Event_Register();
void Pop_Up_Event_Register();
void Game_Flow_Event_Register();
void Audio_Event_Register();
void Combat_Event_Register();

// ----------------------------------------------------------------------------------------------------------------
//									            Register Master
// ----------------------------------------------------------------------------------------------------------------
void Register_All_Events()
{
    Scene_Change_Event_Register();
    Pop_Up_Event_Register();
    Game_Flow_Event_Register();
    Audio_Event_Register();
    Combat_Event_Register();
}
// ----------------------------------------------------------------------------------------------------------------

// ----------------------------------------------------------------------------------------------------------------
//										    Scene Change Event Register
// ----------------------------------------------------------------------------------------------------------------
void Scene_Change_Event_Register()
{
    auto& Event_Manager = EventManager::GetInstance();

    // Event For Change Tatle To Main Menu Scene
    Event_Manager.Subscribe(EventType::Go_From_Title_To_Main_Menu, [](EventData*) {
        Game_Screen_Manager::GetInstance()->Change_Screen(Main_Screen::MENU_SELECT, Sub_Screen::S_WAIT, Game_Select_Screen::G_WAIT);
        Debug::D_Out << "[Event System] Event Triggered : Go To Main Menu" << std::endl;
        });

    // Event For Start In Game Scene
    Event_Manager.Subscribe(EventType::Go_From_Main_Menu_To_In_Game, [](EventData* data) {
        auto* stage_data = static_cast<Stage_Event_Data*>(data);
        int target_stage = (stage_data != nullptr) ? stage_data->Stage_Number : 1;

        // Scene Change And Reset
        Game_Screen_Manager::GetInstance()->Change_Screen(Main_Screen::SELECT_GAME, Sub_Screen::S_WAIT, Game_Select_Screen::GAME_PLAYING);
        Main_Game_Manager::GetIntance()->In_Game_Reset();

        // Target Stage Init
        Enemy_Spawner::GetInstance().Start_Stage(target_stage);

        Mouse_SetVisible(false);

        Debug::D_Out << "[Event System] Event Triggered : Game Start, Stage : " << target_stage << std::endl;
        });

    // Event For Change In Game Scene To Main Menu Scene
    Event_Manager.Subscribe(EventType::Go_From_In_Game_To_Main_Menu, [](EventData*) {
        Game_Screen_Manager::GetInstance()->Change_Screen(Main_Screen::MENU_SELECT, Sub_Screen::S_WAIT, Game_Select_Screen::G_WAIT);
        Main_Game_Manager::GetIntance()->In_Game_Reset();

        Set_Main_Menu_Buffer(Main_Select_Buffer::None);
        Mouse_SetMode(MOUSE_POSITION_MODE_ABSOLUTE);
        Mouse_SetVisible(true);

        Debug::D_Out << "[Event System] Event Triggered : Go From In Game To Main Menu" << std::endl;
        });

    Event_Manager.Subscribe(EventType::Start_Fade, [](EventData* data) {
        auto* Fade_Data = static_cast<Fade_Event_Data*>(data);
        if (Fade_Data != nullptr)
        {
            Fade_Start(Fade_Data->Duration, Fade_Data->IsFadeOut, Fade_Data->FadeColor);
            Debug::D_Out << "[Event System] Fade Started. Duration : " << Fade_Data->Duration << " Sec" << std::endl;
        }
        });
}

// ----------------------------------------------------------------------------------------------------------------
//									            Pop-UP Event Register
// ----------------------------------------------------------------------------------------------------------------
void Pop_Up_Event_Register()
{
    auto& Event_Manager = EventManager::GetInstance();

	// Event For Open Stage Select Window
    Event_Manager.Subscribe(EventType::Open_Stage_Select, [](EventData*) {
        Game_Screen_Manager::GetInstance()->Update_Sub_Screen(Sub_Screen::STAGE_SELECT);
        Debug::D_Out << "[Event System] Stage Select Panel Pop-Up" << std::endl;
        });

	// Event For Close Stage Select Window
    Event_Manager.Subscribe(EventType::Close_Stage_Select, [](EventData*) {
        Game_Screen_Manager::GetInstance()->Update_Sub_Screen(Sub_Screen::S_WAIT);
        Set_Main_Menu_Buffer(Main_Select_Buffer::Start);
        Debug::D_Out << "[Event System] Stage Select Panel Closed" << std::endl;
        });

	// Event For Open Settings Window
    Event_Manager.Subscribe(EventType::Open_Settings, [](EventData*) {
        Game_Screen_Manager::GetInstance()->Update_Sub_Screen(Sub_Screen::SETTINGS);
        Debug::D_Out << "[Event System] Event Triggered : Setting Panel Pop-Up" << std::endl;
        });

	// Event For Close Settings Window
    Event_Manager.Subscribe(EventType::Close_Settings, [](EventData*) {
        Game_Screen_Manager::GetInstance()->Update_Sub_Screen(Sub_Screen::S_WAIT);

        if (Game_Screen_Manager::GetInstance()->Get_Current_Main_Screen() == Main_Screen::MENU_SELECT)
        {
            Set_Main_Menu_Buffer(Main_Select_Buffer::Setting);
        }

        Debug::D_Out << "[Event System] Event Triggered : Setting Panel Closed" << std::endl;
        });

    // Event For Open Ranking Menu
    Event_Manager.Subscribe(EventType::Open_Ranking, [](EventData*) {
		// Need Ranking Screen Implementation
        // Game_Screen_Manager::GetInstance()->Update_Main_Screen(Main_Screen::SELECT_RANKING);
        Debug::D_Out << "[Event System] Event Triggered : Ranking Panel Pop-Up" << std::endl;
        });
}

// ----------------------------------------------------------------------------------------------------------------
//										    Game Flow Event Register
// ----------------------------------------------------------------------------------------------------------------

void Game_Flow_Event_Register()
{
    auto& Event_Manager = EventManager::GetInstance();

    // Event For Stage Start With Phase Number
    //Event_Manager.Subscribe(EventType::Stage_Start, [](EventData* data) {
    //    auto* stage_data = static_cast<Stage_Event_Data*>(data);

    //    if (stage_data)
    //    {
    //        Enemy_Spawner::GetInstance().Start_Stage(stage_data->Stage_Number);
    //        Debug::D_Out << "[Event System] Stage " << stage_data->Stage_Number << " Started!" << std::endl;
    //    }
    //    });

    // Event For Stage Change Flow
    Event_Manager.Subscribe(EventType::Phase_Spawning_Done, [](EventData*) {
        Debug::D_Out << "[Event System] Phase Spawning Done." << std::endl;
        // Need Game Flow Change Logic
        });

    // Event For Game Exit
    Event_Manager.Subscribe(EventType::Exit_Game, [](EventData*) {
        Game_Screen_Manager::GetInstance()->Change_Screen(Main_Screen::EXIT, Sub_Screen::S_DONE, Game_Select_Screen::G_DONE);
        Debug::D_Out << "[Event System] Event Triggered : Game Done, Programe Shut-Down" << std::endl;
        });
}

// ----------------------------------------------------------------------------------------------------------------
//								                Audio Event Register
// ----------------------------------------------------------------------------------------------------------------
void Audio_Event_Register()
{
    auto& Event_Manager = EventManager::GetInstance();

	// Event For Play BGM
    Event_Manager.Subscribe(EventType::Play_Audio_BGM, [](EventData* data) {
		// Casting Event Data to Sound_BGM_Event_Data
        auto* Sound_Data = static_cast<Sound_BGM_Event_Data*>(data);

        if (Sound_Data != nullptr)
        {
            // Get BGM Audio Tag
            std::string BGM_Name = Get_BGM_String(Sound_Data->BGM_Tag);
            Audio_Manager::GetInstance()->Play_BGM(BGM_Name);
            Debug::D_Out << "[Event System] Play BGM Requested : " << BGM_Name << std::endl;
        }
        });

	// Event For Stop BGM
    Event_Manager.Subscribe(EventType::Stop_Audio_BGM, [](EventData* data) {
        // Casting Event Data to Sound_BGM_Event_Data
        auto* Sound_Data = static_cast<Sound_BGM_Event_Data*>(data);

        if (Sound_Data != nullptr)
        {
            // Get BGM Audio Tag
            std::string BGM_Name = Get_BGM_String(Sound_Data->BGM_Tag);
            Audio_Manager::GetInstance()->Stop_BGM(BGM_Name);
			Debug::D_Out << "[Event System] Stop BGM Requested : " << BGM_Name << std::endl;
        }
        });

	// Event For Play SFX
    Event_Manager.Subscribe(EventType::Play_Audio_SFX, [](EventData* data) {
        // Casting Event Data to Sound_SFX_Event_Data
        auto* Sound_Data = static_cast<Sound_SFX_Event_Data*>(data);

        if (Sound_Data != nullptr)
        {
            // Get SFX Audio Tag
            std::string SFX_Name = Get_SFX_String(Sound_Data->SFX_Tag);
            Audio_Manager::GetInstance()->Play_SFX(SFX_Name);
            Debug::D_Out << "[Event System] Play SFX Requested : " << SFX_Name << std::endl;
        }
        });

	// Event For Stop All SFX
    Event_Manager.Subscribe(EventType::Stop_Audio_SFX_All, [](EventData*) {
        Audio_Manager::GetInstance()->Stop_All_SFX();
		Debug::D_Out << "[Event System] Stop All SFX Requested" << std::endl;
        });

	// Event For Change Audio Volume
    Event_Manager.Subscribe(EventType::Change_Audio_Volume, [](EventData* data) {
        auto* Volume_Data = static_cast<Sound_Volume_Event_Data*>(data);
        if (Volume_Data != nullptr)
        {
            if (Volume_Data->Is_BGM)
                Audio_Manager::GetInstance()->Set_Target_BGM_Volume(Volume_Data->Volume);
            else
                Audio_Manager::GetInstance()->Set_Target_SFX_Volume(Volume_Data->Volume);

			Debug::D_Out << "[Event System] Audio Volume Changed - Sound Type : " << (Volume_Data->Is_BGM ? "BGM" : "SFX")
                << ", Volume : " << Volume_Data->Volume * 10 << std::endl;
        }
        });
}

void Combat_Event_Register()
{
    auto& Event_Manager = EventManager::GetInstance();

    // ----------------------------------------------------------------------------------------------------------------
    //                                          Combet State Event Register
    // ----------------------------------------------------------------------------------------------------------------

	// Event For Player Weapon Change
    Event_Manager.Subscribe(EventType::Player_Weapon_Changed, [](EventData* data) {
        auto* Weapon_Data = static_cast<Combat_Weapon_Event_Data*>(data);
        if (Weapon_Data)
        {
            // Need UI Logic
            // Need Weapon Change Sound
            Debug::D_Out << "[Event System] Weapon Changed To: "
                << (Weapon_Data->Weapon_Type == WeaponType::MACHINE_GUN ? "Machine Gun" : "Missile") << std::endl;
        }
        });

	// Event For Player Weapon Fire
    Event_Manager.Subscribe(EventType::Player_Fired, [](EventData* data) {
        auto* Weapon_Data = static_cast<Combat_Weapon_Event_Data*>(data);
        if (Weapon_Data)
        {
            if (Weapon_Data->Weapon_Type == WeaponType::MACHINE_GUN)
            {
                // Need Fire Sound
				// Need Camera Shake ?
                Debug::D_Out << "[Event System] Weapon Fired. Weapon Info : Machine Gun" << std::endl;
            }
            else if (Weapon_Data->Weapon_Type == WeaponType::MISSILE)
            {
                // Need Fire Sound
                Debug::D_Out << "[Event System] Weapon Fired. Weapon Info : Missile" << std::endl;
            }
        }
        });

	// Event For Enemy Hit Visual Effect
    Event_Manager.Subscribe(EventType::Visual_Hit_Effect, [](EventData* data) {
        auto* Hit_Data = static_cast<Combat_Hit_Event_Data*>(data);
        if (Hit_Data)
        {
            Particle_Manager::GetInstance().Spawn_Spark(Hit_Data->Hit_POS);
            // Need Hit Sound
            Debug::D_Out << "[Event System] Enemy Visual Hit Effect Succesed" << std::endl;
        }
        });

    // ----------------------------------------------------------------------------------------------------------------
    //                                          Combet Action Request Event Register
    // ----------------------------------------------------------------------------------------------------------------

	// Event For Player Weapon Change Request
    Event_Manager.Subscribe(EventType::Player_Weapon_Change_Requested, [](EventData*) {
        Weapon_Manager::GetInstance().Toggle_Weapon();
        Debug::D_Out << "[Event System] Weapon Toggle Requested" << std::endl;
        });

	// Event For Player Weapon Fire Request
    Event_Manager.Subscribe(EventType::Player_Fire_Requested, [](EventData* data) {
        auto* Weapon_Data = static_cast<Combat_Fire_Request_Data*>(data);
        if (Weapon_Data)
        {
            Weapon_Manager::GetInstance().Fire_Current_Weapon(Weapon_Data->Player_POS, Weapon_Data->Player_ATK);
            Debug::D_Out << "[Event System] Weapon Fire Requested" << std::endl;
        }
        });

    // ----------------------------------------------------------------------------------------------------------------
    //										    Enemy Combat Event Register
    // ----------------------------------------------------------------------------------------------------------------
    
    // Event For Enemy Fire
    Event_Manager.Subscribe(EventType::Enemy_Fired, [](EventData* data) {
        auto* E_Bullet_data = static_cast<Enemy_Fire_Event_Data*>(data);
        if (E_Bullet_data)
        {
            Bullet_Manager::GetInstance().Fire_Enemy_Bullet(
                E_Bullet_data->start_pos, E_Bullet_data->direction, E_Bullet_data->speed, E_Bullet_data->damage);
        }
        });
}
