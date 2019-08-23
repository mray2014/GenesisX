#version 430

layout (location = 3) in vec3 Position; 

layout (location = 0)uniform mat4 gWVP;

void main()
{          
    gl_Position = gWVP * vec4(Position, 1.0);
}
