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

extern GLuint ApplicateID;
extern GLuint PerspectiveMatrixID, FigureTypeID, TranslationCubeMatrixID, TranslationPyramidMatrixID;
extern GLuint SphereColorID, isGLUID;
extern GLuint RotationMatrixID, RevolutionMatrixID, ScalingMatrixID, TranslationMatrixID;
extern GLuint ScalingOriginFMatrixID, ScalingOriginSMatrixID;
extern GLuint StrangeRevolutionFMatrixID, StrangeRevolutionSMatrixID;

extern GLUquadricObj* qobj_s;
extern GLUquadricObj* qobj_c;

extern float Shape_Range, StrangeRevolutionMaxScale, StrangeRevolutionProgressAmount;
extern float Base_Range, ConvertLocProgressAmount, SquareMovingProgress, SquareMovingProgressAmount;
// 0 : FIrst Figure, 1 : Second Figure, 2 : All Figures
extern int Applicate_to, Display_Figure[], F_phase, S_phase;

extern std::vector<Vertex_glm> Axis_Vertex;
extern std::vector<unsigned int> Axis_Index;
extern std::vector<glm::vec3> All_Vertices;

extern glm::vec3 Rotation_Angles, Rotate_Amount, Revolution_Angles, Revolution_Amount;
extern glm::vec3 Scale_Factor, Scale_Amount, ScaleOrigin_Factor, ScaleOrigin_Amount;
extern glm::vec3 Translate_Factor, Translate_Amount;
extern glm::vec3 SquareMoving_TargetLoc[];

extern bool Rotation_Mode, Revolution_Mode, Scaling_Mode, StrangeRevolution_Mode;
extern bool ConvertLoc_Mode, SquareMoving_Mode;

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

void ComposeUniformVar();
void ResettingFigures();