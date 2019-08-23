#version 430

layout (location = 0) out vec3 textCol;
layout (location = 1) out vec3 textNorm;
layout (location = 2) out vec3 worldPos;
layout (location = 11) uniform bool hasT;
uniform sampler2D myTexture;

struct OutPut
{
	vec4 pos;
	vec4 fragPos;
	vec4 norm;
	vec2 uv;
	vec4 color;
	float time;
};
in OutPut input;


void main()
{
	vec4 newcolor;
	newcolor = input.color;

	if(hasT)
	{
		newcolor = texture(myTexture, input.uv);
	}
	//newcolor = newcolor;
	textCol = newcolor.xyz;
	textNorm = input.norm.xyz;
	worldPos = input.pos.xyz;
}