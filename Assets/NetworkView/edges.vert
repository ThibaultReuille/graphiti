#version 330

layout(location = 0) in vec4 a_Origin;

layout(location = 2) in vec4 a_SourcePosition;
layout(location = 3) in vec4 a_SourceColor;

layout(location = 4) in vec4 a_TargetPosition;
layout(location = 5) in vec4 a_TargetColor;

layout(location = 6) in float a_Width;

out vec3 vs_SourcePosition;
out vec4 vs_SourceColor;

out vec3 vs_TargetPosition;
out vec4 vs_TargetColor;

out float vs_Width;

void main(void)
{
   	gl_Position = a_Origin;

	vs_SourcePosition = a_SourcePosition.xyz;
	vs_SourceColor = a_SourceColor;

	vs_TargetPosition = a_TargetPosition.xyz;
	vs_TargetColor = a_TargetColor;

	vs_Width = a_Width / 2.0;
}
