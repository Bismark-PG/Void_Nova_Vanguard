/*==============================================================================

	Make Billboard [Billboard.cpp]

	Author : Choi HyungJoon

==============================================================================*/
#include "Project_Header.h"
#include "Billboard.h"
#include "direct3d.h"
#include "Texture_Manager.h"
#include "Shader_Manager.h"
#include "Player_Camera.h"
#include "Sprite_Animation.h"
#include "Palette.h"
using namespace DirectX;
using namespace PALETTE;

static constexpr int NUM_VERTEX = 4;

static Microsoft::WRL::ComPtr<ID3D11Buffer> Vertex_Buffer = nullptr;

struct Vertex_Billboard
{
	XMFLOAT3 position;
	XMFLOAT4 color;   
	XMFLOAT2 texcord; 
};

void Billboard_Initialize()
{
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(Vertex_Billboard) * NUM_VERTEX;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	Direct3D_GetDevice()->CreateBuffer(&bd, nullptr, Vertex_Buffer.GetAddressOf());
}

void Billboard_Finalize()
{
	Vertex_Buffer.Reset();
}

void Billboard_Draw_Internal(int Tex_ID, const XMFLOAT3& POS, float Scale_X, float Scale_Y,
	const XMFLOAT2& Pivot, const UV_Parameter& uvParam, const XMFLOAT4& Color, Billboard_Facing facing)
{
	// Set Billboard Color
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	if (SUCCEEDED(Direct3D_GetContext()->Map(Vertex_Buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		Vertex_Billboard* v = (Vertex_Billboard*)mappedResource.pData;
		v[0] = { { 0.0f, 1.0f, 0.0f }, Color, { 0.0f, 0.0f } }; // LU
		v[1] = { { 1.0f, 1.0f, 0.0f }, Color, { 1.0f, 0.0f } }; // RU
		v[2] = { { 0.0f, 0.0f, 0.0f }, Color, { 0.0f, 1.0f } }; // LD
		v[3] = { { 1.0f, 0.0f, 0.0f }, Color, { 1.0f, 1.0f } }; // RD
		Direct3D_GetContext()->Unmap(Vertex_Buffer.Get(), 0);
	}

	// Set UV
	Shader_Manager::GetInstance()->SetUVParameter(uvParam);
	Shader_Manager::GetInstance()->Begin_Billboard();

	// Set Color
	Shader_Manager::GetInstance()->SetDiffuseColor(Color);

	// Set Texture
	ID3D11ShaderResourceView* SRV = Texture_Manager::GetInstance()->Get_Shader_Resource_View(Tex_ID);
	if (SRV)
	{
		Direct3D_GetContext()->PSSetShaderResources(0, 1, &SRV);
	}

	// Set Vertex Buffer
	UINT stride = sizeof(Vertex_Billboard);
	UINT offset = 0;
	Direct3D_GetContext()->IASetVertexBuffers(0, 1, Vertex_Buffer.GetAddressOf(), &stride, &offset);

	// Set Topology
	Direct3D_GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// Set Offset
	XMMATRIX Pivot_Offset = XMMatrixTranslation(-Pivot.x, -Pivot.y, 0.0f);
	// Set Scale Matrix
	XMMATRIX Scale = XMMatrixScaling(Scale_X, Scale_Y, 1.0f);

	float Cam_Pitch = Get_Player_Camera_Pitch();
	float Cam_Yaw	= Get_Player_Camera_Yaw();
	float Cam_Roll	= Get_Player_Camera_Roll();

	XMMATRIX Rotation_Matrix = XMMatrixIdentity();

	switch (facing)
	{
	case Billboard_Facing::ALL_AXIS:
		// Agree All Rotation
		Rotation_Matrix = XMMatrixRotationRollPitchYaw(Cam_Pitch, Cam_Yaw, Cam_Roll);
		break;

	case Billboard_Facing::YAW_ROLL_ONLY:
		// Ignore Pitch Rotation
		Rotation_Matrix = XMMatrixRotationRollPitchYaw(0.0f, Cam_Yaw, Cam_Roll);
		break;

	case Billboard_Facing::FLOOR:
		Rotation_Matrix = XMMatrixRotationX(XMConvertToRadians(90.0f));
		break;

	case Billboard_Facing::FIXED_NONE:
		// Do Nothing
		break;
	}

	// Get Scale And Translation
	XMMATRIX Trans = XMMatrixTranslation(POS.x, POS.y, POS.z);

	// Get Final mtx
	XMMATRIX mtxWorld = Pivot_Offset * Scale * Rotation_Matrix * Trans;

	// Set World mtx
	Shader_Manager::GetInstance()->SetWorldMatrix3D(mtxWorld);

	// Draw
	Direct3D_GetContext()->Draw(NUM_VERTEX, 0);
}

void Billboard_Draw(int Tex_ID, const XMFLOAT3& POS, float Scale_X, float Scale_Y,
	const XMFLOAT2& Pivot, const XMFLOAT4& Color, Billboard_Facing facing)
{
	// Set UV
	UV_Parameter UV = {};
	UV.scale = { 1.0f, 1.0f };
	UV.translation = { 0.0f, 0.0f };

	Billboard_Draw_Internal(Tex_ID, POS, Scale_X, Scale_Y, Pivot, UV, Color, facing);
}

void Billboard_Draw_Animation(int PlayID, const XMFLOAT3& POS, float Scale_X, float Scale_Y,
	const XMFLOAT4& Color, Billboard_Facing facing)
{
	int texID = -1;
	UV_Parameter UV = {};

	if (SpriteAni_Get_Current_UV(PlayID, texID, UV.scale, UV.translation))
	{
		Billboard_Draw_Internal(texID, POS, Scale_X, Scale_Y, { 0.5f, 0.5f }, UV, Color, facing);
	}
}