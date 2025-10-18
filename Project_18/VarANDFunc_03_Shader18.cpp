#include "VarANDFunc_03_Shader18.h"

GLint Window_width{ 800 }, Window_height{ 800 };

GLuint shaderProgramID{}, vertexShader{}, fragmentShader{};

GLuint VBO_axis{}, VAO_axis{}, IBO_axis{};
GLuint VBO{}, VAO{}, IBO{};

GLuint PerspectiveMatrixID{}, FigureTypeID{}, ComTransMatrixID{}, TranslationCubeMatrixID{}, TranslationPyramidMatrixID{};
GLuint TranslationSphereMatrixID{}, SphereColorID{};
GLuint RotationMatrixID{}, RevolutionMatrixID{}, ScalingMatrixID{}, TranslationMatrixID{};

GLUquadricObj* qobj;

float Shape_Range{ 0.3f };
int Applicate_to{ 2 };

glm::vec3 Rotate_Amount, Rotation_Angles, Revolution_Amount, Revolution_Angles;
glm::vec3 Scale_Factor(1.0f, 1.0f, 1.0f), Scale_Amount(0.1f, 0.1f, 0.1f);
glm::vec3 Translate_Factor, Translate_Amount(0.05f, 0.05f, 0.05f);

bool Rotation_Mode{ false }, Revolution_Mode{ false }, Scaling_Mode{ false };

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
