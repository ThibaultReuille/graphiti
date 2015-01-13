
#ifdef GL_ES
precision mediump float;
#endif

uniform sampler2D u_Texture;

varying vec2 v_Texcoord;
varying vec3 v_Normal;
varying vec4 v_Color;

void main(void)
{
    vec4 texcolor = texture2D(u_Texture, v_Texcoord);

	float gray = (texcolor.r + texcolor.g + texcolor.b) / 2.5;
	float c = sin(pow(gray, 4.0));

	texcolor.r = c;
	texcolor.g = c;
	texcolor.b = c;

    gl_FragColor = texcolor + v_Color;
}
