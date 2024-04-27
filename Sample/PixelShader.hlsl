struct PixelInput
{
    float4 svPos : SV_Position;
    float2 uv : TEXCOORD;
};

Texture2D<float4> tex : register(t0);

SamplerState sam : register(s0);

float4 main(PixelInput input) : SV_TARGET
{
    float4 color = tex.Sample(sam, input.uv);
    return color;
}