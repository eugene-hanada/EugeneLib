#version 450


layout(location = 0) in vec2 inPos;
layout(location = 1) in vec2 inUV;

layout(location = 0) out vec2 outputUV;

layout(push_constant) uniform RtMatrix {
	mat4 matrix;
} rt;

layout(set=0, binding=1) uniform TexMatrix
{
	mat4 matrix;
} tex;

void main()
{
	vec4 tmp = rt.matrix * tex.matrix * vec4(inPos, 0.0,1.0);
	tmp.y = -tmp.y;
	gl_Position = tmp;
	outputUV = inUV;
}