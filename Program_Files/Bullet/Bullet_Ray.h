/*==============================================================================

    Manage Bullet Ray-Marching Type [Bullet_Ray.h]

    Author : Choi HyungJoon

==============================================================================*/
#ifndef BULLET_RAY_H
#define BULLET_RAY_H
#include "Bullet.h"
#include "Enemy_Manager.h"
#include "Player.h"
#include "Player_Camera.h"
#include "Billboard.h"          
#include "Texture_Manager.h"
#include "Event_Manager.h"
#include "Combat_Register.h"
using namespace DirectX;

class Bullet_Ray : public Bullet
{
public:
    Bullet_Ray() = default;
    virtual ~Bullet_Ray() = default;

    void Activate_Ray(const XMFLOAT3& Visual_Start, const XMFLOAT3& Logical_Start, const XMFLOAT3& Logical_Dir, int Damage)
    {
        // Base Setup
        Bullet::Activate(Visual_Start, Logical_Dir, BulletOwner::PLAYER, Damage);

        m_Max_Range     = 150.0f;
        m_Step_Size     = 0.5f;
        m_Visual_Speed  = 150.0f;
        m_Hit_Enemy     = false;
        m_Hit_Wall      = false;

		// Get Hit Point Immediately using Ray Marching (Logical Ray-Marching)
        Calculate_Hit_Point(Logical_Start, Logical_Dir);

		// Set Direction from Start to Target (Visual)
        XMVECTOR V_Vis_Start = XMLoadFloat3(&Visual_Start);
        XMVECTOR V_Target = XMLoadFloat3(&m_Target_Position);
        XMVECTOR V_New_Vis_Dir = XMVector3Normalize(V_Target - V_Vis_Start);
        XMStoreFloat3(&m_Direction, V_New_Vis_Dir);
    }

    virtual void Update(float Elapsed_Time) override
    {
        if (!m_IsActive) return;

        // --- Visual Bullet Movement ---
        XMVECTOR V_Current_Pos = XMLoadFloat3(&m_Position);
        XMVECTOR V_Dir = XMLoadFloat3(&m_Direction);
        XMVECTOR V_Target_Pos = XMLoadFloat3(&m_Target_Position);

        V_Current_Pos += V_Dir * m_Visual_Speed * Elapsed_Time;
        XMStoreFloat3(&m_Position, V_Current_Pos);

		// Check If Visual Bullet Reached Target Point
        float Dist_To_Target = XMVectorGetX(XMVector3Length(V_Target_Pos - V_Current_Pos));

        if (Dist_To_Target <= (m_Visual_Speed * Elapsed_Time))
        {
            Spawn_Hit_Effect();
            Deactivate();
        }

        m_Alive_Timer += Elapsed_Time;
        if (m_Alive_Timer >= 10.0f)
        {
            Deactivate();
        }
    }

    virtual void Draw() override
    {
        if (!m_IsActive) return;

        int texID = Texture_Manager::GetInstance()->GetID("Debug_Circle");
        Billboard_Draw(texID, m_Position, 0.2f, 0.2f, { 0.5f, 0.5f }, { 1.0f, 0.8f, 0.0f, 1.0f });
    }

private:
    float m_Max_Range = 150.0f;
    float m_Step_Size = 0.5f;
    bool m_Hit_Enemy = false;
    bool m_Hit_Wall = false;

	// Ray Casting To Calculate Hit Point Immediately Upon Firing
    void Calculate_Hit_Point(const XMFLOAT3& Start_P, const XMFLOAT3& Dir_V)
    {
        XMVECTOR Ray_Pos = XMLoadFloat3(&Start_P);
        XMVECTOR Ray_Dir = XMLoadFloat3(&Dir_V);
        float Now_Dist = 0.0f;

		// Default Target Is The End Of The Ray (In Case No Collision)
        XMVECTOR V_End_Pos = Ray_Pos + Ray_Dir * m_Max_Range;
        XMStoreFloat3(&m_Target_Position, V_End_Pos);

        // It AABB Box Will Be Bigger Than Step Size For Empty Space
        float Check_Radius = m_Step_Size * 0.8f;

        while (Now_Dist < m_Max_Range)
        {
            XMFLOAT3 Check_Pos;
            XMStoreFloat3(&Check_Pos, Ray_Pos);

            AABB Ray_AABB = {
                { Check_Pos.x + Check_Radius, Check_Pos.y + Check_Radius, Check_Pos.z + Check_Radius },
                { Check_Pos.x - Check_Radius, Check_Pos.y - Check_Radius, Check_Pos.z - Check_Radius }
            };

			// If Player's Bullet, Check Enemy Collision
            if (m_Owner == BulletOwner::PLAYER)
            {
                Enemy* Hit_Enemy = Enemy_Manager::GetInstance().Check_Collision_AABB(Ray_AABB);
                if (Hit_Enemy != nullptr)
                {
					// Give Damage To Enemy
                    Hit_Enemy->OnDamage(m_Damage);
                    m_Hit_Enemy = true;
                    XMStoreFloat3(&m_Target_Position, Ray_Pos);
                    break;
                }
            }

            // Move To Next Step (Bullet Position)
            Ray_Pos += Ray_Dir * m_Step_Size;
            Now_Dist += m_Step_Size;
        }
    }

    void Spawn_Hit_Effect() const
    {
        if (m_Hit_Enemy || m_Hit_Wall)
        {
            Combat_Hit_Event_Data hit_data(m_Target_Position);
            EventManager::GetInstance().Fire(EventType::Visual_Hit_Effect, &hit_data);
        }
    }
};
#endif // BULLET_RAY_H