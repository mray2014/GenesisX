#version 430

layout (location = 3) in struct Vertex
{
	vec3 position;
	vec2 uv;
	vec3 normal;
} Input;
layout (location = 6) uniform vec4 myColor;
layout (location = 7) uniform mat4 modelWorld;
layout (location = 8) uniform mat4 view;
layout (location = 9) uniform mat4 proj;
layout (location = 10) uniform float t;

struct OutPut
{
    vec4 pos;
	vec4 fragPos;
	vec4 norm;
	vec2 uv;
	vec4 color;
	float time;
};
out OutPut input;


void main()
{
		mat4 worldViewProj = proj * view * modelWorld;
		vec4 norm = vec4(Input.normal, 0);

		input.pos = modelWorld * vec4(Input.position, 1);
		input.fragPos = worldViewProj * vec4(Input.position, 1);
		input.norm = modelWorld*norm;
		input.norm = normalize(input.norm);
		input.uv = Input.uv;
		input.color = myColor;
		input.time = t;

		gl_Position = worldViewProj*vec4(Input.position, 1);	
}
