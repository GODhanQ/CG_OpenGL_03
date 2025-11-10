#version 330 core

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_color;

uniform int Figure_Type;
uniform mat4 Perspective_Matrix, View_Matrix;
uniform mat4 Model_Matrix, OBJ_Model_Transform_Matrix, Box_Matrix;
uniform vec3 OBJ_Center_Offset;

out vec3 out_color;

void main()
{
    vec4 aPos = vec4(in_position, 1.0);
    
    if (Figure_Type >= 0) {
        aPos.xyz -= OBJ_Center_Offset;
        aPos = Box_Matrix * aPos;
        aPos.xyz += OBJ_Center_Offset;

        aPos = OBJ_Model_Transform_Matrix * aPos;
        aPos = Model_Matrix * aPos;
    }

    gl_Position = Perspective_Matrix * View_Matrix * aPos;
    out_color = in_color;
}
