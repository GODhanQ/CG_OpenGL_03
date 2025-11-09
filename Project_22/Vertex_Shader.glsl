#version 330 core

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_color;

uniform int Figure_Type;
uniform mat4 Perspective_Matrix, View_Matrix;
uniform mat4 Model_Matrix;
uniform mat4 Body_Matrix, Neck_Matrix, Head1_Matrix, Head2_Matrix;
uniform mat4 Mouth1_Matrix, Mouth2_Matrix, Flag1_Matrix, Flag2_Matrix;

out vec3 out_color;

void main()
{
    vec4 aPos = vec4(in_position, 1.0);
    
    if (Figure_Type >= 1) {
        if (Figure_Type == 1) {             // Body
            aPos = Body_Matrix * aPos;
        } else if (Figure_Type == 2) {      // Neck
            aPos = Neck_Matrix * aPos;
        } else if (Figure_Type == 3) {      // Head_1
            aPos = Head1_Matrix * aPos;
        } else if (Figure_Type == 4) {      // Head_2
            aPos = Head2_Matrix * aPos;
        } else if (Figure_Type == 5) {      // Mouth_1
            aPos = Mouth1_Matrix * aPos;
        } else if (Figure_Type == 6) {      // Mouth_2
            aPos = Mouth2_Matrix * aPos;
        } else if (Figure_Type == 7) {      // Flag_1
            aPos = Flag1_Matrix * aPos;
        } else if (Figure_Type == 8) {      // Flag_2
            aPos = Flag2_Matrix * aPos;
        } else if (Figure_Type == 99 ) {    // ETC
            
        }
        
        aPos = Model_Matrix * aPos;
    }

    gl_Position = Perspective_Matrix * View_Matrix * aPos;
    out_color = in_color;
}
