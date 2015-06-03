#version 330

#ifdef GL_ES
precision mediump float;
#endif

in vec4 gs_Color;

out vec4 FragColor;

void main(void)
{
    FragColor = gs_Color;
}
