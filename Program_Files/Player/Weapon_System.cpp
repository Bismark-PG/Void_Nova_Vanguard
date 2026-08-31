
/*==============================================================================

	Manage Weapon System Logic [Weapon_System.cpp]

	Author : Choi HyungJoon

==============================================================================*/
#include "Project_Header.h"
#include "Weapon_System.h"
#include "Player_Camera.h"
#include "Player.h"
#include "Enemy_Manager.h"
#include "Bullet_Manager.h"
#include "Heapler_Logic.h"
#include "Enemy_Spawner.h"
#include "Combat_Register.h"
#include "Event_Manager.h"

using namespace DirectX;

std::vector<LockOn_Data> Weapon_Manager::Locked_Targets;

void Weapon_Manager::Init()
{
}

void Weapon_Manager::Update(float dt)
{
	if (m_Fire_Cooldown > 0.0f)
		m_Fire_Cooldown -= dt;

	if (m_Current_Weapon == WeaponType::MISSILE)
	{
		Missile_Lock_On();
	}
	else
	{
		Missile_Lock_On_List_Clear();
	}
}

void Weapon_Manager::Missile_Lock_On()
{
	// If Enemy Is Dead, Clear In Target List
	auto it = std::remove_if(Locked_Targets.begin(), Locked_Targets.end(),
		[](const LockOn_Data& L) { return !L.Target_Ptr->IsActive() || L.Target_Ptr->GetUniqueID() != L.Target_ID; });
	Locked_Targets.erase(it, Locked_Targets.end());

	// Get Ray For Missile Target Pos
	XMFLOAT3 Cam_Pos = Player_Camera_Get_POS();
	XMVECTOR V_Cam = XMLoadFloat3(&Cam_Pos);
	XMVECTOR V_Aim = XMLoadFloat3(&Player_Get_Aim_POS());
	XMVECTOR V_RayDir = XMVector3Normalize(V_Aim - V_Cam);

	XMFLOAT3 RayDir;
	XMStoreFloat3(&RayDir, V_RayDir);

	if (RayDir.z > 0.0001f) // If Look Forward
	{
		for (Enemy* E : Enemy_Manager::GetInstance().Get_Active_List())
		{
			if (Locked_Targets.size() >= MAX_LOCK_ON) break; // Limit Max Size

			// Already Lock On, Do Pass
			bool already_locked = false;
			for (const LockOn_Data& L : Locked_Targets) {
				if (L.Target_ID == E->GetUniqueID()) { already_locked = true; break; }
			}
			if (already_locked) continue;

			// When Z Is Same To Enemy, Get Aim POS
			XMFLOAT3 E_Pos = E->GetPosition();
			float Hit_T = (E_Pos.z - Cam_Pos.z) / RayDir.z;
			float Hit_X = Cam_Pos.x + RayDir.x * Hit_T;
			float Hit_Y = Cam_Pos.y + RayDir.y * Hit_T;

			// If In Radius, Lock On
			float Lock_Radius = 4.0f; // Can Change Lock On Radius
			if (abs(Hit_X - E_Pos.x) < Lock_Radius && abs(Hit_Y - E_Pos.y) < Lock_Radius)
			{
				// Save Lock On Data With Enemy Pointer And Enemy ID
				Locked_Targets.push_back({ E, E->GetUniqueID() });
			}
		}
	}
}

void Weapon_Manager::Missile_Lock_On_List_Clear()
{
	Locked_Targets.clear();
}

void Weapon_Manager::Missile_Fire(float Damage)
{
	if (!Locked_Targets.empty())
	{
		// Shoot Lock On Target
		for (const LockOn_Data& L : Locked_Targets)
		{
			// Random Direction For Visual Effect
			float Random_X = RandomFloatMinus1To1() * Random_X_Ratio;
			float Random_Y = RandomFloatRange(Random_Y_Range_MIN, Random_Y_Range_MAX);
			XMFLOAT3 Start_Dir = { Random_X, Random_Y, Missile_Z_Start_POS };

			Bullet_Manager::GetInstance().Fire_Missile(Player_Get_POS(), Start_Dir, Damage * 3, L.Target_Ptr);
		}
		// End Shoot, Clear List
		Locked_Targets.clear();
	}
	else
	{
		// Shoot Ray In Aim POS
		XMFLOAT3 Cam_POS = Player_Camera_Get_POS();
		XMVECTOR V_Cam = XMLoadFloat3(&Cam_POS);
		XMVECTOR V_Aim = XMLoadFloat3(&Player_Get_Aim_POS());
		XMVECTOR V_Player = XMLoadFloat3(&Player_Get_POS());

		XMVECTOR V_Cam_To_Aim = XMVector3Normalize(V_Aim - V_Cam);

		float Target_Z = Enemy_Spawner::GetInstance().Get_Z_Depth();// Enemy Spawn Z
		float Dir_Z = XMVectorGetZ(V_Cam_To_Aim);
		if (abs(Dir_Z) < 0.0001f) Dir_Z = 0.0001f; // Safety Code For Division 0

		float Hit_Target = (Target_Z - Cam_POS.z) / Dir_Z;
		XMVECTOR V_Target3D = V_Cam + V_Cam_To_Aim * Hit_Target;

		XMVECTOR V_Dir = XMVector3Normalize(V_Target3D - V_Player);
		XMFLOAT3 Dir;
		XMStoreFloat3(&Dir, V_Dir);

		Bullet_Manager::GetInstance().Fire_Missile(Player_Get_POS(), Dir, Damage * 3, nullptr);
	}
}

void Weapon_Manager::Machine_Gun_Fire(XMVECTOR V_Player, float Damage)
{
	// Get Camera POS, AIM POS, And Shoot Ray In Aim POS
	XMFLOAT3 Cam_POS = Player_Camera_Get_POS();
	XMVECTOR V_Cam = XMLoadFloat3(&Cam_POS);
	XMVECTOR V_Aim = XMLoadFloat3(&Player_Get_Aim_POS());

	// Get Logical Direction From Camera To Aim
	XMVECTOR V_Logical_Dir = XMVector3Normalize(V_Aim - V_Cam);
	XMFLOAT3 Logical_Dir;
	XMStoreFloat3(&Logical_Dir, V_Logical_Dir);

	// Visual Start POS In Player POS
	XMFLOAT3 Visual_Start;
	XMStoreFloat3(&Visual_Start, V_Player);

	// Fire Ray
	Bullet_Manager::GetInstance().Fire_Ray(Visual_Start, Cam_POS, Logical_Dir, Damage);
}

void Weapon_Manager::Set_Missile_Random_POS(float X, float Y_Min, float Y_Max, float Z)
{
	Random_X_Ratio		= X;
	Random_Y_Range_MIN	= Y_Min;
	Random_Y_Range_MAX	= Y_Max;
	Missile_Z_Start_POS = Z;
}

const std::vector<LockOn_Data> Weapon_Manager::Return_Lock_On_List()
{
	return Locked_Targets;
}

void Weapon_Manager::Toggle_Weapon()
{
	// Weapon Toggle
	m_Current_Weapon = (m_Current_Weapon == WeaponType::MACHINE_GUN) ? WeaponType::MISSILE : WeaponType::MACHINE_GUN;
	Missile_Lock_On_List_Clear();

	// Fire Event For Weapon Change
	Combat_Weapon_Event_Data combat_data(m_Current_Weapon);
	EventManager::GetInstance().Fire(EventType::Player_Weapon_Changed, &combat_data);
}

void Weapon_Manager::Fire_Current_Weapon(const DirectX::XMFLOAT3& Player_Pos, float Damage)
{
	// If Cooldown Is Not Ready, Do Nothing
	if (m_Fire_Cooldown > 0.0f) return;

	XMVECTOR V_Player = XMLoadFloat3(&Player_Pos);

	if (m_Current_Weapon == WeaponType::MACHINE_GUN)
	{
		Machine_Gun_Fire(V_Player, Damage);
		m_Fire_Cooldown = 0.1f;		// Shoot For 10/Sec
	}
	else if (m_Current_Weapon == WeaponType::MISSILE)
	{
		Missile_Fire(Damage);
		m_Fire_Cooldown = 1.0f;		// Shoot For 1/Sec
	}

	// Register Combat Event For Weapon Fire
	Combat_Weapon_Event_Data combat_data(m_Current_Weapon);
	EventManager::GetInstance().Fire(EventType::Player_Fired, &combat_data);
}