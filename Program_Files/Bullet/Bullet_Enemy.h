/*==============================================================================

    Manage Bullet For Enemy [Bullet_Enemy.h]

    Author : Choi HyungJoon

==============================================================================*/
#ifndef BULLET_ENEMY_H
#define BULLET_ENEMY_H
#include "Bullet.h"
#include "Billboard.h"
#include "Texture_Manager.h"

class Bullet_Enemy : public Bullet
{
public:
    Bullet_Enemy() = default;
    virtual ~Bullet_Enemy() = default;

    void Activate_Enemy_Bullet(const DirectX::XMFLOAT3& Start_Pos, const DirectX::XMFLOAT3& Dir, float speed, int Damage)
    {
        Bullet::Activate(Start_Pos, Dir, BulletOwner::ENEMY, Damage);
        m_Visual_Speed = speed;
        m_Alive_Timer = 0.0f;
        m_Radius = 0.5f;
    }

    virtual void Update(float Elapsed_Time) override
    {
        if (!m_IsActive) return;

        DirectX::XMVECTOR V_Current_Pos = DirectX::XMLoadFloat3(&m_Position);
        DirectX::XMVECTOR V_Dir = DirectX::XMLoadFloat3(&m_Direction);

        Prev_Z = m_Position.z;

        V_Current_Pos += V_Dir * m_Visual_Speed * Elapsed_Time;
        DirectX::XMStoreFloat3(&m_Position, V_Current_Pos);

        // Check Bullet POS Just One Time
        if (Prev_Z > 0.0f && m_Position.z <= 0.0f)
        {
            if (!Player_Is_Dead())
            {
				// Check AABB Collision With Player
                if (Collision_Is_Hit_AABB(GetAABB(), Player_Get_AABB()).Is_Hit)
                {
                    // Player Gets Damage
                    Player_Damaged(m_Damage);
                    Debug::D_Out << "[Enemy Bullet] Bullet Was Hit Player, Player Get Damage : " << m_Damage << std::endl;

                    // Hit Effect Event Fire
                    Combat_Hit_Event_Data hit_data(m_Position);
                    EventManager::GetInstance().Fire(EventType::Visual_Hit_Effect, &hit_data);

                    Deactivate();
                    return;
                }
            }
        }

		// Over Z Axis From Player, Deactivate Bullet
        if (m_Position.z <= m_Destroy_Z_Axis)
        {
            Deactivate();
        }
    }

    virtual void Draw() override
    {
        if (!m_IsActive) return;
        int texID = Texture_Manager::GetInstance()->GetID("Bullet_Enemy_Normal");

        Billboard_Draw(texID, m_Position, 0.5f, 0.5f, { 0.5f, 0.5f }, { 1.0f, 0.5f, 0.0f, 1.0f });
    }

private:
	float Prev_Z = 0.0f;
};
#endif // BULLET_ENEMY_H