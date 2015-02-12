#version 330 core

uniform mat4 u_ProjectionMatrix;

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

in vec4 vs_Color[];
in float vs_Size[];

out vec4 gs_Color;
out vec2 gs_UV;

void main()
{	
	vec2 pos[4] = vec2[](
		vec2(-0.5, -0.5),
		vec2( 0.5, -0.5),
		vec2(-0.5,  0.5),
		vec2( 0.5,  0.5)
	);

	vec2 uv[4] = vec2[](
		vec2(0.0, 0.0),
		vec2(1.0, 0.0),
		vec2(0.0, 1.0),
		vec2(1.0, 1.0)
	);

	vec4 p = gl_in[0].gl_Position;

	for(int i = 0; i < 4; i++)
	{
		vec2 v = p.xy + pos[i] * vs_Size[0];

		gl_Position = u_ProjectionMatrix * vec4(v, p.zw);
		gs_Color = vs_Color[0];
		gs_UV = uv[i];
    	EmitVertex();
	}

  	EndPrimitive();
}