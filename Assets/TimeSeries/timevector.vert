#version 330

layout(location = 0) in float a_Time;
layout(location = 1) in float a_Value;

void main(void)
{
    gl_Position = vec4(a_Time, a_Value, 0.0, 1.0);
}

