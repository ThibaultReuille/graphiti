#version 330

uniform mat4 u_ModelViewMatrix;
uniform mat4 u_ProjectionMatrix;

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_UV;

layout(location = 2) in vec3 a_Translation;
layout(location = 3) in vec3 a_Scale;
layout(location = 4) in vec4 a_Color; 

out vec4 vs_Color;
out vec2 vs_UV;

void main(void)
{
	vec3 pos = a_Position * a_Scale + a_Translation;

   	gl_Position = u_ProjectionMatrix * u_ModelViewMatrix * vec4(pos, 1.0);

	vs_UV = a_UV;
	vs_Color = a_Color;
}
