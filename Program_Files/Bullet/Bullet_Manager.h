/*==============================================================================

    Manage Bullet Spawn System [Bullet_Manager.h]

    Author : Choi HyungJoon

==============================================================================*/
#ifndef BULLET_MANAGER_H
#define BULLET_MANAGER_H
#include <vector>
#include <DirectXMath.h>
#include "Bullet.h"

class Bullet_Manager
{
public:
    static Bullet_Manager& GetInstance()
    {
        static Bullet_Manager instance;
        return instance;
    }

    void Init();
    void Final();
    void Reset();

    void Update(float Elapsed_Time);
    void Draw();

    // Player Fire
    void Fire_Ray(const DirectX::XMFLOAT3& Visual_Start, const DirectX::XMFLOAT3& Logical_Start, const DirectX::XMFLOAT3& Logical_Dir, int Damage);
    void Fire_Missile(const DirectX::XMFLOAT3& Start_Pos, const DirectX::XMFLOAT3& Dir, int Damage, class Enemy* Target);

    // Enemy Fire
    void Fire_Enemy_Bullet(const DirectX::XMFLOAT3& Start_Pos, const DirectX::XMFLOAT3& Dir, float speed, int Damage);

private:
    Bullet_Manager() = default;
    ~Bullet_Manager() = default;
    Bullet_Manager(const Bullet_Manager&) = delete;
    Bullet_Manager& operator=(const Bullet_Manager&) = delete;

    static constexpr int MAX_RAY_POOL = 100;
    static constexpr int MAX_MISSILE_POOL = 50;
    static constexpr int MAX_ENEMY_BULLET_POOL = 300;

    std::vector<Bullet*> m_Ray_Pool;
    std::vector<Bullet*> m_Missile_Pool;
    std::vector<Bullet*> m_Active_List;

    std::vector<Bullet*> m_Enemy_Pool;
};
#endif // BULLET_MANAGER_H