struct PixelInput
{
    float4 svPos : SV_Position;
    float2 uv : TEXCOORD;
};

float4 main(PixelInput input) : SV_TARGET
{
	return float4(input.uv, 0.0f, 1.0f);
}