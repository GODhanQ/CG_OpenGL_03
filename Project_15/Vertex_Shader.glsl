#version 330 core
layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_color;

uniform mat4 Perspective_Matrix;

out vec3 out_color;

void main()
{
    gl_Position = Perspective_Matrix * vec4(in_position, 1.0);
    out_color = in_color;
}