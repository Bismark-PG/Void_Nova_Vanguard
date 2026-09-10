/*==============================================================================

	Sound Event Register For Event System [Combat_Register.h]

	Author : Choi HyungJoon

==============================================================================*/
#ifndef COMBAT_REGISTER_H
#define COMBAT_REGISTER_H

#include <DirectXMath.h>
#include "Event_Types.h"
#include "Weapon_System.h"

// Enum For Game Difficulty
enum class GameDifficulty
{
    Easy,
    Normal,
    Hard
};

// Class For Manage Difficulty
class Difficulty_Manager
{
public:
	static Difficulty_Manager& GetInstance()
	{
		static Difficulty_Manager instance;
		return instance;
	}

	// Control Difficulty
	void Set_Difficulty(GameDifficulty diff) { m_Difficulty = diff; }
	GameDifficulty Get_Difficulty() const { return m_Difficulty; }
	float Get_Difficulty_Ratio() const
	{
		switch (m_Difficulty)
		{
		case GameDifficulty::Easy:   return 0.5f;
		case GameDifficulty::Normal: return 1.0f;
		case GameDifficulty::Hard:   return 2.0f;
		}

		return 1.0f; // Default
	}

private:
	Difficulty_Manager() = default;
	~Difficulty_Manager() = default;

	Difficulty_Manager(const Difficulty_Manager&) = delete;
	Difficulty_Manager& operator=(const Difficulty_Manager&) = delete;

	GameDifficulty m_Difficulty = GameDifficulty::Normal;
};

//----------------------------------------------------------------

// Weapon Change Event Data
struct Combat_Weapon_Event_Data : public EventData
{
    WeaponType Weapon_Type;

    Combat_Weapon_Event_Data(WeaponType type) : Weapon_Type(type) {}
};

// Player Fire Event Data
struct Combat_Hit_Event_Data : public EventData
{
    DirectX::XMFLOAT3 Hit_POS;

    Combat_Hit_Event_Data(const DirectX::XMFLOAT3& pos) : Hit_POS(pos) {}
};

// Player Fire Request Event Data
struct Combat_Fire_Request_Data : public EventData
{
    DirectX::XMFLOAT3 Player_POS;
    float Player_ATK;

    Combat_Fire_Request_Data(const DirectX::XMFLOAT3& pos, float atk)
        : Player_POS(pos), Player_ATK(atk) {
    }
};

// Enemy Fire Event Data
struct Enemy_Fire_Event_Data : public EventData
{
    DirectX::XMFLOAT3 start_pos;
    DirectX::XMFLOAT3 direction;
    float speed;
    int damage;

    Enemy_Fire_Event_Data(const DirectX::XMFLOAT3& p, const DirectX::XMFLOAT3& d, float s, int dmg)
        : start_pos(p), direction(d), speed(s), damage(dmg) {
    }
};

#endif // COMBAT_REGISTER_H