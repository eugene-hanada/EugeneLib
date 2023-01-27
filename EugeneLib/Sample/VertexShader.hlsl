
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

cbuffer RenderTargetMat : register(b0)
{
    matrix rtMat;
}

VertexOutput main(VertexInput input)
{
	VertexOutput output;
    output.svPos = mul(rtMat, input.pos);
	output.uv = input.uv;
	return output;
}