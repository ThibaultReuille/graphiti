#version 330 core

uniform mat4 u_ModelViewMatrix;
uniform mat4 u_ProjectionMatrix;

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;
//layout(line_strip, max_vertices = 2) out;

in vec3 vs_SourcePosition[];
in vec4 vs_SourceColor[];

in vec3 vs_TargetPosition[];
in vec4 vs_TargetColor[];

in float vs_Width[];

out vec4 gs_Color;
//out vec2 gs_UV;

/* Simple Lines
void main()
{	
	vec4 origin = gl_in[0].gl_Position;

	vec4 source = u_ProjectionMatrix * u_ModelViewMatrix * vec4(origin.xyz + vs_SourcePosition[0], 1.0);
	vec4 target = u_ProjectionMatrix * u_ModelViewMatrix * vec4(origin.xyz + vs_TargetPosition[0], 1.0);

	gl_Position = source;
	gs_Color = vs_SourceColor[0];
	EmitVertex();
	
	gl_Position = target;
	gs_Color = vs_TargetColor[0];
	EmitVertex();
	
  	EndPrimitive();
}
*/

void main()
{	
	vec4 origin = gl_in[0].gl_Position;

	vec4 source = u_ModelViewMatrix * vec4(origin.xyz + vs_SourcePosition[0], 1.0);
	vec4 target = u_ModelViewMatrix * vec4(origin.xyz + vs_TargetPosition[0], 1.0);

	vec3 p0 = source.xyz / source.w;
	vec3 p1 = target.xyz / target.w;

	vec3 direction = normalize(p1 - p0);
	vec3 extrusion = 0.5 * vs_Width[0] * cross(direction, vec3(0, 0, 1));

	//vec3 extrusion = 0.5 * vs_Width[0] * vec3(-direction.y, direction.x, 0);

	gl_Position = u_ProjectionMatrix * vec4(p0 + extrusion, 1.0);
	gs_Color = vs_SourceColor[0];
	EmitVertex();
	
	gl_Position = u_ProjectionMatrix * vec4(p1 + extrusion, 1.0);
	gs_Color = vs_TargetColor[0];
	EmitVertex();

	gl_Position = u_ProjectionMatrix * vec4(p0 - extrusion, 1.0);
	gs_Color = vs_SourceColor[0];
	EmitVertex();

	gl_Position = u_ProjectionMatrix * vec4(p1 - extrusion, 1.0);
	gs_Color = vs_TargetColor[0];
	EmitVertex();
	
  	EndPrimitive();
}
