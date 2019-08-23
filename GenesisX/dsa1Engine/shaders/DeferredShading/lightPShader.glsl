#version 430
	struct Att{
		float Constant;
		float Linear;
		float Exp;
	};
struct Light{
	vec3 Position;
	vec3 Color;
	float AmbientIntensity;
	float DiffuseIntensity;
	Att Atten;	 
};

layout (location = 7) uniform float width;
layout (location = 8) uniform float height;
layout (location = 9) uniform float lightRadius;
layout (location = 10) uniform float linear;
layout (location = 11) uniform float quadratic;
layout (location = 12) uniform vec3 lightPos;
layout (location = 13) uniform vec3 lightColor;
layout (location = 14) uniform vec3 camPos;

layout (location = 15) uniform sampler2D col_Text;
layout (location = 16) uniform sampler2D normal_Text;
layout (location = 17) uniform sampler2D worldPos_Text;

out vec4 FragColor;

vec4 CalcLight(Light lig, vec3 lightDir, vec3 worldPos, vec3 norm){
	vec4 AmbientColor = vec4(lig.Color, 1.0) * lig.AmbientIntensity;
	float Diffuse = dot(norm, -lightDir);

	vec4 DiffuseColor = vec4(0,0,0,0);
	vec4 SpecColor = vec4(0,0,0,0);
	if(Diffuse > 0){
		DiffuseColor = vec4(lig.Color * lig.DiffuseIntensity * Diffuse, 1.0f);
		vec3 E = normalize(camPos - worldPos);
		vec3 reflect = normalize(reflect(lightDir, norm));
		float spec = dot(E,reflect);
		if(spec > 0){
			spec = pow(spec, 16);
			SpecColor = vec4(lig.Color * spec, 1.0);
		}
	}
	return (AmbientColor + DiffuseColor + SpecColor);
}

vec4 CalcPointLight(Light lig, vec3 norm, vec3 worldPos){
	vec3 LightDir = worldPos - lig.Position;
	float dist = length(LightDir);
	LightDir = normalize(LightDir);

	vec4 Color = CalcLight(lig, LightDir, worldPos,norm);
	float Att = lig.Atten.Constant + lig.Atten.Linear * dist + lig.Atten.Exp * dist * dist;
	
	return Color/Att; 
}

void main()
{
	//getting pixel data
	vec2 uv = vec2(gl_FragCoord.x /width, gl_FragCoord.y /height);
	vec3 color = texture(col_Text,uv).rgb;
	vec3 norm = texture(normal_Text,uv).rgb;
	vec3 fragPos = texture(worldPos_Text,uv).rgb;

	//lighting calculations
	vec3 LightDir = fragPos - lightPos;
	float dist = length(LightDir);
	LightDir = normalize(LightDir);

	vec4 AmbientColor = vec4(0.2,0.2,0.2,1.0);
	float Diffuse = dot(normalize(norm), -LightDir);

	vec4 DiffuseColor = vec4(0,0,0,0);
	vec4 SpecColor = vec4(0,0,0,0);
	if(Diffuse > 0){
		DiffuseColor = vec4(lightColor * 1.0f * Diffuse, 1.0f);
		vec3 E = normalize(camPos - fragPos);
		vec3 reflect = normalize(reflect(LightDir, norm));
		float spec = dot(E,reflect);
		if(spec > 0){
			spec = pow(spec, 16);
			SpecColor = vec4(lightColor * spec, 1.0);
		}
	}
	vec4 lighting = (AmbientColor + DiffuseColor + SpecColor);
	float Att = 0.0 + linear * dist + quadratic * dist * dist;

	FragColor = vec4(color,1.0) * (lighting/Att);

	//FragColor = vec4(fragPos,1.0);
	//FragColor = (lighting/Att);
	//FragColor = vec4(1.0,1.0,1.0,1.0) * dist;
}