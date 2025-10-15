#version 330 core
layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_color;

uniform int Figure_Type, Plane_Number;
uniform float Shape_Range;
uniform mat4 ComTransMatrix, Perspective_Matrix, DoorTrans_Matrix;
uniform mat4 Scaling_Matrix, SideRotate_Matrix;

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
        aPos = ComTransMatrix * aPos;
    }

    gl_Position = Perspective_Matrix * aPos;

    out_color = in_color;
}