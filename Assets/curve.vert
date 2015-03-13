#version 330

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;

uniform mat4 u_ModelViewProjection;
uniform vec4 u_Tint;

out vec4 vs_Color;

void main(void)
{
    vs_Color = a_Color * u_Tint;
    gl_Position = u_ModelViewProjection * vec4(a_Position, 1.0);
}
