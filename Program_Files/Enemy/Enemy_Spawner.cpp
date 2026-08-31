/*==============================================================================
   
   Manage Enemy Spawning Logic [Enemy_Spawner.cpp]
   
   Author : Choi HyungJoon

==============================================================================*/
#include "Project_Header.h"
#include "Enemy_Spawner.h"
#include "Enemy_Manager.h"
#include "Player.h"
#include "debug_ostream.h"
#include "Heapler_Logic.h"
#include "Wave_Data.h"
#include "Player_Camera.h"
#include "Palette.h"
#include "Event_Manager.h"

using namespace DirectX;
using namespace PALETTE;

void Enemy_Spawner::Init()
{
    m_CooldownTimer = 0.0f;
    m_Current_Stage = 0;
    m_Is_Spawning_Done = false;
    m_Wave_Index = 0;
    m_Current_Stage_Waves.clear();
}

void Enemy_Spawner::Reset()
{
    Init();
}

void Enemy_Spawner::Start_Stage(int stage)
{
    Init();
    m_Current_Stage = stage;

    // Read Stage Info, And Load Stage Spwan Info
    auto it = Stage_Data_Map.find(stage);

    if (it != Stage_Data_Map.end())
    {
        m_Current_Stage_Waves = it->second;
    }
    else
    {
        Debug::D_Out << "[Enemy Spawner] Warning : Stage " << stage << " Data Not Found!" << std::endl;
    }
}

void Enemy_Spawner::Set_Z_Depth(float depth)
{
    m_Spawn_Base_Z = depth;
}

float Enemy_Spawner::Get_Z_Depth() const
{
    return m_Spawn_Base_Z;
}

void Enemy_Spawner::Update(float dt)
{
    if (m_Current_Stage <= 0 || m_Is_Spawning_Done) return;

    // Get Alive Enemy
    bool is_wave_cleared = (Enemy_Manager::GetInstance().Get_Active_Enemy_Count() == 0);

    // Wait For CoolDown
    if (m_CooldownTimer > 0.0f && !is_wave_cleared)
    {
        m_CooldownTimer -= dt;
        return;
    }

    // If Left Spawn Wave, Do Spawn
    if (m_Wave_Index < m_Current_Stage_Waves.size())
    {
        const Wave& current_wave = m_Current_Stage_Waves[m_Wave_Index];
        float max_cooldown = 0.0f;

        // Spawn All Pattern In Wave Index
        for (PatternTag p_tag : current_wave)
        {
            const Pattern_Info& pattern_info = Get_Pattern_Data(p_tag);

            // Use Most Longest Cool-Time
            max_cooldown = std::max(max_cooldown, pattern_info.Cooldown);

            // Spawn Logic
            for (size_t i = 0; i < pattern_info.Enemies.size(); ++i)
            {
                const auto& enemy_node = pattern_info.Enemies[i];
                XMFLOAT3 finalPos = Get_Spawn_Position(enemy_node.Type, enemy_node.X_Ratio);
                Enemy_Manager::GetInstance().Spawn(enemy_node.Type, finalPos);
            }
        }

        // Set CoolTime
        m_CooldownTimer = max_cooldown;
        m_Wave_Index++;
    }
    // Spawn Done All Wave, No More Enemy, Phase Done
    else if (is_wave_cleared && !m_Is_Spawning_Done)
    {
        m_Is_Spawning_Done = true;
        EventManager::GetInstance().Fire(EventType::Phase_Spawning_Done);
        Debug::D_Out << "[Enemy Spawner] Stage " << m_Current_Stage << " Spawning & Cleared Done!" << std::endl;
    }
}

XMFLOAT3 Enemy_Spawner::Get_Spawn_Position(EnemyType type, float ratio_X) const
{
    // 1. Get Enemy Type Info
    const Enemy_Info& Info = Get_Enemy_Info(type);

	// 2. Get X Axis : Range Is Based On World_Limit_X, But Can Be Adjusted By Ratio_X (0.0 ~ 1.0)
    float limitX = Get_Player_Limit_X();
    float minX = -limitX;
    float maxX = limitX;
    float totalWidth = maxX - minX;

    float spawnX = minX + (ratio_X * totalWidth);

	// 3. Get Y Axis : Get Y Ratio Each Enemy Type
    float limitY_Max = A_Half * Get_Player_Limit_Y_Max();
    float spawnY = limitY_Max * Info.Spawn_Y_Ratio;

	// 4. Get Z Axis : Get Z Ratio Each Enemy Type
    float spawnZ = m_Spawn_Base_Z * Info.Spawn_Z_Ratio;

    return XMFLOAT3(spawnX, spawnY, spawnZ);
}