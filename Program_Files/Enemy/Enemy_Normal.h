/*==============================================================================

	Enemy Normal Type Class [Enemy_Normal.h]

	Author : Choi HyungJoon

==============================================================================*/
#ifndef ENEMY_NORMAL_H
#define ENEMY_NORMAL_H
#include "Enemy.h"
#include "Texture_Manager.h"
#include "Billboard_Manager.h"
#include "Palette.h"
#include "Player.h"          
#include "Event_Manager.h"   
#include "Combat_Register.h"

static constexpr float Shoot_Z_Axis = 100.0f;

class Enemy_Normal : public Enemy
{
public:
    static int Normal_TexID;
    bool m_HasFired = false;
    float m_BulletSpeed = 100.0f;

    static void Initialize_Resource()
    {
        Normal_TexID = Texture_Manager::GetInstance()->GetID("Enemy_Normal");
    }

    virtual void Activate(const DirectX::XMFLOAT3& pos, EnemyType type, EvadeDirection Dir) override
    {
        Enemy::Activate(pos, type, Dir);
        m_Has_Fired = false;

        m_Is_Evading = false;
        m_Evade_Dir = Dir;
        m_Evade_Timer = 0.0f;

        m_Is_Shooting = false;
        m_Shoot_Count = 0;
        m_Shoot_Timer = 0.0f;
    }

	virtual void Update_Logic(float dt) override
    {
		// Check Evade Trigger
        if (!m_Is_Evading && Position.z <= m_Evade_Trigger_Z)
        {
            m_Is_Evading = true;
        }

        if (m_Is_Evading)
        {
            m_Evade_Timer += dt;
            float Ratio = std::min(m_Evade_Timer / m_Evade_Accel_Time, 1.0f);

            float Max_Speed = m_Info.Speed * 2.5f;
            float Current_speed = Max_Speed * Ratio;

            switch (m_Evade_Dir)
            {
            case EvadeDirection::Left:  Position.x -= Current_speed * dt; break;
            case EvadeDirection::Right: Position.x += Current_speed * dt; break;
            case EvadeDirection::Up:    Position.y += Current_speed * dt; break;
            case EvadeDirection::None:  Position.z -= Current_speed * dt; break;
            }

            float z_speed_ratio = 1.0f - (Ratio * 0.5f);
            Position.z -= m_Info.Speed * z_speed_ratio * dt;
        }
        else
        {
            //Normal Movement
            Position.z -= m_Info.Speed * dt;
        }

		// Shoot Trigger Check
        if (!m_Has_Fired && Position.z <= m_Shoot_Trigger_Z)
        {
            m_Has_Fired = true;
            m_Is_Shooting = true;

			// Get Difficulty Ratio
            float diff_ratio = Difficulty_Manager::GetInstance().Get_Difficulty_Ratio();

			// For Debug, Shoot Count is 2 * Difficulty Ratio
            m_Max_Shoot_Count = static_cast<int>(2 * diff_ratio);

			// For Debug, Damage is Bullet Damage * Difficulty Ratio
            m_Current_Damage = static_cast<int>(m_Info.Bullet_Damage * diff_ratio);
            if (m_Current_Damage < 1) m_Current_Damage = 1;
        }

		// Shooting Logic
        if (m_Is_Shooting)
        {
            m_Shoot_Timer -= dt;
            if (m_Shoot_Timer <= 0.0f)
            {
                // Get Enemy, Player Vector For Bullet
                DirectX::XMVECTOR V_Enemy = DirectX::XMLoadFloat3(&Position);
                DirectX::XMFLOAT3 Player_POS = Player_Get_POS();
                DirectX::XMVECTOR V_Player = DirectX::XMLoadFloat3(&Player_POS);

                // Get Distance
                DirectX::XMVECTOR V_Sub = DirectX::XMVectorSubtract(V_Player, V_Enemy);
                DirectX::XMVECTOR V_Dir = DirectX::XMVector3Normalize(V_Sub);

                DirectX::XMFLOAT3 Dir;
                DirectX::XMStoreFloat3(&Dir, V_Dir);

                // Fire Shoot Event
                Enemy_Fire_Event_Data fire_data(Position, Dir, m_BulletSpeed, m_Info.Bullet_Damage);
                EventManager::GetInstance().Fire(EventType::Enemy_Fired, &fire_data);

                m_Shoot_Count++;
				m_Shoot_Timer = m_Shoot_Interval; // Wait For Next Shoot

				// Shoot Count Check
                if (m_Shoot_Count >= m_Max_Shoot_Count)
                {
                    m_Is_Shooting = false;
                }
            }
        }
    }

    virtual void Draw() override
    {
        if (!m_IsActive) return;
        Billboard_Draw(Normal_TexID, Position, m_Info.Scale, m_Info.Scale,
            { 0.5f, 0.5f }, PALETTE::White ,Billboard_Facing::YAW_ROLL_ONLY);
    }

private:
    float m_Shoot_Trigger_Z = 100.0f; // Shoot Z Axis Ratio
    float m_Evade_Trigger_Z = 50.0f;  // Evade Z Axis Ratio
	float m_Evade_Accel_Time = 2.0f;  // Max Time For Evade Acceleration

    bool m_Has_Fired = false;
    bool m_Is_Evading = false;
	float m_Evade_Timer = 0.0f;       // Timer For Evade Acceleration
    EvadeDirection m_Evade_Dir = EvadeDirection::None;

    bool m_Is_Shooting = false;
    int m_Shoot_Count = 0;
    int m_Max_Shoot_Count = 2;
    float m_Shoot_Timer = 0.0f;
    float m_Shoot_Interval = 0.5f;
    int m_Current_Damage = 2;
};
#endif // ENEMY_NORMAL_H