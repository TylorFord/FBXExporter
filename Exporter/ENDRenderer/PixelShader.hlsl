


struct PSin
{
	float4 position : SV_POSITION;
	float3 color : COLOR;
};

float4 main(PSin input) : SV_TARGET
{
	return float4(input.color, 1);
}
