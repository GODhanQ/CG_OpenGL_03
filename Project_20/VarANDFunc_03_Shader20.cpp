#include "VarANDFunc_03_Shader20.h"

GLint Window_width{ 800 }, Window_height{ 800 };

GLuint shaderProgramID{}, vertexShader{}, fragmentShader{};

GLuint VBO_axis{}, VAO_axis{}, IBO_axis{};

glm::vec3 EYE(15.0, 10.0, 15.0), AT(0.0, 0.0, 0.0), UP(0.0, 1.0, 0.0);
GLuint PerspectiveMatrixID{}, ViewMatrixID;
float Camera_Rotation_Sum{}, Camera_Rotation_Factor{}, Camera_Revolution_Sum{ 0.0f }, Camera_Revolution_Factor{ 0.0f };

GLuint FigureTypeID{};
GLuint ModelMatrixID{};
GLuint BodyMatrixID{}, NeckMatrixID{}, Head1MatrixID{}, Head2MatrixID;
GLuint Mouth1MatrixID{}, Mouth2MatrixID{}, Flag1MatrixID{}, Flag2MatrixID{};

glm::vec3 Model_Movement_Factor(0.0f, 0.0f, 0.0f);
glm::vec3 Neck_Rotate_Factor(0.0, 0.0, 0.0);
glm::vec3 Head_Rotate_Factor(0.0, 0.0, 0.0), Head_Movement_Factor(0.0, 0.0, 0.0);
float Head_Movement_Factor_Param{ 0.0f };
glm::vec3 Flag_Rotate_Factor(0.0, 0.0, 0.0);

bool Rotating_Head{ false };

std::vector<Vertex_glm> Axis_Vertex = {
	// Positions					// Colors
	{ glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f) }, // X axis - Red
	{ glm::vec3(5.0f, 0.0f, 0.0f),  glm::vec3(1.0f, 0.0f, 0.0f) },
	{ glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f) }, // Y axis - Green
	{ glm::vec3(0.0f, 5.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f) },
	{ glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f) }, // Z axis - Blue
	{ glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, 1.0f) }
};
std::vector<unsigned int> Axis_Index = {
	0, 1,
	2, 3,
	4, 5
};
