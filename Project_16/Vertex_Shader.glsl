#version 330 core
layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_color;

uniform int Figure_Type;
uniform mat4 ComTransMatrix;
uniform mat4 Perspective_Matrix;

out vec3 out_color;

void main()
{
    vec4 aPos = vec4(in_position, 1.0);

    if (Figure_Type != 2) {
        gl_Position = Perspective_Matrix * ComTransMatrix * aPos;
    }
    else {
        gl_Position = Perspective_Matrix * aPos;
    }

    out_color = in_color;
}