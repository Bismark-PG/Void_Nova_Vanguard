/*==============================================================================

	Make Billboard [Billboard.h]

	Author : Choi HyungJoon

==============================================================================*/
#ifndef BILLBOARD_H
#define BILLBOARD_H

#include "Shader_Manager.h"

enum class Billboard_Facing
{
	ALL_AXIS,       // For Player
	YAW_ROLL_ONLY,  // For Enemy
	FLOOR,			// For Ground
	FIXED_NONE      // For Sky
};

void Billboard_Initialize();
void Billboard_Finalize();

void Billboard_Draw_Internal(int Tex_ID, const DirectX::XMFLOAT3& POS, float Scale_X, float Scale_Y,
	const DirectX::XMFLOAT2& Pivot, const UV_Parameter& uvParam, const DirectX::XMFLOAT4& Color,
	Billboard_Facing facing);

void Billboard_Draw(int Tex_ID, const DirectX::XMFLOAT3& POS, float Scale_X, float Scale_Y,
	const DirectX::XMFLOAT2& Pivot, const DirectX::XMFLOAT4& Color = { 1.0f, 1.0f, 1.0f, 1.0f },
	Billboard_Facing facing = Billboard_Facing::ALL_AXIS);

void Billboard_Draw_Animation(int PlayID, const DirectX::XMFLOAT3& POS, float Scale_X, float Scale_Y,
	const DirectX::XMFLOAT4& Color = { 1.0f, 1.0f, 1.0f, 1.0f },
	Billboard_Facing facing = Billboard_Facing::ALL_AXIS);

#endif // BILLBOARD_H
