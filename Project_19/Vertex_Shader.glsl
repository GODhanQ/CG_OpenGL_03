#version 330 core

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_color;

uniform mat4 View_Matrix, Perspective_Matrix, Ortho_Matrix;
uniform int Figure_Type, Model_Index;
uniform mat4 Sun_ModelMatrix, Planet1_ModelMatrix, Planet2_ModelMatrix, Planet3_ModelMatrix;
uniform mat4 Sat1_ModelMatrix, Sat2_ModelMatrix, Sat3_ModelMatrix;
uniform mat4 GlobalTransform_Matrix;

uniform bool Perspective_Flag;


out vec3 out_color;

void main()
{
    vec4 aPos = vec4(in_position, 1.0);

    if (Figure_Type != -1) {
        switch (Model_Index) {
            case 0:
                aPos = Sun_ModelMatrix * aPos;
                break;
            case 1:
                aPos = Planet1_ModelMatrix * aPos;
                break;
            case 2:
                aPos = Planet2_ModelMatrix * aPos;
                break;
            case 3:
                aPos = Planet3_ModelMatrix * aPos;
                break;
            case 4:
                aPos = Sat1_ModelMatrix * aPos;
                break;
            case 5:
                aPos = Sat2_ModelMatrix * aPos;
                break;
            case 6:
                aPos = Sat3_ModelMatrix * aPos;
                break;
        }

        aPos = GlobalTransform_Matrix * aPos;
    }
    else {
        mat4 Scale_Axis = mat4(1.0);
        Scale_Axis = mat4(
            10.0, 0.0, 0.0, 0.0,
            0.0, 10.0, 0.0, 0.0,
            0.0, 0.0, 10.0, 0.0,
            0.0, 0.0, 0.0, 1.0
        );

        aPos = Scale_Axis * aPos;
    }

    aPos = View_Matrix * aPos;
    if (Perspective_Flag) aPos = Perspective_Matrix * aPos;
    else aPos = Ortho_Matrix * aPos;

    gl_Position = aPos;
    out_color = in_color;
}