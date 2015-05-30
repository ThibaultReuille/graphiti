#version 330

uniform mat4 u_ModelViewProjectionMatrix;

layout(lines) in;
layout(triangle_strip, max_vertices = 4) out;

out float gs_Y;

void main()
{	
	vec4 p0 = gl_in[0].gl_Position;
	vec4 p1 = gl_in[1].gl_Position;

	gl_Position = u_ModelViewProjectionMatrix * p0;
	gs_Y = p0.y;
	EmitVertex();
	
	gl_Position = u_ModelViewProjectionMatrix * vec4(p0.x, 0,    p0.z, 1.0);
	gs_Y = 0;
	EmitVertex();
	
	gl_Position = u_ModelViewProjectionMatrix * vec4(p1.x, p0.y, p1.z, 1.0);
	gs_Y = p1.y;
	EmitVertex();
	
	gl_Position = u_ModelViewProjectionMatrix * vec4(p1.x, 0,    p1.z, 1.0);
	gs_Y = 0;
	EmitVertex();

  	EndPrimitive();
}
