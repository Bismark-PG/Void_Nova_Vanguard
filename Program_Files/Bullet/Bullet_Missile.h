/*==============================================================================

    Manage Bullet For Lock-On Missile Type [Bullet_Missile.h]

    Author : Choi HyungJoon

==============================================================================*/
#ifndef BULLET_MISSILE_H
#define BULLET_MISSILE_H
#include "Bullet.h"
#include "Enemy.h"
#include "Enemy_Manager.h"
#include "Particle_Manager.h"
#include "Billboard.h"        
#include "Texture_Manager.h"
#include "Frame.h"
using namespace DirectX;

class Bullet_Missile : public Bullet
{
public:
    Bullet_Missile() = default;
    virtual ~Bullet_Missile() = default;

    // For Target Point And ID For Missile Activate
    void Activate_Missile(const XMFLOAT3& Start_Pos, const XMFLOAT3& Dir, int Damage, Enemy* Target)
    {
        Bullet::Activate(Start_Pos, Dir, BulletOwner::PLAYER, Damage);
        m_Target = Target;
        m_TargetID = (Target != nullptr) ? Target->GetUniqueID() : -1;

        m_Visual_Speed = 60.0f; 
        m_Turn_Speed = 5.0f;   
        m_Hit_Enemy = false;

        if (m_Target != nullptr)
        {
            XMVECTOR V_Start = XMLoadFloat3(&Start_Pos);
            XMFLOAT3 Target_Pos = Target->GetPosition();
            XMVECTOR V_Target = XMLoadFloat3(&Target_Pos);

            m_Initial_Distance = XMVectorGetX(XMVector3Length(V_Target - V_Start));
        }
        else
        {
            m_Initial_Distance = 0.0f;
        }
    }

    virtual void Update(float Elapsed_Time) override
    {
        if (!m_IsActive) return;

        XMVECTOR V_Current_Pos = XMLoadFloat3(&m_Position);
        XMVECTOR V_Dir = XMLoadFloat3(&m_Direction);
        float Current_Speed = m_Visual_Speed;

        // If Target Alive And Same ID, Get Chase
        if (m_Target != nullptr && m_Target->IsActive() && m_Target->GetUniqueID() == m_TargetID)
        {
            XMFLOAT3 Target_Pos = m_Target->GetPosition();
            XMVECTOR V_Target_Pos = XMLoadFloat3(&Target_Pos);

            // Get 3D-World Distance
            float Current_Distance = XMVectorGetX(XMVector3Length(V_Target_Pos - V_Current_Pos));

            // Get Ratio
            float Ratio = 0.0f;
            if (m_Initial_Distance > 0.0f)
            {
                Ratio = 1.0f - (Current_Distance / m_Initial_Distance);
                if (Ratio < 0.0f) Ratio = 0.0f;
                if (Ratio > 1.0f) Ratio = 1.0f;
            }

            // Dynamic Proportional Navigation Guidance Setting With Ratio
            Current_Speed = m_Visual_Speed * (1.0f + (Ratio * 2.0f));       // Speed : 1 ~ 3
            float Dynamic_Turn = m_Turn_Speed * (1.0f + (Ratio * 7.0f));    // Turn  : 1 ~ 8

            // When Missile Is Close Enemy, Set Visual Effect
            if (Current_Distance <= (Current_Speed * Elapsed_Time * 1.5f))
            {
                m_Target->OnDamage(m_Damage);
                // Need Effect For Hit Enemy
                Deactivate();
                return;
            }

            /*
            // Get Distance Z Axis Only
			float Z_Diff = abs(Target_Pos.z - m_Position.z);

            // If Close Enough, Hit Enemy
            if (Z_Diff <= (m_Visual_Speed * Elapsed_Time * static_cast<float>(Frame_Rate::Three_Frame)))
            {
                m_Target->OnDamage(m_Damage);
				// Need Effect For Hit Enemy
                Deactivate();
                return;
			}
            */

            // Lerp To Target POS
            XMVECTOR V_To_Target = XMVector3Normalize(V_Target_Pos - V_Current_Pos);
            V_Dir = XMVector3Normalize(XMVectorLerp(V_Dir, V_To_Target, m_Turn_Speed * Elapsed_Time));
            XMStoreFloat3(&m_Direction, V_Dir);
        }

        // Movement Accept
        V_Current_Pos += V_Dir * m_Visual_Speed * Elapsed_Time;
        XMStoreFloat3(&m_Position, V_Current_Pos);
        
        // AABB Setting
        AABB Bullet_AABB = {
            { m_Position.x + 0.5f, m_Position.y + 0.5f, m_Position.z + 0.5f },
            { m_Position.x - 0.5f, m_Position.y - 0.5f, m_Position.z - 0.5f }
        };

        Enemy* Hit_Enemy = Enemy_Manager::GetInstance().Check_Collision_AABB(Bullet_AABB);
        if (Hit_Enemy != nullptr)
        {
            Hit_Enemy->OnDamage(m_Damage);
            // Need Effect For Hit Enemy
            Deactivate();
            return;
        }

        m_Alive_Timer += Elapsed_Time;

        if (m_Alive_Timer >= 5.0f) Deactivate(); // Auto Deactivate
    }

    virtual void Draw() override
    {
        if (!m_IsActive) return;

        int texID = Texture_Manager::GetInstance()->GetID("Debug_Circle");
        Billboard_Draw(texID, m_Position, 1.5f, 1.5f, { 0.5f, 0.5f }, { 0.0f, 0.5f, 1.0f, 1.0f });
    }

private:
    Enemy* m_Target = nullptr;
    int m_TargetID = -1;
    float m_Turn_Speed = 5.0f;
    bool m_Hit_Enemy = false;
    float m_Initial_Distance = 0.0f;
};
#endif // BULLET_MISSILE_H