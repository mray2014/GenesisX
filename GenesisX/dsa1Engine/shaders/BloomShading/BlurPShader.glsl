#version 430

layout (location = 2) uniform sampler2D Bloom_Text;
layout (location = 3) uniform bool horizontal;
//uniform float weight[5] = float[] (0.227027,0.1945946, 0.1216216, 0.054054, 0.016216);
//uniform float weight[5] = float[] (0.054054, 0.016216, 0.008054, 0.004216, 0.001216);
//uniform float weight[7] = float[] (0.307027,0.2245946, 0.103054, 0.004216, 0.000216, 0.000216, 0.000216);
//uniform float weight[7] = float[] (0.307027,0.1645946, 0.103054, 0.004216, 0.000216, 0.000216, 0.000216);
//uniform float weight[6] = float[] (0.307027,0.1645946, 0.103054, 0.004216, 0.000216, 0.000216);
//uniform float weight[5] = float[] (0.307027,0.1645946, 0.103054, 0.004216, 0.000216);
uniform float weight[5] = float[] (0.307027,0.1645946, 0.103054, 0.009216, 0.001016);

in vec2 TextCoord;
out vec4 color;

void main()
{
	vec2 tex_Offset = 1.0 / textureSize(Bloom_Text,0);
	vec3 newcolor = texture(Bloom_Text, TextCoord).rgb * weight[0];
	if(horizontal)
	{
		for(int i = 1; i < 5; ++i)
		{
			newcolor += texture(Bloom_Text, TextCoord + vec2(tex_Offset.x * i, 0.0)).rgb * weight[i];
			newcolor += texture(Bloom_Text, TextCoord - vec2(tex_Offset.x * i, 0.0)).rgb * weight[i];
		}
	}
	else
	{
		for(int i = 1; i < 5; ++i)
			{
				newcolor += texture(Bloom_Text, TextCoord + vec2(tex_Offset.y * i, 0.0)).rgb * weight[i];
				newcolor += texture(Bloom_Text, TextCoord - vec2(tex_Offset.y * i, 0.0)).rgb * weight[i];
			}
	}

	color = vec4(newcolor,1.0);
	//color = vec4(1,0,0,1);
}