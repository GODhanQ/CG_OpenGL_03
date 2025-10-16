#version 330 core
layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_color;

uniform int Figure_Type, Plane_Number, Current_Pyramid_SeqIndex;
uniform float Shape_Range;
uniform mat4 ComTransMatrix, Perspective_Matrix, DoorTrans_Matrix;
uniform mat4 Scaling_Matrix, SideRotate_Matrix;
uniform mat4 PyramidPX_Matrix, PyramidMX_Matrix, PyramidPZ_Matrix, PyramidMZ_Matrix;

uniform mat4 TranslationCube_Matrix, TranslationPyramid_Matrix;

out vec3 out_color;

void main()
{
    vec4 aPos = vec4(in_position, 1.0);

    // Cube
    if (Figure_Type == 0) {
        if (Plane_Number == 0) {
            aPos.z -= Shape_Range; aPos.y -= Shape_Range;
            aPos = DoorTrans_Matrix * aPos;
            aPos.z += Shape_Range; aPos.y += Shape_Range;
        } 
        else if (Plane_Number == 1) {
            aPos.z += Shape_Range;
            aPos = Scaling_Matrix * aPos;
            aPos.z -= Shape_Range;
        }
        else if (Plane_Number == 4) {
            aPos.x -= Shape_Range;
            aPos = SideRotate_Matrix * aPos;
            aPos.x += Shape_Range;
        }
        else if (Plane_Number == 5) {
            aPos.x += Shape_Range;
            aPos = SideRotate_Matrix * aPos;
            aPos.x -= Shape_Range;
        }

        aPos = ComTransMatrix * aPos;
    }
    // Pyramid
    else if (Figure_Type == 1) {
        if (Plane_Number == 6) { // -> +Z
            if (Current_Pyramid_SeqIndex == 0 || Current_Pyramid_SeqIndex == 1) {
                aPos.z -= Shape_Range; aPos.y += Shape_Range;
                aPos = PyramidPZ_Matrix * aPos;
                aPos.z += Shape_Range; aPos.y -= Shape_Range;
            }
        }
        else if (Plane_Number == 7) { // -> -Z
            if (Current_Pyramid_SeqIndex == 0 || Current_Pyramid_SeqIndex == 2) {
                aPos.z += Shape_Range; aPos.y += Shape_Range;
                aPos = PyramidMZ_Matrix * aPos;
                aPos.z -= Shape_Range; aPos.y -= Shape_Range;
            }
        }
        else if (Plane_Number == 8) { // -> +X
            if (Current_Pyramid_SeqIndex == 0 || Current_Pyramid_SeqIndex == 3) {
                aPos.x -= Shape_Range; aPos.y += Shape_Range;
                aPos = PyramidPX_Matrix * aPos;
                aPos.x += Shape_Range; aPos.y -= Shape_Range;
            }
        }
        else if (Plane_Number == 9) { // -> -X
            if (Current_Pyramid_SeqIndex == 0 || Current_Pyramid_SeqIndex == 4) {
                aPos.x += Shape_Range; aPos.y += Shape_Range;
                aPos = PyramidMX_Matrix * aPos;
                aPos.x -= Shape_Range; aPos.y -= Shape_Range;
            }
        }

        aPos = ComTransMatrix * aPos;
    }

    // Test for Project_18
    if (Figure_Type == 0) {// Cube
        aPos = TranslationCube_Matrix * aPos;
    } else if (Figure_Type == 1) {// Pyramid
        aPos = TranslationPyramid_Matrix * aPos;
    }

    gl_Position = Perspective_Matrix * aPos;

    out_color = in_color;
}