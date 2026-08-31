/*==============================================================================

    Manage Enemy Spawn System [Enemy_Manager.cpp]

    Author : Choi HyungJoon

==============================================================================*/
#include "Project_Header.h"
#include "Enemy_Manager.h"
#include "Enemy_Spawner.h"
#include "Enemy_Normal.h"

static constexpr int POOL_SIZE = 200;

void Enemy_Manager::Init()
{
    m_EnemyPool.clear();
    m_ActiveList.clear();

	Enemy::Initialize_Resource();

    Enemy_Spawner::GetInstance().Init();

    for (int i = 0; i < POOL_SIZE; ++i)
    {
        m_EnemyPool.push_back(new Enemy_Normal());
    }
}
void Enemy_Manager::Final()
{
    for (Enemy* e : m_EnemyPool)
    {
        delete e;
    }

    m_EnemyPool.clear();
    m_ActiveList.clear();
}

void Enemy_Manager::Update(double elapsed_time)
{
	float dt = static_cast<float>(elapsed_time);

    m_ActiveList.clear();

	Enemy_Spawner::GetInstance().Update(dt);

    for (Enemy* e : m_EnemyPool)
    {
        if (e->IsActive())
        {
            e->Update(dt);
            m_ActiveList.push_back(e);
        }
    }
}

void Enemy_Manager::Reset()
{
    m_ActiveList.clear();

    Enemy_Spawner::GetInstance().Reset();

    for (Enemy* e : m_EnemyPool)
    {
        if (e->IsActive())
        {
            e->Deactivate();
        }
    }
}

void Enemy_Manager::draw()
{
	for (Enemy* e : m_ActiveList)
	{
		e->Draw();
	}
}

void Enemy_Manager::Spawn(EnemyType type, const DirectX::XMFLOAT3& spawnPos)
{
    for (Enemy* e : m_EnemyPool)
    {
        if (!e->IsActive())
        {
            e->Activate(spawnPos, type);
            return;
        }
    }
}

int Enemy_Manager::Get_Active_Enemy_Count() const
{
    int count = 0;

    for (const auto& enemy : m_EnemyPool)
    {
        if (enemy->IsActive()) count++;
    }
    return count;
}

Enemy* Enemy_Manager::Check_Collision_AABB(const AABB& box)
{
    for (Enemy* e : m_ActiveList)
    {
        if (Collision_Is_Hit_AABB(box, e->GetAABB()).Is_Hit)
            return e;
    }
    return nullptr;
}