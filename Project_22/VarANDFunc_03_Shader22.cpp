#include "VarANDFunc_03_Shader22.h"

GLint Window_width{ 1200 }, Window_height{ 675 };

GLuint shaderProgramID{}, vertexShader{}, fragmentShader{};

GLuint VBO_axis{}, VAO_axis{}, IBO_axis{};

bool keyStates[256] = { false, };
std::map<int, bool> specialKeyStates;


glm::vec3 EYE(0.0f, 20.0f, 40.0f), AT(0.0f, 5.0f, 0.0f), UP(0.0f, 1.0f, 0.0f);
glm::vec3 Camera_Transform(0.0f, 0.0f, 0.0f), Camera_Transform_Factor(0.0f, 0.0f, 0.0f);
GLuint PerspectiveMatrixID{}, ViewMatrixID;
float Camera_Movement_Factor_Scale{ 10.f };
float Camera_Rotation_Sum{}, Camera_Rotation_Factor{}, Camera_Rotation_Factor_Scale{ 50.f };

GLuint FigureTypeID{};

GLuint ModelMatrixID{};
glm::vec3 Model_Transform(0.0f, 0.0f, 0.0f), Model_Movement_Factor(0.0f, 0.0f, 0.0f);
glm::vec3 Model_Scale(0.5f, 0.5f, 0.5f);
float Model_Movement_Factor_Scale{ 10.0f }, Rotation_Speed{ 10.0f };
glm::quat Model_Orientation{};

GLuint BodyMatrixID{};

float Animation_Time{ 0.0f }, Animation_Speed{ 5.0f };
GLuint LeftArmMatrixID{}, RightArmMatrixID{};
glm::vec3 Arm_Offset(0.0f, 7.5f ,0.0f), Arm_Rotation_Angle(0.0f, 0.0f, 0.0f);
float Arm_Rotation_Speed{ 5.0f };

GLuint LeftLegMatrixID{}, RightLegMatrixID{};
glm::vec3 Leg_Offset(0.0f, 4.5f, 0.0f), Leg_Rotation_Angle(0.0f, 0.0f, 0.0f);
float Leg_Rotation_Speed{ 5.0f };

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
