#version 330 core

uniform mat4 u_ProjectionMatrix;
uniform mat4 u_ViewMatrix;

layout(points) in;
layout(line_strip, max_vertices = 2) out;

in vec4 vs_Color[];
in float vs_Size[];

out vec4 gs_Color;

void main()
{	
	vec4 p = gl_in[0].gl_Position;

	vec3 n = normalize(p.xyz);

	gl_Position = u_ProjectionMatrix * u_ViewMatrix * p;
	gs_Color = vs_Color[0];
	EmitVertex();

	gl_Position = u_ProjectionMatrix * u_ViewMatrix * vec4(p.xyz + vs_Size[0] * n, 1.0);
	gs_Color = vs_Color[0];
	EmitVertex();

  	EndPrimitive();
}