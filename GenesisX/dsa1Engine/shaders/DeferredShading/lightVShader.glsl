#version 430

layout (location = 0) uniform mat4 worldMat;
layout (location = 1) uniform mat4 projMat;
layout (location = 2) uniform mat4 viewMat;
layout (location = 3) in vec3 pos;
layout (location = 4) in vec2 uv;
layout (location = 5) in vec3 nor;


void main()
{
	mat4 PVW = projMat * viewMat * worldMat;
	gl_Position = PVW * vec4(pos,1.0f);	
}
