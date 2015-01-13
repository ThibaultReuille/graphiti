
attribute vec3 a_Position;
attribute vec3 a_Color;

uniform mat4 u_ModelViewProjection;

varying vec4 v_Color;

void main(void)
{
    v_Color = vec4(a_Color, 1.0);
    gl_Position = u_ModelViewProjection * vec4(a_Position, 1.0);
}

