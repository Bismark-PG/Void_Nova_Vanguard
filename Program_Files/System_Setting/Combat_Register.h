/*==============================================================================

	Sound Event Register For Event System [Combat_Register.h]

	Author : Choi HyungJoon

==============================================================================*/
#ifndef COMBAT_REGISTER_H
#define COMBAT_REGISTER_H

#include <DirectXMath.h>
#include "Event_Types.h"
#include "Weapon_System.h"

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