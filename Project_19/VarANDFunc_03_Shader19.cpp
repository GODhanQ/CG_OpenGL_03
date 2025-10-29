#include "VarANDFunc_03_Shader19.h"

GLint Window_width{ 800 }, Window_height{ 800 };

GLuint shaderProgramID{}, vertexShader{}, fragmentShader{};

GLuint VBO_axis{}, VAO_axis{}, IBO_axis{};
GLuint VBO{}, VAO{}, IBO{};

GLuint ViewTransformMatrixID{}, PerspectiveMatrixID{}, OrthoMatrixID{};
GLuint FigureTypeID{}, ModelIndexID{};
GLuint SunMatrixID{}, Planet1MatrixID{}, Planet2MatrixID{}, Planet3MatrixID{};
GLuint Sat1MatrixID{}, Sat2MatrixID{}, Sat3MatrixID{};
GLuint PerspectiveFlagID{};
GLuint GlobalTransformMatrixID{};

float Shape_Range{ 0.3f }, Base_Range{ 1.5f };
float Sun_scale{ 1.5f }, planet_scale{ 1.0f }, satlite_scale{ 0.3f };
float Lv1_Radius{ 4.0f }, Lv2_Radius{ 2.0f };
float Lv1_Rotation_Speed{ 0.6f }, Lv2_Rotation_Speed{ 0.5f };
float Planet1_Orbit_Angle{ 0.0f }, Planet2_Orbit_Angle{ 90.0f }, Planet3_Orbit_Angle{ 180.f };
float Sat1_Orbit_Angle{ 0.0f }, Sat2_Orbit_Angle{ 90.0f }, Sat3_Orbit_Angle{ 180.0f };

bool Perspective_Flag{ true };

glm::vec3 Global_Translation{ 0.0f, 0.0f, 0.0f }, Global_Rotation{ 0.0f, 0.0f, 0.0f };

std::vector<Vertex_glm> Axis_Vertex = {
	// Positions					// Colors
	{ glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f) }, // X axis - Red
	{ glm::vec3(1.0f, 0.0f, 0.0f),  glm::vec3(1.0f, 0.0f, 0.0f) },
	{ glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f) }, // Y axis - Green
	{ glm::vec3(0.0f,  1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f) },
	{ glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f) }, // Z axis - Blue
	{ glm::vec3(0.0f, 0.0f,  1.0f), glm::vec3(0.0f, 0.0f, 1.0f) }
};
std::vector<unsigned int> Axis_Index = {
	0, 1,
	2, 3,
	4, 5
};
std::vector<glm::vec3> All_Vertices = {
	// eight vertices of a cube | range : Shape_Range
	glm::vec3(Shape_Range,  Shape_Range,  Shape_Range),			// 0 
	glm::vec3(Shape_Range,  Shape_Range, -Shape_Range),			// 1 
	glm::vec3(-Shape_Range,  Shape_Range, -Shape_Range),		// 2
	glm::vec3(-Shape_Range,  Shape_Range,  Shape_Range),		// 3
	glm::vec3(Shape_Range, -Shape_Range,  Shape_Range),			// 4
	glm::vec3(Shape_Range, -Shape_Range, -Shape_Range),			// 5
	glm::vec3(-Shape_Range, -Shape_Range, -Shape_Range),		// 6
	glm::vec3(-Shape_Range, -Shape_Range,  Shape_Range),		// 7
	glm::vec3(0.0f,  Shape_Range, 0.0f),						// 8
};
glm::vec3 SquareMoving_TargetLoc[] = {
	glm::vec3(0.0f, 0.0f, Base_Range),
	glm::vec3(0.0f, -Base_Range, 0.0f),
	glm::vec3(0.0f, 0.0f, -Base_Range),
	glm::vec3(0.0f, Base_Range, 0.0f),
};