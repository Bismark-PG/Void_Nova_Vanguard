/*==============================================================================

	Manage Enemy Type [Enemy_Type.h]

	Author : Choi HyungJoon

==============================================================================*/
#ifndef ENEMY_TYPE_H
#define ENEMY_TYPE_H
enum class EnemyType
{
	ENEMY_NORMAL,
};

enum class EvadeDirection
{
	Left,
	Right,
	Up,
	None
};

struct Enemy_Info
{
	EnemyType Type;
	int HP, MaxHP;
	float Speed;
	float Scale;
	float Bullet_Size, Bullet_Speed;

	int Collision_Damage;
	int Bullet_Damage;   

	float Spawn_Y_Ratio; // Ratio For Ground Or Sky
	float Spawn_Z_Ratio; // Ratio For Base Z (Far Or Near)
};

// --- Normal Type--- 
static const Enemy_Info Enemy_Normal_Info
{	// Enemy Type
	EnemyType::ENEMY_NORMAL,
	// HP, Max_HP, Speed, Scale, Bullet_Size, Bullet_Speed
	10, 10, 25.0f, 3.0f, 2.5f, 10.0f,
	//Collision_Damage, Bullet_Damage
	1, 2,
	// Spawn_Y_Ratio
	0.8f,
	// Spawn_Z_Ratio
	1.0f
};

inline const Enemy_Info& Get_Enemy_Info(EnemyType type)
{
	Enemy_Info Info;

	switch (type)
	{
	case EnemyType::ENEMY_NORMAL: 
		Info = Enemy_Normal_Info;
		break;
		// Need More Enemy Type
	}

	return Info;
}

#endif // ENEMY_TYPE_H