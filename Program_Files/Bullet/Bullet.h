/*==============================================================================

    Manage Bullet Base [Bullet.h]

    Author : Choi HyungJoon

==============================================================================*/
#ifndef BULLET_H
#define BULLET_H
#include <DirectXMath.h>
#include "Collision.h"

// Bullet Type
enum class BulletType
{
    MACHINE_GUN,    
    MISSILE 
};

// Bullet Owner
enum class BulletOwner
{
    PLAYER,
    ENEMY
};

class Bullet
{
public:
    Bullet() = default;
    virtual ~Bullet() = default;

    // --- Pooling System ---
    virtual void Activate(const DirectX::XMFLOAT3& Start_Pos, const DirectX::XMFLOAT3& Dir, BulletOwner Owner, int Damage)
    {
        m_IsActive = true;
        m_Position = Start_Pos;
        m_Start_Position = Start_Pos;
        m_Owner = Owner;
        m_Damage = Damage;
        m_Alive_Timer = 0.0f;

        // Direction Normalize
        DirectX::XMVECTOR V_Dir = DirectX::XMLoadFloat3(&Dir);
        V_Dir = DirectX::XMVector3Normalize(V_Dir);
        DirectX::XMStoreFloat3(&m_Direction, V_Dir);
    }

    virtual void Deactivate()
    {
        m_IsActive = false;
    }

    bool IsActive() const { return m_IsActive; }

    // --- Main Logic ---
    virtual void Update(float Elapsed_Time) = 0;
    virtual void Draw() = 0;

protected:
    bool m_IsActive = false;

    DirectX::XMFLOAT3 m_Position = { 0.0f, 0.0f, 0.0f };        // Visual Current POS
    DirectX::XMFLOAT3 m_Start_Position = { 0.0f, 0.0f, 0.0f };  // Logical Start  POS
    DirectX::XMFLOAT3 m_Target_Position = { 0.0f, 0.0f, 0.0f }; // Logical Target POS
    DirectX::XMFLOAT3 m_Direction = { 0.0f, 0.0f, 1.0f };

    BulletOwner m_Owner = BulletOwner::PLAYER;

    int m_Damage = 10;
    float m_Visual_Speed = 0.0f;
    float m_Alive_Timer = 0.0f;
    float m_Destroy_Z_Axis = -10.0f;
};
#endif // BULLET_H