#version 330

uniform mat4 u_ModelViewMatrix;

layout(location = 0) in vec3 a_Origin;

layout(location = 2) in vec3 a_Position;
layout(location = 3) in float a_Size;
layout(location = 4) in vec4 a_Color; 

out vec4 vs_Color;
out float vs_Size;

void main(void)
{
	vec3 pos = a_Origin + a_Position;

   	gl_Position = u_ModelViewMatrix * vec4(pos, 1.0);

	vs_Color = a_Color;
	vs_Size = a_Size;
}
