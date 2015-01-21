#version 330

#ifdef GL_ES
precision mediump float;
#endif

varying vec4 v_Color;

out vec4 FragColor;

void main(void)
{
    FragColor = v_Color;
}
