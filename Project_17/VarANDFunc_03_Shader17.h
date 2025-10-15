#pragma once
#include <GL/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>
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
	const int CUBE = 0;
	const int PYRAMID = 1;
}

struct Vertex_glm {
	glm::vec3 position;
	glm::vec3 color;
};

struct Plane {
	int Plane_Index, Figure_Type;			// Figure_Type : 0 for cube, 1 for pyramid
	//std::vector<Vertex_glm> vertices;
	std::vector<unsigned int> indices;
	bool isSet;


	Plane() : Plane_Index(-1), isSet(false), Figure_Type(Figure_Type::CUBE) {}
	Plane(int index) : Plane_Index(index), isSet(false), Figure_Type(Figure_Type::CUBE) {}

	void Enable() { isSet = true; }
	void Disable() { isSet = false; }
};

struct PlaneManager {
	std::vector<Plane> planes;
	//std::vector<Vertex_glm> all_vertices;
	std::vector<unsigned int> all_indices;

	void PrepareBuffer() {
		//all_vertices.clear();
		all_indices.clear();
		unsigned int index_offset = 0;

		for (auto& plane : planes) {
			for (const auto& idx : plane.indices) {
				all_indices.push_back(idx);
			}
		}
	}
};

struct ThreeDFigure {
	std::vector<PlaneManager> Figures;
	std::vector<unsigned int> indices;

	ThreeDFigure() {
		Figures.clear();
		indices.clear();
	}

};

extern GLint Window_width, Window_height;

extern GLuint shaderProgramID, vertexShader, fragmentShader;

extern GLuint VBO_axis, VAO_axis, IBO_axis;
extern GLuint VBO, VAO, IBO;

extern GLuint PerspectiveMatrixID, FigureTypeID, ComTransMatrixID, PlaneNumberID;
extern GLuint Shape_RangeID, DoorTransMatrixID, ScalingMatrixID, SideRotateMatrixID;

extern float Shape_Range;
extern float DoorMaxAngle, Door_ProgressChangeAmount;
extern float MaxScale, Scaling_ProgressChangeAmount;
extern float SideRotate_Angle, SideRotate_Amount;

extern std::vector<Vertex_glm> Axis_Vertex;
extern std::vector<unsigned int> Axis_Index;
extern std::vector<glm::vec3> All_Vertices;

extern glm::vec3 Rotation_Angles, Transform, Rotate_Amount, Trans_Amount;

extern bool is_Traslate_Mode, Cube_Mode, is_DoorAnimating;
extern bool is_ScalingAnimating, is_SideRotateAnimating;

GLvoid drawScene();
GLvoid Reshape(int w, int h);

void KeyBoard(unsigned char key, int x, int y);
void MouseClick(int button, int state, int x, int y);
std::pair<float, float> ConvertScreenToOpenGL(int screen_x, int screen_y);

void INIT_BUFFER();
void UPDATE_BUFFER();

char* filetobuf(const char* file);
GLuint make_shaderProgram(const char* vertPath, const char* fragPath);

void MakePerspactiveMatrix();
void MakeComTransMatrix();

void SetupVertices();
void SetupCube();
void SetupPyramid();
void DisableAll();
void EnableAll();
void EnableAll(int figure_type);

void ComposeUniformVar();