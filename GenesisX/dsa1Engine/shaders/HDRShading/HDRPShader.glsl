#version 430

layout (location = 2) uniform float exposure;
layout (location = 3) uniform sampler2D HDR_Text;
layout (location = 4) uniform sampler2D Bloom_Text;

in vec2 TextCoord;
out vec4 color;

void main()
{
	vec3 newcolor;
	vec3 bloom;
	const float gamma = 2.2;

	newcolor = texture(HDR_Text, TextCoord).rgb;
	bloom = texture(Bloom_Text, TextCoord).rgb;

	newcolor += bloom;

	vec3 mapped = vec3(1.0) - exp(-newcolor * exposure);

	mapped = pow(mapped, vec3(1.0/gamma));
	color = vec4(mapped,1.0);
}