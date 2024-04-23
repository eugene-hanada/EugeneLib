
struct VertexInput
{
	float4 pos : POSITION;
	float2 uv : TEXCOORD;
    uint normal : NORMAL;
};

struct VertexOutput
{
	float4 svPos : SV_Position;
	float2 uv : TEXCOORD;
    float3 normal : NORMAL;
};

cbuffer RenderTargetMat : register(b0)
{
    matrix rtMat;
}

cbuffer TextureMat : register(b1)
{
	matrix texMat;
}

VertexOutput main(VertexInput input)
{
	VertexOutput output;
    float4 pos = mul(texMat, input.pos);
    output.svPos = mul(rtMat, pos);
	output.uv = input.uv;
    output.normal.x = input.normal & 0xff;
	return output;
}