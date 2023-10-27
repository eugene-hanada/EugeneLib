#version 450

layout(location = 0) in vec2 inUV;

layout(set=1, binding=0) uniform texture2D tex;
layout(set=2 ,binding = 0) uniform sampler  smp;


layout(location = 0) out vec4 outColor;

void main() {
	outColor = texture( sampler2D(tex, smp) ,inUV);
	outColor  = float4(1.0f,0.0f,0.0f,1.0f)
}