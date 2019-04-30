#pragma pack_matrix(row_major)

cbuffer MVP_t
{
	matrix modeling;
	matrix view;
	matrix projection;
};

struct VSIn
{
	uint vertexId : SV_VertexID;
};

struct VSOut
{
	float4 pos : SV_POSITION;
	float4 normal : NORMAL;
};

static const float3 cube_n[6] =
{
	float3(1.0f, 0.0f, 0.0f),
	float3(0.0f, 1.0f, 0.0f),
	float3(0.0f, 0.0f, 1.0f),
	float3(-1.0f, 0.0f, 0.0f),
	float3(0.0f, -1.0f, 0.0f),
	float3(0.0f, 0.0f, -1.0f)
};

static const float3 cube_v[36] =
{
	float3(0.5f,-0.5f,0.5f),
	float3(0.5f,0.5f,0.5f),
	float3(0.5f,-0.5f,-0.5f),
	float3(0.5f,0.5f,-0.5f),
	float3(0.5f,-0.5f,-0.5f),
	float3(0.5f,0.5f,0.5f),

	float3(0.5f,0.5f,-0.5f),
	float3(-0.5f,0.5f, 0.5f),
	float3(-0.5f,0.5f,-0.5f),
	float3(-0.5f,0.5f, 0.5f),
	float3(0.5f,0.5f,-0.5f),
	float3(0.5f,0.5f,0.5f),

	float3(-0.5f,-0.5f,0.5f),
	float3(-0.5f, 0.5f,0.5f),
	float3(0.5f,0.5f,0.5f),
	float3(-0.5f,-0.5f,0.5f),
	float3(0.5f,0.5f,0.5f),
	float3(0.5f,-0.5f, 0.5f),

	float3(-0.5f,0.5f,0.5f),
	float3(-0.5f,-0.5f,0.5f),
	float3(-0.5f,-0.5f,-0.5f),
	float3(-0.5f,0.5f,-0.5f),
	float3(-0.5f,0.5f,0.5f),
	float3(-0.5f,-0.5f,-0.5f),

	float3(0.5f,-0.5f,-0.5f),
	float3(-0.5f,-0.5f,-0.5f),
	float3(-0.5f,-0.5f, 0.5f),
	float3(-0.5f,-0.5f, 0.5f),
	float3(0.5f,-0.5f,0.5f),
	float3(0.5f,-0.5f,-0.5f),

	float3(-0.5f,-0.5f,-0.5f),
	float3(0.5f,0.5f,-0.5f),
	float3(-0.5f, 0.5f,-0.5f),
	float3(-0.5f,-0.5f,-0.5f),
	float3(0.5f,-0.5f, -0.5f),
	float3(0.5f,0.5f,-0.5f)
};

VSOut main(VSIn input)
{
	VSOut output;

	float4 vert_pos = float4(cube_v[input.vertexId], 1.0f);
	float4 vert_norm = float4(cube_n[input.vertexId/6], 0.0f);

	output.pos = mul( vert_pos, modeling);
	output.pos = mul(output.pos, view);
	output.pos = mul(output.pos, projection);

	output.normal = mul(vert_norm , modeling);
	output.normal = mul(output.normal, view);

	return output;
}
	