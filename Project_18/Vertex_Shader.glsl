#version 330 core

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_color;

uniform int Figure_Type;
uniform vec3 Sphere_Color;
uniform mat4 Perspective_Matrix;

uniform mat4 TranslationCube_Matrix, TranslationPyramid_Matrix, TranslationSphere_Matrix;

uniform mat4 Rotation_Matrix, Revolution_Matrix, Scaling_Matrix, Translation_Matrix;

out vec3 out_color;

void main()
{
    vec4 aPos = vec4(in_position, 1.0);
    vec3 color = in_color;

    // GLU Models
    if (Figure_Type == 2) {
        aPos = Rotation_Matrix * aPos;
        aPos = Translation_Matrix * aPos;

        aPos = TranslationSphere_Matrix * aPos;

        aPos = Scaling_Matrix * aPos;
        aPos = Revolution_Matrix * aPos;
        color = Sphere_Color;
    }
    // Custom Models
    else {
        if (Figure_Type == 0) {
            aPos = Rotation_Matrix * aPos;
            aPos = Translation_Matrix * aPos;

            aPos = TranslationCube_Matrix * aPos;
            
            aPos = Scaling_Matrix * aPos;
            aPos = Revolution_Matrix * aPos;
        }
        else if (Figure_Type == 1) {
            aPos = Rotation_Matrix * aPos;
            aPos = Translation_Matrix * aPos;

            aPos = TranslationPyramid_Matrix * aPos;

            aPos = Scaling_Matrix * aPos;
            aPos = Revolution_Matrix * aPos;
        }
    }

    gl_Position = Perspective_Matrix * aPos;
    out_color = color;
}