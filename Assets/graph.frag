#ifdef GL_ES
precision mediump float;
#endif

varying vec4 v_Color;

void main(void)
{
    gl_FragColor = v_Color;
}
