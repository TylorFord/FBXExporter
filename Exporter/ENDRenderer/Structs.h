#pragma once

struct Skinned_vert
{
	DirectX::XMFLOAT4 pos;
	DirectX::XMFLOAT3 norm;
	DirectX::XMFLOAT2 tex_coord;
	DirectX::XMFLOAT4 indices;
	DirectX::XMFLOAT4 weights;

	DirectX::XMFLOAT4 tangents;
	//DirectX::XMFLOAT4 binormals;
};

struct Simple_vert
{
	DirectX::XMFLOAT4 pos;
	DirectX::XMFLOAT3 norm;
	DirectX::XMFLOAT2 tex_coord;
	DirectX::XMFLOAT4 indices;
	DirectX::XMFLOAT4 weights;

	DirectX::XMFLOAT4 tangents;
	DirectX::XMFLOAT4 binormals;
};

struct Skinned_mesh
{
	uint32_t vert_count = 0;
	uint32_t index_count = 0;
	Skinned_vert* verts = nullptr;
	uint32_t* indices = nullptr;
};

struct Simple_mesh
{
	uint32_t vert_count = 0;
	uint32_t index_count = 0;
	Simple_vert* verts = nullptr;
	uint32_t* indices = nullptr;
};

struct psCB
{
	XMFLOAT4 lightPos;
	XMFLOAT4 cameraPos;
};

struct Quarternions
{
	DirectX::XMFLOAT4 pos;
	DirectX::XMFLOAT4 rotation;	
	int parent_index;
};

struct KeyFrame
{
	double time;
	Quarternions* joints;
};

struct AnimClip
{
	double duration;
	KeyFrame* frames;
};