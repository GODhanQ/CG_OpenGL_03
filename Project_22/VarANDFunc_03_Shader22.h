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
	const int FLOOR = 0;
	const int BODY = 1;
	const int NECK = 2;
	const int HEAD_1 = 3;
	const int HEAD_2 = 4;
	const int MOUTH_1 = 5;
	const int MOUTH_2 = 6;
	const int FLAG_1 = 7;
	const int FLAG_2 = 8;
}

struct Vertex_glm {
	glm::vec3 position;
	glm::vec3 color;
};

struct Custom_OBJ {
	std::string name;
	std::vector<Vertex_glm> vertices;
	std::vector<unsigned int> indices;
	GLuint VBO{}, VAO{}, IBO{};

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
extern GLuint ModelMatrixID;
extern GLuint BodyMatrixID, NeckMatrixID, Head1MatrixID, Head2MatrixID;
extern GLuint Mouth1MatrixID, Mouth2MatrixID, Flag1MatrixID, Flag2MatrixID;

extern glm::vec3 Model_Movement_Factor;
extern glm::vec3 Neck_Rotate_Factor;
extern glm::vec3 Head_Rotate_Factor, Head_Movement_Factor;
extern float Head_Movement_Factor_Param;
extern glm::vec3 Flag_Rotate_Factor;

extern bool Rotating_Head;

GLvoid drawScene();
GLvoid Reshape(int w, int h);

void KeyBoard(unsigned char key, int x, int y);
void SpecialKeyBoard(int key, int x, int y);
void MouseClick(int button, int state, int x, int y);
std::pair<float, float> ConvertScreenToOpenGL(int screen_x, int screen_y);

void INIT_BUFFER();

char* filetobuf(const char* file);
GLuint make_shaderProgram(const char* vertPath, const char* fragPath);
bool ReadObj(const std::string& path, OBJ_File& outFile);

void MakeStaticMatrix();
void MakeDynamicMatrix();

void ComposeUniformVar();
void ComposeOBJColor();