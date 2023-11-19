struct VertexInput
{
    float4 pos : POSITION;
    float2 uv : TEXCOORD;
};

struct VertexOutput
{
    float4 svPos : SV_Position;
    float2 uv : TEXCOORD;
};

cbuffer Camera : register(b0)
{
    matrix view;
    matrix projection;
}

cbuffer Model : register(b1)
{
    matrix model;
}

VertexOutput main(VertexInput input)
{
    VertexOutput output;
    float4 pos = mul(mul(mul(model, input.pos), view), projection);
    output.svPos = mul(rtMat, pos);
    output.uv = input.uv;
    return output;
}