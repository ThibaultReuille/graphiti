#version 330

#ifdef GL_ES
precision mediump float;
#endif

uniform vec4 u_Color;

out vec4 FragColor;

void main(void)
{
    FragColor = u_Color;
}
