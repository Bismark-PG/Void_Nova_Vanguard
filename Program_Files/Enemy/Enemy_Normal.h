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
        Normal_TexID = Texture_Manager::GetInstance()->GetID("Enemy");
    }

    virtual void Activate(const DirectX::XMFLOAT3& pos, EnemyType type) override
    {
        Enemy::Activate(pos, type);
        m_HasFired = false;
    }

	virtual void Update_Logic(float dt) override
    {
        Position.z -= m_Info.Speed * dt;

        if (!m_HasFired && Position.z <= Shoot_Z_Axis)
        {
            m_HasFired = true;

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
        }
    }

    virtual void Draw() override
    {
        if (!m_IsActive) return;
        Billboard_Draw(Normal_TexID, Position, m_Info.Scale, m_Info.Scale,
            { 0.5f, 0.5f }, PALETTE::White ,Billboard_Facing::YAW_ROLL_ONLY);
    }
};

#endif // ENEMY_NORMAL_H