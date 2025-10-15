#include "VarANDFunc_03_Shader17.h"

GLint Window_width{ 800 }, Window_height{ 800 };

GLuint shaderProgramID{}, vertexShader{}, fragmentShader{};

GLuint VBO_axis{}, VAO_axis{}, IBO_axis{};
GLuint VBO{}, VAO{}, IBO{};

GLuint PerspectiveMatrixID{}, FigureTypeID{}, ComTransMatrixID{}, PlaneNumberID{};
GLuint Shape_RangeID{}, DoorTransMatrixID{}, ScalingMatrixID, SideRotateMatrixID{};
GLuint PyramidPXMatrixID{}, PyramidMXMatrixID{}, PyramidPZMatrixID{}, PyramidMZMatrixID{}, PyramidSeqIndexID{};

float Shape_Range{ 0.3f };
float DoorMaxAngle{ 120.0f }, Door_ProgressChangeAmount{ 0.002f };
float MaxScale{ 3.0f }, Scaling_ProgressChangeAmount{ 0.002f };
float SideRotate_Angle{ 0.0f }, SideRotate_Amount{ 0.0f };
float Pyramid_DoorMaxAngle{ 180.0f }, Pyramid_Door_ProgressChangeAmount{ 0.002f }, prevPyramidAngle{ 0.0f };
// 0 : all planes, 1 : +X plane, 2 : -X plane, 3 : +Z plane, 4 : -Z plane, -1 :Invalid
int Current_Pyramid_SeqIndex{ -1 };

glm::vec3 Rotate_Amount, Trans_Amount;
glm::vec3 Rotation_Angles{ 0.0f, 0.0f, 0.0f }, Transform{ 0.0f, 0.0f, 0.0f };

bool is_Traslate_Mode{ true }, Cube_Mode{ true }, is_DoorAnimating{ false };
bool is_ScalingAnimating{ false }, is_SideRotateAnimating{ false };
bool is_PyramidSeqAnimating{ false }, is_PyramidAllAnimating{ false }, is_OneSeqEnd{ false };

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
