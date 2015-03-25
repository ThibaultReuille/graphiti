#version 330

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec2 a_Texcoord;

uniform mat4 u_ModelViewProjectionMatrix;

out vec3 vs_Normal;
out vec2 vs_Texcoord;

void main(void)
{
    vs_Normal = a_Normal;
    vs_Texcoord = a_Texcoord;
    gl_Position = u_ModelViewProjectionMatrix * vec4(a_Position, 1.0);
}

