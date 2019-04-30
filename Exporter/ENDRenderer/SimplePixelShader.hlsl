cbuffer cb : register (b0)
{
	float4 vLightPos;
	float4 cameraWorld;
}

Texture2D baseTexture : register(t0);
Texture2D specularTexture : register(t1);
Texture2D emissiveTexture : register(t2);
Texture2D BumpMapTexture : register(t3);

SamplerState filter : register(s0);

struct Input
{
	float4 pos : SV_POSITION;
	float3 normal : NORMAL;
	float2 tex : TEXCOORD;
	float4 world : POSITION;
	float4 tangent : TANGENT;
};
float3 CalcBumpedNormal(Input input);

float4 main(Input input) : sv_target
{
	//input.normal = CalcBumpedNormal(input);
	float4 ambient = { 0.25f,  0.25f, 0.25f, 1};
	float4 finalColor = 0;
	float4 directional = saturate(dot(normalize(-vLightPos.xyz), input.normal.xyz));

	float4 diffuse = baseTexture.Sample(filter, input.tex);
	float4 emissive = emissiveTexture.Sample(filter, input.tex);

	float3 vLightDir = normalize(input.world - vLightPos).xyz;
	float3 vReflect = reflect(input.normal.xyz, vLightDir).xyz;
	float3 vToCam = normalize(input.world - cameraWorld).xyz;
	float fSpecDot = saturate(dot(vToCam, vReflect));
	float fSpecPow = pow(fSpecDot, 16);
	float4 specular = specularTexture.Sample(filter, input.tex) * fSpecPow;

	finalColor = diffuse * (directional + specular + emissive + ambient);

	return finalColor;
}

float3 CalcBumpedNormal(Input input)
{
	/*float3 BumpMap = BumpMapTexture.Sample(filter, input.tex).rgb;
	BumpMap = 2 * BumpMap - 1;
	float3x3 TBN = float3x3(input.tangent.xyz, input.binormal.xyz, input.normal);
	BumpMap = mul(TBN, BumpMap);
	return BumpMap;*/

	/*float3 Normal = normalize(input.normal);
	float3 Tangent = normalize(input.tangent);
	Tangent = normalize(Tangent - dot(Tangent, Normal) * Normal);
	float3 Bitangent = cross(Tangent, Normal);
	float3 BumpMapNormal = BumpMapTexture.Sample(filter, Tangent).xyz;
	BumpMapNormal = 2.0 * BumpMapNormal - float3(1.0, 1.0, 1.0);
	float3 NewNormal;
	float3x3 TBN = float3x3(Tangent, Bitangent, Normal);
	NewNormal = mul(TBN, BumpMapNormal);
	NewNormal = normalize(NewNormal);
	return NewNormal;*/
}