#version 330

#define M_PI 3.1415926535897932384626433832795

layout(location = 0) in vec2 a_Location;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in float a_Size;

uniform vec2 u_Dimension;

out float vs_Size;
out vec4 vs_Color;

void main()
{
	vs_Size = a_Size;
	vs_Color = a_Color;

	float radLatitude = a_Location.x * M_PI / 180.0;
	float radLongitude = a_Location.y * M_PI / 180.0;

	// NOTE : http://en.wikipedia.org/wiki/Miller_projection
	vec2 miller = vec2
	(
		radLongitude,
		(5.0f / 4.0f) * log(tan(M_PI / 4.0f + 2.0f * radLatitude / 5.0f))
	);

	float range = (5.0f / 4.0f) * log(tan(9.0f * M_PI / 20.0f));

	// NOTE : Scale to screen
	vec2 pos = vec2 
	(
		u_Dimension.x / 2 + u_Dimension.x * miller.x / (2 * M_PI),
		u_Dimension.y / 2 + u_Dimension.y * miller.y / (2 * range)
	);

	gl_Position = vec4(pos, 0.0, 1.0);
}