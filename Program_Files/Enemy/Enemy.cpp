/*==============================================================================

	Manage Enemy Base [Enemy.cpp]

	Author : Choi HyungJoon

==============================================================================*/
#include "Project_Header.h"
#include "Enemy.h"
#include "Billboard.h"        
#include "Billboard_Manager.h" 
#include "Audio_Manager.h"
#include "Damage_Text_Manager.h"
#include "Enemy_Normal.h"
using namespace DirectX;

int Enemy::Global_ID_Counter = 0;
int Enemy_Normal::Normal_TexID = -1;

void Enemy::Activate(const XMFLOAT3& pos, EnemyType type)
{
	m_IsActive = true;
	Position = pos;

	m_UniqueID = ++Global_ID_Counter;

	switch (type)
	{
	case EnemyType::ENEMY_NORMAL:	m_Info = Enemy_Normal_Info;	break;
	}

	m_HP = m_Info.MaxHP;
}

void Enemy::Deactivate()
{
	if (!m_IsActive) return;
	m_IsActive = false;
}

void Enemy::Update(float elapsed_time)
{
	if (!m_IsActive) return;

	Update_Logic(elapsed_time);

	if (Position.z < -20.0f)
	{
		Deactivate();
	}
}

void Enemy::Initialize_Resource()
{
	Enemy_Normal::Initialize_Resource();
}

void Enemy::Draw()
{
	if (!m_IsActive) return;
}

void Enemy::OnDamage(int damage)
{
	if (m_HP <= 0 || !m_IsActive) return;

	int FinalDamage = (damage < 1) ? 1 : damage;
	m_HP -= FinalDamage;

	DirectX::XMFLOAT3 spawnPos = Position;
	spawnPos.y += m_Info.Scale * 0.8f;
	Damage_Text_Manager::GetInstance().Spawn_Damage(spawnPos, FinalDamage, { 1.0f, 0.2f, 0.2f, 1.0f }, 1.5f);

	if (m_HP <= 0)
	{
		OnHit();
	}
}

void Enemy::OnHit()
{
	// Audio_Manager::GetInstance()->Play_SFX("Enemy_Dead");

	float effectScale = m_Info.Scale * 0.6f;
	Billboard_Manager::GetInstance().Create_Effect(Position, effectScale, Effect_Type::EXPLOSION);

	Drop_Reward();
	Deactivate();
}

AABB Enemy::GetAABB() const
{
	float radius = m_Info.Scale * 0.5f;
	float depth  = radius;

	return AABB
	{
		{ Position.x + radius, Position.y + m_Info.Scale, Position.z + depth },
		{ Position.x - radius, Position.y               , Position.z - depth }
	};
}

void Enemy::Drop_Reward()
{
}