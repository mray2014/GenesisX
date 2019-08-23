#version 430

layout (location = 2) uniform sampler2D Fin_Text;

in vec2 TextCoord;
out vec4 brightnessColor;

void main()
{
	vec3 newcolor = texture(Fin_Text, TextCoord).rgb;

	float bright= dot(newcolor.rgb, vec3(0.2126,0.7152,0.0722));
	brightnessColor = vec4(0,0,0,1.0);
	if(bright > 2.0f)
	{
		brightnessColor = vec4(newcolor.rgb,1.0);
		//brightnessColor = vec4(0.0,0.0,0.0,1.0);
	}
}