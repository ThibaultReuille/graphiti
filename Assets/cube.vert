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

attribute vec3 a_Position;
attribute vec3 a_Normal;

uniform mat4 u_ModelMatrix;
uniform mat4 u_ViewMatrix;
uniform mat4 u_ProjectionMatrix;
uniform mat3 u_NormalMatrix;

uniform Light u_Light;
uniform Material u_Material;

varying vec3 v_Position;
varying vec3 v_Normal;
varying vec4 v_Color;

void main(void)
{
	v_Position = vec3(u_ViewMatrix * u_ModelMatrix * vec4(a_Position, 1.0));
	v_Normal = normalize(u_NormalMatrix * a_Normal);

	v_Color = vec4(u_Material.Ambient, 0.0);

	vec3 lightPositionViewSpace = vec3(u_ViewMatrix * vec4(u_Light.Position, 0.0));
	vec3 lightDirection = normalize(lightPositionViewSpace - v_Position);
	float lambert = dot(v_Normal, lightDirection);

	if (lambert > 0.0)
	{
		v_Color += vec4(u_Light.Color, 1.0) * u_Material.Diffuse * lambert;

		vec3 eyeDirection = normalize(-v_Position);
		vec3 r = reflect(-lightDirection, v_Normal);

		float specular = pow(max(dot(r, eyeDirection), 0.0), u_Material.Shininess);

		v_Color += vec4(u_Material.Specular * specular, 0.0);
	}

        v_Color.a = u_Material.Diffuse.a;

        gl_Position = u_ProjectionMatrix * vec4(v_Position, 1.0);
}

