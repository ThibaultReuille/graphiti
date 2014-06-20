
#ifdef GL_ES
precision mediump float;
#endif

uniform sampler2D u_Texture;

varying vec2 v_Texcoord;

void main(void)
{
      	vec4 texcolor = texture2D(u_Texture, v_Texcoord);
	float gray = (texcolor.r + texcolor.g + texcolor.b) / 3.0;

	float c = sin(pow(gray, 3.0));

	texcolor.r = c;
	texcolor.g = c;
	texcolor.b = c;

        gl_FragColor = texcolor;
}
