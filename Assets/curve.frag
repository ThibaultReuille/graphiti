#version 330

#ifdef GL_ES
precision mediump float;
#endif

in vec4 vs_Color;

out vec4 FragColor;

void main(void)
{
    FragColor = vs_Color;
}
