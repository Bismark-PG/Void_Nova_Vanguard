/*==============================================================================

    Manage Bullet For Enemy [Bullet_Enemy.h]

    Author : Choi HyungJoon

==============================================================================*/
#ifndef BULLET_ENEMY_H
#define BULLET_ENEMY_H
#include "Bullet.h"
#include "Billboard.h"
#include "Texture_Manager.h"
using namespace DirectX;

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
    }

    virtual void Update(float Elapsed_Time) override
    {
        if (!m_IsActive) return;

        DirectX::XMVECTOR V_Current_Pos = DirectX::XMLoadFloat3(&m_Position);
        DirectX::XMVECTOR V_Dir = DirectX::XMLoadFloat3(&m_Direction);

        V_Current_Pos += V_Dir * m_Visual_Speed * Elapsed_Time;
        DirectX::XMStoreFloat3(&m_Position, V_Current_Pos);

        if (m_Position.z <= m_Destroy_Z_Axis)
        {
            Deactivate();
        }
    }

    virtual void Draw() override
    {
        if (!m_IsActive) return;
        int texID = Texture_Manager::GetInstance()->GetID("Debug_Circle");

        Billboard_Draw(texID, m_Position, 0.5f, 0.5f, { 0.5f, 0.5f }, { 1.0f, 0.5f, 0.0f, 1.0f });
    }
};
#endif // BULLET_ENEMY_H