/*==============================================================================
    
    Manage Enemy Spawning Logic [Enemy_Spawner.h]

    Author : Choi HyungJoon

==============================================================================*/
#ifndef ENEMY_SPAWNER_H
#define ENEMY_SPAWNER_H

#include <vector>
#include <DirectXMath.h>
#include "Enemy_Type.h"
#include "Wave_Data.h"

class Enemy_Spawner
{
public:
    static Enemy_Spawner& GetInstance()
    {
        static Enemy_Spawner instance;
        return instance;
    }

    void Init();
    void Update(float dt);
    void Reset();

    void Start_Stage(int stage);

    void Set_Z_Depth(float depth);
    float Get_Z_Depth() const;

private:
    Enemy_Spawner() = default;
    ~Enemy_Spawner() = default;
    Enemy_Spawner(const Enemy_Spawner&) = delete;
    Enemy_Spawner& operator=(const Enemy_Spawner&) = delete;

    DirectX::XMFLOAT3 Get_Spawn_Position(EnemyType type, float ratio_X) const;

    // Spawner Info
    float m_Spawn_Base_Z = 150.0f;
    float m_CooldownTimer = 0.0f;

    // Stage Info
    int m_Current_Stage = 0;
    bool m_Is_Spawning_Done = false;

    // Stage Wave Info
    std::vector<Wave> m_Current_Stage_Waves;    // Copy Index For Wave List
    size_t m_Wave_Index = 0;                    // For Current Wave List
};
#endif // ENEMY_SPAWNER_H