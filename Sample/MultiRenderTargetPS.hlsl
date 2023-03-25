struct PixelInput
{
    float4 svPos : SV_Position;
    float2 uv : TEXCOORD;
};

struct PsOutput
{
    float4 color1 : SV_Target0;
    float4 color2 : SV_Target1;
};

Texture2D<float4> tex : register(t0);

SamplerState sam : register(s0);

PsOutput main(PixelInput input)
{
    PsOutput output;
    output.color1 = tex.Sample(sam, input.uv);
    output.color2 = float4(input.uv, 1.0f, 1.0f);
    return output;
}