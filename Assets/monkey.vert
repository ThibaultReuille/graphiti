uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;

attribute vec3 a_Position;
attribute vec3 a_Normal;

struct Light
{
    vec3 position;
};

uniform Light u_Light;

varying vec4 v_Color;

void main(void)
{
    mat4 mvp = u_Projection * u_View * u_Model;

    vec3 v = (u_Model * vec4(a_Position, 1.0)).xyz;
    vec3 n = (u_Model * vec4(a_Normal, 1.0)).xyz;
    n = normalize(n);

    vec3 lightDir = u_Light.position - vec3(v.x, v.y, v.z);
    lightDir = normalize(lightDir);

    float lambert = dot(lightDir, n);
    v_Color = lambert * vec4(1.0, 1.0, 1.0, 1.0);

    gl_Position = u_Projection * u_View * vec4(v, 1.0);
}
