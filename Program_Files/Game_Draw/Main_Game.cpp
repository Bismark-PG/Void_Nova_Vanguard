/*==============================================================================

    Main Game Screen [Main_Game.cpp]

    Author : Choi HyungJoon

==============================================================================*/
#include "Project_Header.h"
#include "Main_Game.h"
#include "Grid.h"
#include "Cube.h"
#include "Player.h"
#include "Player_Camera.h"
#include "Light_Manager.h"
#include "direct3d.h"
#include "Billboard_Manager.h"
#include "Weapon_System.h"
#include "Enemy_Manager.h"
#include "Bullet_Manager.h"
#include "Particle_Manager.h"
#include "Map_Manager.h"
#include "Event_Manager.h"

using namespace DirectX;

Main_Game_Manager* Main_Game_Manager::GetIntance()
{
    static Main_Game_Manager GetInstance;
    return &GetInstance;
}

void Main_Game_Manager::In_Game_Initialize()
{
    Player_Initialize();
    Player_Camera_Initialize();
}

void Main_Game_Manager::In_Game_Finalize()
{
    Player_Camera_Finalize();
    Player_Finalize();
}

void Main_Game_Manager::In_Game_Reset()
{
    Player_Reset();
    Player_Camera_Reset();

    Weapon_Manager::GetInstance().Missile_Lock_On_List_Clear();
    Enemy_Manager::GetInstance().Reset();
    Bullet_Manager::GetInstance().Reset();
    Billboard_Manager::GetInstance().Reset();
}

void Main_Game_Manager::In_Game_Update(float elapsed_time)
{
    // Update Map
	Map_Manager::GetInstance().Update(elapsed_time);

	// Update Player
    Player_Camera_Update(elapsed_time);
    Player_Update(elapsed_time);

	// Update Game Logic
	Weapon_Manager::GetInstance().Update(elapsed_time);
    Enemy_Manager::GetInstance().Update(elapsed_time);
    Bullet_Manager::GetInstance().Update(elapsed_time);
    Billboard_Manager::GetInstance().Update(elapsed_time);
    Particle_Manager::GetInstance().Update(elapsed_time);

	// Update Global Light
    Light_Manager::GetInstance().Global_Light_Update(elapsed_time);
}

void Main_Game_Manager::In_Game_Draw()
{
    Direct3D_SetDepthEnable(true);
    Shader_Manager::GetInstance()->Begin3D();
    Light_Manager::GetInstance().Global_Light_Set_Up();

	// Draw Map   
    Map_Manager::GetInstance().Draw();
    
	// Draw In-Game Objects
    Enemy_Manager::GetInstance().draw();
    Bullet_Manager::GetInstance().Draw();

	// Draw Player
    Player_Draw();

    // Draw Effects
    Billboard_Manager::GetInstance().Draw();
    Particle_Manager::GetInstance().Draw();
}