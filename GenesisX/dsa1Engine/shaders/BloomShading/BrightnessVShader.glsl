#version 430

layout (location = 0) in vec2 position;
layout (location = 1) in vec2 texcoord;

out vec2 TextCoord;
void main()
{
	TextCoord = texcoord;
	gl_Position = vec4(position.x,position.y,0.0f,1.0f);	
}
