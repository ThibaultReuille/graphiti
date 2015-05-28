#version 330

#ifdef GL_ES
precision mediump float;
#endif

uniform sampler2D u_Texture;

in vec2 vs_Texcoord;
in vec3 vs_Normal;
in vec4 vs_Color;

out vec4 FragColor;

void main(void)
{
    vec4 texcolor = texture(u_Texture, vs_Texcoord).rgba;

	//float gray = (texcolor.r + texcolor.g + texcolor.b) / 2.5;
	//float c = sin(pow(gray, 4.0));

	//texcolor.r = c;
	//texcolor.g = c;
	//texcolor.b = c;

    FragColor = texcolor * vs_Color;
}
