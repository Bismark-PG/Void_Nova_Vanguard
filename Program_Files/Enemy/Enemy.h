/*==============================================================================

	Manage Enemy Base [Enemy.h]

	Author : Choi HyungJoon

==============================================================================*/
#ifndef ENEMY_H
#define ENEMY_H
#include <DirectXMath.h>
#include "Collision.h"
#include "Enemy_Type.h"

class Enemy
{
public:
	Enemy() = default;
	virtual ~Enemy() = default;

	// --- Pulling System ---
	virtual void Activate(const DirectX::XMFLOAT3& pos, EnemyType type, EvadeDirection Dir);
	virtual void Deactivate();
	bool IsActive() const { return m_IsActive; }

	// --- Main Logic ---
	void Update(float elapsed_time);
	virtual void Update_Logic(float dt) = 0;

	// --- Rendering ---
	static void Initialize_Resource();
	virtual void Draw();

	// --- System ---
	virtual void OnDamage(int damage);
	virtual void OnHit();
	bool IsDead() const { return m_HP <= 0; }
	AABB GetAABB() const;

	// --- Getter ---
	int GetUniqueID() const { return m_UniqueID; }
	DirectX::XMFLOAT3 GetPosition() const { return Position; }
	EnemyType GetType() const { return m_Info.Type; }

	static int s_Flight_Normal_TexID;

protected:
	// --- Drop Resource ---
	void Drop_Reward();

	// --- State ---
	bool m_IsActive = false;
	Enemy_Info m_Info;
	int m_HP = 100;

	// --- Transform ---
	DirectX::XMFLOAT3 Position = { 0.0f, 0.0f, 0.0f };

	// --- ID System ---
	static int Global_ID_Counter;
	int m_UniqueID = -1;
};

#endif // ENEMY_H