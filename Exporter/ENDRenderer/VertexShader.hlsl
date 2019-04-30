#pragma pack_matrix(row_major)

cbuffer cb
{
	float4x4 world;
	float4x4 view;
	float4x4 proj;
};

struct VSin
{
	float3 position : POSITION;
	float3 color: COLOR;
};

struct VSout
{
	float4 position : SV_POSITION;
	float3 color : COLOR;
};

VSout main(VSin input)
{
	VSout output;
	output.position = mul(float4(input.position, 1), world);
	output.position = mul(output.position, view);
	output.position = mul(output.position, proj);
	//output.normal = input.normal;
	output.color = input.color;
	return output;
}