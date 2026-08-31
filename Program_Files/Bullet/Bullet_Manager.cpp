/*==============================================================================

    Manage Bullet Spawn System [Bullet_Manager.cpp]

    Author : Choi HyungJoon

==============================================================================*/
#include "Project_Header.h"
#include "Bullet_Manager.h"
#include "Bullet_Ray.h"
#include "Bullet_Missile.h"
#include "Bullet_Enemy.h":

void Bullet_Manager::Init()
{
    m_Ray_Pool.clear();
    m_Missile_Pool.clear();
    m_Active_List.clear();
    m_Enemy_Pool.clear();

    for (int i = 0; i < MAX_RAY_POOL; ++i)
    {
        m_Ray_Pool.push_back(new Bullet_Ray());
    }

    for (int i = 0; i < MAX_MISSILE_POOL; ++i)
    {
        m_Missile_Pool.push_back(new Bullet_Missile());
    }

    for (int i = 0; i < MAX_ENEMY_BULLET_POOL; ++i)
    {
        m_Enemy_Pool.push_back(new Bullet_Enemy());
    }
}

void Bullet_Manager::Final()
{
    for (Bullet* B : m_Ray_Pool) delete B;
    for (Bullet* B : m_Missile_Pool) delete B;
    for (Bullet* B : m_Enemy_Pool) delete B;

    m_Ray_Pool.clear();
    m_Missile_Pool.clear();
    m_Active_List.clear();
    m_Enemy_Pool.clear();
}

void Bullet_Manager::Reset()
{
    for (Bullet* B : m_Ray_Pool)
    {
        if (B->IsActive()) B->Deactivate();
    }

    for (Bullet* B : m_Missile_Pool)
    {
        if (B->IsActive()) B->Deactivate();
    }

    for (Bullet* B : m_Enemy_Pool)
    {
        if (B->IsActive()) B->Deactivate();
    }

    m_Active_List.clear();
}

void Bullet_Manager::Update(float Elapsed_Time)
{
    m_Active_List.clear();

    for (Bullet* B : m_Ray_Pool)
    {
        if (B->IsActive())
        {
            B->Update(Elapsed_Time);
            m_Active_List.push_back(B);
        }
    }

    for (Bullet* B : m_Missile_Pool)
    {
        if (B->IsActive())
        {
            B->Update(Elapsed_Time);
            m_Active_List.push_back(B);
        }
    }

    for (Bullet* B : m_Enemy_Pool)
    {
        if (B->IsActive())
        {
            B->Update(Elapsed_Time);
            m_Active_List.push_back(B);
        }
    }
}

void Bullet_Manager::Draw()
{
    for (Bullet* B : m_Active_List)
    {
        B->Draw();
    }
}

void Bullet_Manager::Fire_Ray(const XMFLOAT3& Visual_Start, const XMFLOAT3& Logical_Start, const XMFLOAT3& Logical_Dir, int Damage)
{
    for (Bullet* B : m_Ray_Pool)
    {
        if (!B->IsActive())
        {
            static_cast<Bullet_Ray*>(B)->Activate_Ray(Visual_Start, Logical_Start, Logical_Dir, Damage);
            return;
        }
    }
}

void Bullet_Manager::Fire_Missile(const XMFLOAT3& Start_Pos, const XMFLOAT3& Dir, int Damage, Enemy* Target)
{
    for (Bullet* B : m_Missile_Pool)
    {
        if (!B->IsActive())
        {
            static_cast<Bullet_Missile*>(B)->Activate_Missile(Start_Pos, Dir, Damage, Target);
            return;
        }
    }
}

void Bullet_Manager::Fire_Enemy_Bullet(const XMFLOAT3& Start_Pos, const XMFLOAT3& Dir, float speed, int Damage)
{
    for (Bullet* B : m_Enemy_Pool)
    {
        if (!B->IsActive())
        {
            static_cast<Bullet_Enemy*>(B)->Activate_Enemy_Bullet(Start_Pos, Dir, speed, Damage);
            return;
        }
    }
}
