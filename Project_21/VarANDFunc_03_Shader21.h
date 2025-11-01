#pragma once
#include <GL/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <gl/GLU.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <deque>
#include <tuple>
#include <random>
#include <chrono>
#include <algorithm>
#include <map>
#include <string>
#define NOMINMAX
#include <windows.h>

namespace Figure_Type {
	const int IDK = -2;
	const int AXIS = -1;
	const int BOX = 0;
	const int CUBE = 1;
	const int BALL = 2;
}

struct Vertex_glm {
	glm::vec3 position;
	glm::vec3 color;
};

struct Plane {
	glm::vec3 normal;      // 평면의 법선 벡터
	glm::vec3 point;       // 평면 위의 한 점

	Plane() : normal(0.0f), point(0.0f) {}
	Plane(glm::vec3 n, glm::vec3 p) : normal(glm::normalize(n)), point(p) {}

	// 점에서 평면까지의 부호있는 거리
	float getSignedDistance(const glm::vec3& pos) const {
		return glm::dot(normal, pos - point);
	}
};

struct Custom_OBJ {
	std::string name;
	std::vector<Vertex_glm> vertices;
	std::vector<unsigned int> indices;
	GLuint VBO{}, VAO{}, IBO{};
	glm::vec3 movement_vector{};
	glm::vec3 base_location{};
	float collision_radius{ 0.5f };
	glm::vec3 center_offset{};
	glm::vec3 rotation_axis{};      // 회전축
	float rotation_angle{ 0.0f };   // 회전각도
};

struct OBJ_File {
	std::string file_name;
	std::vector<Custom_OBJ> objects;
};

extern GLint Window_width, Window_height;

extern GLuint shaderProgramID, vertexShader, fragmentShader;

extern GLuint VBO_axis, VAO_axis, IBO_axis;
extern std::vector<Vertex_glm> Axis_Vertex;
extern std::vector<unsigned int> Axis_Index;

extern glm::vec3 EYE, AT, UP;
extern GLuint PerspectiveMatrixID, ViewMatrixID;
extern float Camera_Rotation_Sum, Camera_Rotation_Factor, Camera_Revolution_Sum, Camera_Revolution_Factor;


extern GLuint FigureTypeID;
// for all models
extern GLuint ModelMatrixID;
// for specific OBJ model
extern GLuint OBJModelTransformMatrixID, BoxMatrixID;
extern GLuint OBJCenterOffsetID;

extern float Gravity_Factor, Delta_Time;
extern std::chrono::high_resolution_clock::time_point Last_Frame_Time;
extern std::vector<Plane> Box_Planes;
extern float Object_Radius, Box_Rotate_Factor, Box_Rotate_Sum;

extern glm::vec3 Model_Transform, Model_Rotation;

extern float Max_Camera_Revolution_Angle, Camera_Revolution_Factor, Camera_Revolution_Sum;

extern int Mouse_X, Mouse_Y;

extern bool Open_Floor;
extern float Floor_Open_Angle, Floor_Open_Angle_Factor;

GLvoid drawScene();
GLvoid Reshape(int w, int h);

void KeyBoard(unsigned char key, int x, int y);
void SpecialKeyBoard(int key, int x, int y);
void MouseClick(int button, int state, int x, int y);
void MouseDrag(int x, int y);
void MousePassiveMotion(int x, int y);
std::pair<float, float> ConvertScreenToOpenGL(int screen_x, int screen_y);

void INIT_BUFFER();

char* filetobuf(const char* file);
GLuint make_shaderProgram(const char* vertPath, const char* fragPath);
bool ReadObj(const std::string& path, OBJ_File& outFile);
void LoadObjects();

void MakeStaticMatrix();
void MakeDynamicMatrix();

void ComposeUniformVar();
void ComposeOBJColor();

void InitializeBoxPlanes();
void UpdateBoxPlanes(const glm::mat4& rotationMatrix);