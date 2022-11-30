
struct VertexInput
{
	float2 pos : POSITION;
	float2 uv : TEXCOORD;
};

struct VertexOutput
{
	float4 svPos : SV_Position;
	float2 uv : TEXCOORD;
};

VertexOutput main(VertexInput input)
{
	VertexOutput output;
	output.svPos = float4(1, 1, 1, 1);
    output.svPos.xy = input.pos;
	output.uv = input.uv;
	return output;
}