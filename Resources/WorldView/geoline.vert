uniform mat4 u_ModelViewProjection;

attribute vec3 a_Position;

void main(void)
{
    gl_Position = u_ModelViewProjection * vec4(a_Position, 1.0);
}
