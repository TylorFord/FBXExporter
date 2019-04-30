struct VSOut
{
	float4 pos : SV_POSITION;
	float4 normal : NORMAL;
};

float4 main(VSOut input) : SV_TARGET
{
	return input.normal;
}