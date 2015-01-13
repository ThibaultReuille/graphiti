attribute vec3 a_Position;
attribute vec2 a_Texcoord;

uniform mat4 u_ModelViewProjection;

varying vec2 v_Texcoord;

void main(void)
{
    v_Texcoord = a_Texcoord;
    gl_Position = u_ModelViewProjection * vec4(a_Position, 1.0);
}

