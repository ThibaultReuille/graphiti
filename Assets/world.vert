#version 330

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_Texcoord;

uniform mat4 u_ModelViewProjection;

out vec2 vs_Texcoord;

void main(void)
{
    vs_Texcoord = a_Texcoord;
    gl_Position = u_ModelViewProjection * vec4(a_Position, 1.0);
}

