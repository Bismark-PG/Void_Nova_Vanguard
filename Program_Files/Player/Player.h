/*==============================================================================

	Manage Player Logic [Player.h]

	Author : Choi HyungJoon

==============================================================================*/
#ifndef PLAYER_H
#define PLAYER_H
#include "Collision.h"
#include "Weapon_System.h"

void Player_Initialize();
void Player_Finalize();

void Player_Update(float elapsed_time);

void Player_Reset();
void Player_Draw();

// --- Parameter Setter  ---
void Player_Set_POS(const DirectX::XMFLOAT3& POS);
void Player_Damaged(int damage);

// --- Parameter Geter  ---
const DirectX::XMFLOAT3& Player_Get_POS();
const DirectX::XMFLOAT3& Player_Get_Aim_POS();
float Player_Get_HP();
float Player_Get_MaxHP();

// --- Debug Function  ---
void GUI_Set_Player_POS(float x, float y, float z);
void GUI_Set_Player_Health(float hp, float max);
void GUI_Set_Player_Stats(float atk, float def);

#endif // PLAYER_H