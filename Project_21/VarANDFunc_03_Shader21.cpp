#include "VarANDFunc_03_Shader21.h"

GLint Window_width{ 800 }, Window_height{ 800 };

GLuint shaderProgramID{}, vertexShader{}, fragmentShader{};

GLuint VBO_axis{}, VAO_axis{}, IBO_axis{};

glm::vec3 EYE(0.0, 0.0, 30.0), AT(0.0, 0.0, 0.0), UP(0.0, 1.0, 0.0);
GLuint PerspectiveMatrixID{}, ViewMatrixID;

GLuint FigureTypeID{};
GLuint ModelMatrixID{};
GLuint OBJModelTransformMatrixID{}, BoxMatrixID{};
GLuint OBJCenterOffsetID{};

float Gravity_Factor{ 0.00000098f }, Delta_Time{};
std::chrono::high_resolution_clock::time_point Last_Frame_Time;
std::vector<Plane> Box_Planes;
float Object_Radius{ 1.0f }, Box_Rotate_Factor{ 0.0f }, Box_Rotate_Sum{};

glm::vec3 Model_Transform(0.0f, 0.0f, 0.0f), Model_Rotation(0.0f, 0.0f, 0.0f);

float Max_Camera_Revolution_Angle{ 60.0f }, Camera_Revolution_Factor{ 0.03f }, Camera_Revolution_Sum{ 0.0f };

int Mouse_X = 0, Mouse_Y = 0;

bool Open_Floor{ false };
float Floor_Open_Angle{ 0.0f }, Floor_Open_Angle_Factor{ 0.03f };

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
