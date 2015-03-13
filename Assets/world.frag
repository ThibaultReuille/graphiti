#version 330

#ifdef GL_ES
precision mediump float;
#endif

uniform sampler2D u_Texture;

in vec2 vs_Texcoord;

out vec4 FragColor;

void main(void)
{
    FragColor = texture(u_Texture, vs_Texcoord).rgba;
}

