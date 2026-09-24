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
    Reset();

    Wave_Data_Manager::GetInstance().Load_Wave_CSV("Resource/Data/Wave_Data.csv");
}

void Enemy_Spawner::Reset()
{
    m_CooldownTimer = 0.0f;
    m_Current_Stage = 0;
    m_Is_Spawning_Done = false;
    m_Wave_Index = 0;
    m_Current_Stage_Waves.clear();
}

void Enemy_Spawner::Set_Stage(int stage)
{
    m_Current_Stage = stage;
}

void Enemy_Spawner::Start_Stage(int stage)
{
    Reset();
    Set_Stage(stage);

    // Read Stage Info, And Load Stage Spwan Info
    const Stage_Waves* waves = Wave_Data_Manager::GetInstance().Get_Stage_Waves(stage);

    if (waves)
    {
        m_Current_Stage_Waves = *waves;
    }
    else
    {
        Debug::D_Out << "[Enemy Spawner] Warning : Stage " << stage << " Data Not Found in CSV!" << std::endl;
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

void Enemy_Spawner::Add_Spawner_Cooldown(float time)
{
    m_CooldownTimer += time;
}

void Enemy_Spawner::Update(float dt)
{
    if (m_Current_Stage <= 0 || m_Is_Spawning_Done) return;

    // Get Alive Enemy
    bool Is_Wave_Cleared = (Enemy_Manager::GetInstance().Get_Active_Enemy_Count() == 0);

    // Wait For CoolDown
    if (m_CooldownTimer > 0.0f && !Is_Wave_Cleared)
    {
        m_CooldownTimer -= dt;
        return;
    }

    // If Left Spawn Wave, Do Spawn
    if (m_Wave_Index < m_Current_Stage_Waves.size())
    {
        const Wave& Current_Wave = m_Current_Stage_Waves[m_Wave_Index];
        float Max_Cooldown = 0.0f;
        bool Is_End_Triggered = false;

        // Spawn All Pattern In Wave Index
        for (int Pattern_Num : Current_Wave)
        {
			// If Pattern Number is 999, Trigger End Phase
            if (Pattern_Num == 999)
            {
                Is_End_Triggered = true;
                break;
            }
			// If Pattern Number is 0, Trigger Boss Phase
            else if (Pattern_Num == 0)
            {
                Debug::D_Out << "[Enemy Spawner] BOSS Phase Triggered!" << std::endl;
				// Need Boss Spawn Logic
				Max_Cooldown = std::max(Max_Cooldown, 10.0f); // Set Cooldown For Boss Phase (Debug, 10 Sec)
                continue;
            }

			// Spawn Logic For Each Pattern
            PatternTag Pattern_T = static_cast<PatternTag>(Pattern_Num - 1);
            const Pattern_Info& Pattern_Info = Get_Pattern_Data(Pattern_T);
            Max_Cooldown = std::max(Max_Cooldown, Pattern_Info.Cooldown);

            for (size_t i = 0; i < Pattern_Info.Enemies.size(); ++i)
            {
                const auto& Enemy_Node = Pattern_Info.Enemies[i];
                XMFLOAT3 POS = Get_Spawn_Position(Enemy_Node.Type, Enemy_Node.X_Ratio);
                Enemy_Manager::GetInstance().Spawn(Enemy_Node.Type, POS, Enemy_Node.E_Dir);
            }
        }
        
        // Wave Done
        if (Is_End_Triggered)
        {
			// If All Enemy Cleared, Trigger Phase Done Event
            if (Is_Wave_Cleared)
            {
                m_Is_Spawning_Done = true;
                EventManager::GetInstance().Fire(EventType::Phase_Spawning_Done);
                Debug::D_Out << "[Enemy Spawner] Stage " << m_Current_Stage << " Spawning & Cleared Done!" << std::endl;
            }
            return;
        }

        // Set CoolTime
        m_CooldownTimer = Max_Cooldown;
        m_Wave_Index++;
    }
    // Spawn Done All Wave, No More Enemy, Phase Done
    else if (Is_Wave_Cleared && !m_Is_Spawning_Done)
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