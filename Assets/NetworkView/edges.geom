#version 330 core

uniform mat4 u_ModelViewMatrix;
uniform mat4 u_ProjectionMatrix;

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

in vec3 vs_SourcePosition[];
in vec4 vs_SourceColor[];

in vec3 vs_TargetPosition[];
in vec4 vs_TargetColor[];

in float vs_Width[];

out vec4 gs_Color;
//out vec2 gs_UV;

void main()
{	
	vec4 origin = gl_in[0].gl_Position;

	vec4 p0 = u_ModelViewMatrix * (origin + vec4(vs_SourcePosition[0], 1.0));
	vec4 p1 = u_ModelViewMatrix * (origin + vec4(vs_TargetPosition[0], 1.0));

	vec3 np0 = p0.xyz / p0.w;
	vec3 np1 = p1.xyz / p1.w;

	vec2 direction = normalize(np1.xy - np0.xy);
	vec2 ortho = 0.5 * vs_Width[0] * vec2(-direction.y, direction.x);

	gl_Position = vec4(vs_SourcePosition[0], 1.0) + vec4(ortho.xy, 0, 0);
	gl_Position = u_ProjectionMatrix * u_ModelViewMatrix * gl_Position;
	gs_Color = vs_SourceColor[0];
	EmitVertex();
	
	gl_Position = vec4(vs_TargetPosition[0], 1.0) + vec4(ortho.xy, 0, 0);
	gl_Position = u_ProjectionMatrix * u_ModelViewMatrix * gl_Position;
	gs_Color = vs_TargetColor[0];
	EmitVertex();

	gl_Position = vec4(vs_SourcePosition[0], 1.0) - vec4(ortho.xy, 0, 0);
	gl_Position = u_ProjectionMatrix * u_ModelViewMatrix * gl_Position;
	gs_Color = vs_SourceColor[0];
	EmitVertex();

	gl_Position = vec4(vs_TargetPosition[0], 1.0) - vec4(ortho.xy, 0, 0);
	gl_Position = u_ProjectionMatrix * u_ModelViewMatrix * gl_Position;
	gs_Color = vs_TargetColor[0];
	EmitVertex();
	
  	EndPrimitive();
}