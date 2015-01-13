attribute vec3 a_Position;
attribute vec3 a_Normal;
attribute vec2 a_Texcoord;

uniform mat4 u_ModelViewProjectionMatrix;

varying vec3 v_Normal;
varying vec2 v_Texcoord;

void main(void)
{
    v_Normal = a_Normal;
    v_Texcoord = a_Texcoord;
    gl_Position = u_ModelViewProjectionMatrix * vec4(a_Position, 1.0);
}

