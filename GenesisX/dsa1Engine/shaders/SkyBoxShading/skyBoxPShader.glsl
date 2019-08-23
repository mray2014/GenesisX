#version 430

uniform samplerCube sky;

in vec3 TexCoord;
out vec4 color;


void main()
{
	color = texture(sky, TexCoord);
	//color = vec4(1,0,0,1);
}