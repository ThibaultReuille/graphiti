#version 330

struct Light
{
	int Type;
	vec3 Position;
	vec3 Direction;
	vec3 Color;
};

struct Material
{
	vec3 Ambient;
	vec4 Diffuse;
	vec3 Specular;
	float Shininess;
};

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_Texcoord;

uniform mat4 u_ModelMatrix;
uniform mat4 u_ViewMatrix;
uniform mat4 u_ProjectionMatrix;
uniform mat3 u_NormalMatrix;

uniform Light u_Light;
uniform Material u_Material;

out vec3 vs_Position;
out vec2 vs_Texcoord;
out vec3 vs_Normal;
out vec4 vs_Color;

void main(void)
{
    vs_Position = vec3(u_ViewMatrix * u_ModelMatrix * vec4(a_Position, 1.0));
    vs_Normal = normalize(u_NormalMatrix * a_Normal);
    vs_Texcoord = a_Texcoord;

    vs_Color = vec4(u_Material.Ambient, 0.0);

    vec3 lightPositionViewSpace = vec3(u_ViewMatrix * vec4(u_Light.Position, 0.0));
    vec3 lightDirection = normalize(lightPositionViewSpace - vs_Position);
    float lambert = dot(vs_Normal, lightDirection);

    if (lambert > 0.0)
    {
        vs_Color += vec4(u_Light.Color, 1.0) * u_Material.Diffuse * lambert;
        
        vec3 eyeDirection = normalize(-vs_Position);
        vec3 r = reflect(-lightDirection, vs_Normal);
        
        float specular = pow(max(dot(r, eyeDirection), 0.0), u_Material.Shininess);
        
        vs_Color += vec4(u_Material.Specular * specular, 0.0);
    }
    
    gl_Position = u_ProjectionMatrix * vec4(vs_Position, 1.0);
}