#include "VarANDFunc_03_Shader15.h"

GLint Window_width{ 800 }, Window_height{ 800 };

Vertex_glm Axis_Vertex[] = {
	// Positions      // Colors
	{ glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f) }, // X axis - Red
	{ glm::vec3(1.0f, 0.0f, 0.0f),  glm::vec3(1.0f, 0.0f, 0.0f) },
	{ glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f) }, // Y axis - Green
	{ glm::vec3(0.0f,  1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f) },
	{ glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 0.0f, 1.0f) }, // Z axis - Blue
	{ glm::vec3(0.0f, 0.0f,  1.0f), glm::vec3(0.0f, 0.0f, 1.0f) }
};
unsigned int Axis_Index[] = {
	0, 1,
	2, 3,
	4, 5
};