#version 330 core

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_color;

uniform bool is_GLU;
uniform int Figure_Type, Applicate_to;
uniform vec3 Sphere_Color;
uniform mat4 Perspective_Matrix;

uniform mat4 TranslationCube_Matrix, TranslationPyramid_Matrix;

uniform mat4 Rotation_Matrix, Revolution_Matrix, Scaling_Matrix, Translation_Matrix;
uniform mat4 ScalingOriginF_Matrix, ScalingOriginS_Matrix;
uniform mat4 StrangeRevolutionF_Matrix, StrangeRevolutionS_Matrix;

out vec3 out_color;

void main()
{
    vec4 aPos = vec4(in_position, 1.0);

    if (Figure_Type == 0 && Applicate_to != 1) {
        aPos = ScalingOriginF_Matrix * aPos;
        aPos = StrangeRevolutionF_Matrix * aPos;
        aPos = Rotation_Matrix * aPos;
        aPos = Translation_Matrix * aPos;

        aPos = TranslationCube_Matrix * aPos;
            
        aPos = Scaling_Matrix * aPos;
        aPos = Revolution_Matrix * aPos;
    }
    else if (Figure_Type == 1 && Applicate_to != 0) {
        aPos = ScalingOriginS_Matrix * aPos;
        aPos = StrangeRevolutionS_Matrix * aPos;
        aPos = Rotation_Matrix * aPos;
        aPos = Translation_Matrix * aPos;

        aPos = TranslationPyramid_Matrix * aPos;

        aPos = Scaling_Matrix * aPos;
        aPos = Revolution_Matrix * aPos;
    }

    gl_Position = Perspective_Matrix * aPos;

    if (is_GLU && Figure_Type != -1) out_color = Sphere_Color;
    else out_color = in_color;
}