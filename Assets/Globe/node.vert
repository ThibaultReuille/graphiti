#version 330

#define M_PI 3.1415926535897932384626433832795

layout(location = 0) in vec2 a_Location;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in float a_Size;

uniform float u_Radius;

out float vs_Size;
out vec4 vs_Color;

void main()
{
	vs_Size = a_Size;
	vs_Color = a_Color;

	float radLatitude = (a_Location.x - 90) * M_PI / 180.0;
	float radLongitude = (180 - a_Location.y) * M_PI / 180.0;

	vec3 pos = vec3(
		u_Radius * cos(radLongitude) * sin(radLatitude),
		u_Radius * cos(radLatitude),
		u_Radius * sin(radLongitude) * sin(radLatitude)
	); 

	gl_Position = vec4(pos, 1.0);
}