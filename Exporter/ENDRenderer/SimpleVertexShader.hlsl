#pragma pack_matrix(row_major)
#define JOINTCOUNT 57

cbuffer cb : register (b0)
{
	float4x4 world;
	float4x4 view;
	float4x4 proj;
	float4x4 m[JOINTCOUNT];
};

struct Input
{
	float4 pos : POSITION;
	float3 normal : NORMAL;
	float2 tex : TEXCOORD;
	float4 indices : INDICES;
	float4 weights : WEIGHTS;
	float4 tangent : TANGENT;
};

struct Output
{
	float4 pos : SV_POSITION;
	float3 normal : NORMAL;
	float2 tex : TEXCOORD;
	float4 world : POSITION;
	float4 tangent : TANGENT;
};

Output main(Input input)
{
	Output output;
	float4 skinned_pos = float4(0, 0, 0, 0);
	float4 skinned_norm = float4(0, 0, 0, 0);

	for (int i = 0; i < 4; i++)
	{
		skinned_pos += mul(float4(input.pos.xyz, 1.0f), m[input.indices[i]]) * input.weights[i];
		skinned_norm += mul(float4(input.normal.xyz, 0.0f), m[input.indices[i]]) * input.weights[i];
	}

	output.pos = mul(skinned_pos, world);
	output.world = output.pos;
	output.pos = mul(output.pos, view);
	output.pos = mul(output.pos, proj);

	output.normal = normalize(mul(skinned_norm, world)).xyz;

	output.tex = input.tex;

	//output.tangent = normalize(mul(input.tangent, world));
	//output.binormal = normalize(mul(input.binormal, world));
	output.tangent = input.tangent;
	//output.binormal = input.binormal;

	return output;
}