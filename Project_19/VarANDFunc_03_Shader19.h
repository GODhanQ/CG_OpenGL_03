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
	const int AXIS = -1;
	const int CUBE = 0;
	const int PYRAMID = 1;
	const int SPHERE = 2;
	const int CYLINDER = 3;
	const int CUSTOM_OBJ = 4;
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

struct ModelData {
	std::string name;
	int Figure_Type = Figure_Type::CUSTOM_OBJ;
	int Model_Index = -1;
	std::vector<Vertex_glm> vertices;
	std::vector<unsigned int> indices;
	GLuint vao = 0, vbo = 0, ibo = 0;
	glm::mat4 modelMatrix = glm::mat4(1.0f);
};

extern GLint Window_width, Window_height;

extern GLuint shaderProgramID, vertexShader, fragmentShader;

extern GLuint VBO_axis, VAO_axis, IBO_axis;
extern GLuint VBO, VAO, IBO;

extern GLuint ViewTransformMatrixID, PerspectiveMatrixID, OrthoMatrixID;
extern GLuint FigureTypeID, ModelIndexID;
extern GLuint SunMatrixID, Planet1MatrixID, Planet2MatrixID, Planet3MatrixID;
extern GLuint Sat1MatrixID, Sat2MatrixID, Sat3MatrixID;
extern GLuint PerspectiveFlagID;
extern GLuint GlobalTransformMatrixID;
extern GLuint RotateMatrixID;

extern float Shape_Range, Base_Range;
extern float Sun_scale, planet_scale, satlite_scale;
extern float Lv1_Radius, Lv2_Radius;
extern float Lv1_Rotation_Speed, Lv2_Rotation_Speed;
extern float Planet1_Orbit_Angle, Planet2_Orbit_Angle, Planet3_Orbit_Angle;
extern float Sat1_Orbit_Angle, Sat2_Orbit_Angle, Sat3_Orbit_Angle;

extern std::vector<Vertex_glm> Axis_Vertex;
extern std::vector<unsigned int> Axis_Index;
extern std::vector<glm::vec3> All_Vertices;

extern bool Perspective_Flag;

extern glm::vec3 Global_Translation, Global_Rotation;

GLvoid drawScene();
GLvoid Reshape(int w, int h);

void KeyBoard(unsigned char key, int x, int y);
void MouseClick(int button, int state, int x, int y);
std::pair<float, float> ConvertScreenToOpenGL(int screen_x, int screen_y);

void INIT_BUFFER();
void UPDATE_BUFFER();

char* filetobuf(const char* file);
GLuint make_shaderProgram(const char* vertPath, const char* fragPath);

void MakeStaticMatrix();
void MakeDynamicMatrix();

void SetupVertices();
void SetupCube();
void SetupPyramid();
bool loadOBJ(const char* path, std::vector<glm::vec3>& out_vertices, std::vector<unsigned int>& out_indices);
void PushModels();
void ModelDivision();

void ComposeUniformVar();