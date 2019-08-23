#version 430

layout (location = 3) in vec3 pos;
layout (location = 1) uniform mat4 view;
layout (location = 2) uniform mat4 proj;


out vec3 TexCoord;


void main()
{
		TexCoord = pos;
		vec4 verPos = (proj * view) * vec4(pos,1.0);
		gl_Position = 	verPos.xyww;
}
