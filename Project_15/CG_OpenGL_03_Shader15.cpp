#define _CRT_SECURE_NO_WARNINGS
#include "VarANDFunc_03_Shader15.h"

GLuint shaderProgramID;
GLuint vertexShader;
GLuint fragmentShader;

GLuint VBO_axis, VAO_axis, IBO_axis;
GLuint VBO, VAO, IBO;

GLuint PerspectiveMatrixID;
glm::mat4 Perspective_Matrix(1.0f);

std::vector<Vertex_glm> Axis_Vertex = {
	// Positions					// Colors
	{ glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f) }, // X axis - Red
	{ glm::vec3(1.0f, 0.0f, 0.0f),  glm::vec3(1.0f, 0.0f, 0.0f) },
	{ glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f) }, // Y axis - Green
	{ glm::vec3(0.0f,  1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f) },
	{ glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 0.0f, 1.0f) }, // Z axis - Blue
	{ glm::vec3(0.0f, 0.0f,  1.0f), glm::vec3(0.0f, 0.0f, 1.0f) }
};
std::vector<unsigned int> Axis_Index = {
	0, 1,
	2, 3,
	4, 5
};
std::vector<Vertex_glm> Cube_Vertex;
std::vector<Vertex_glm> Pyramid_Vertex;

std::vector<Vertex_glm> all_vertices;
std::vector<unsigned int> all_indices;

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition(500, 50);
	glutInitWindowSize(Window_width, Window_height);
	glutCreateWindow("Example15");

	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		return 1;
	}
	std::cout << "glew initialized\n";



	shaderProgramID = make_shaderProgram("Vertex_Shader.glsl", "Fragment_Shader.glsl");
	std::cout << "Make Shader Program Completed\n";

	INIT_BUFFER();
	std::cout << "INIT BUFFER Completed\n";

	glEnable(GL_PROGRAM_POINT_SIZE);

	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(KeyBoard);
	glutMouseFunc(MouseClick);
	glutIdleFunc(drawScene);

	glutMainLoop();
}

GLvoid drawScene() {
	GLfloat rColor{ 0.3f }, gColor{ 0.3f }, bColor{ 0.3f };
	glClearColor(rColor, gColor, bColor, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//MakePerspectiveMat();

	PerspectiveMatrixID = glGetUniformLocation(shaderProgramID, "Perspective_Matrix");
	glUniformMatrix4fv(PerspectiveMatrixID, 1, GL_FALSE, &Perspective_Matrix[0][0]);

	glUseProgram(shaderProgramID);

	glBindVertexArray(VAO_axis);
	glLineWidth(3.0f);
	glDrawElements(GL_LINES, Axis_Index.size(), GL_UNSIGNED_INT, 0);

	glutSwapBuffers();
}
GLvoid Reshape(int w, int h) {
	glViewport(0, 0, w, h);
}

void KeyBoard(unsigned char key, int x, int y) {
	switch (key) {
	case 'q':
		exit(0);
	}
}
void MouseClick(int button, int state, int x, int y) {

}
std::pair<float, float> ConvertScreenToOpenGL(int screen_x, int screen_y) {
	int width = glutGet(GLUT_WINDOW_WIDTH);
	int height = glutGet(GLUT_WINDOW_HEIGHT);

	float ogl_x = (2.0f * screen_x) / width - 1.0f;
	float ogl_y = 1.0f - (2.0f * screen_y) / height;

	return { ogl_x, ogl_y };
}

void INIT_BUFFER() {
	SetupCubeVertex();
	SetupPyramidVertex();

	std::cout << "Setup Axis Completed\n";
	glGenVertexArrays(1, &VAO_axis);
	glGenBuffers(1, &VBO_axis);
	glGenBuffers(1, &IBO_axis);

	glBindVertexArray(VAO_axis);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_axis);

	glBufferData(GL_ARRAY_BUFFER, Axis_Vertex.size() * sizeof(Vertex_glm), Axis_Vertex.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex_glm), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex_glm), (GLvoid*)offsetof(Vertex_glm, color));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO_axis);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, Axis_Index.size() * sizeof(unsigned int), Axis_Index.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}
void UPDATE_BUFFER() {
	
}

char* filetobuf(const char* file)
{
	FILE* fptr;
	long length;
	char* buf;
	fptr = fopen(file, "rb");						// Open file for reading
	if (!fptr)										// Return NULL on failure
		return NULL;
	fseek(fptr, 0, SEEK_END);						// Seek to the end of the file
	length = ftell(fptr);							// Find out how many bytes into the file we are
	buf = (char*)malloc(length + 1);				// Allocate a buffer for the entire length of the file and a null terminator
	fseek(fptr, 0, SEEK_SET);						// Go back to the beginning of the file
	fread(buf, length, 1, fptr);					// Read the contents of the file in to the buffer
	fclose(fptr);									// Close the file
	buf[length] = 0;								// Null terminator
	return buf;										// Return the buffer
}
GLuint make_shaderProgram(const char* vertPath, const char* fragPath) {
	GLchar* vertexSource;
	GLchar* fragmentSource;

	vertexSource = filetobuf(vertPath);
	fragmentSource = filetobuf(fragPath);

	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	glCompileShader(vertexShader);
	free(vertexSource);
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
		std::cerr << "ERROR: vertex shader Compile Failed (" << vertPath << ")\n" << errorLog << std::endl;
		return 0;
	}

	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader);
	free(fragmentSource);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
		std::cerr << "ERROR: frag_shader Compile Failed (" << fragPath << ")\n" << errorLog << std::endl;
		return 0;
	}

	GLuint shaderID;
	shaderID = glCreateProgram();
	glAttachShader(shaderID, vertexShader);
	glAttachShader(shaderID, fragmentShader);
	glLinkProgram(shaderID);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	glGetProgramiv(shaderID, GL_LINK_STATUS, &result);
	if (!result) {
		glGetProgramInfoLog(shaderID, 512, NULL, errorLog);
		std::cerr << "ERROR: shader program link Failed\n" << errorLog << std::endl;
		return 0;
	}
	return shaderID;
}

void MakePerspactiveMatrix() {
	// showing view is rotated x axis by 30 degree, y axis by 30 degree and z axis by 0 degree

	Perspective_Matrix = glm::rotate(Perspective_Matrix, glm::radians(30.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	Perspective_Matrix = glm::rotate(Perspective_Matrix, glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	Perspective_Matrix = glm::rotate(Perspective_Matrix, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
}

void SetupPlane(int shape, int plane_loc) {
	// center at origin(0,0,0)
	// shape 0 : Cube, 1 : Pyramid
	// shape Cube -> plane_loc 1 : + dir YZ, 2 : - dir YZ, 3 : + dir XZ, 4 : - dir XZ, 5 : + dir XY, 6 : - dir XY
	// shape Pyramid -> plane_loc 1 : bottom plane - dir XY, 2 : side plane + dir YZ, 3 : side plane - dir YZ, 4 : side plane + dir XZ, 5 : side plane - dir XZ
	switch (shape) {
	case 0:
		// Cube
		if (Cube_Vertex.empty()) {
			SetupCubeVertex();
		}


		break;
	case 1:
		// Pyramid
		break;
	}
}
void SetupCubeVertex() {
	glm::vec3 cube_position[] = {
		// eight vertices of a cube | range : Shape_Range
		(-Shape_Range, -Shape_Range, -Shape_Range), // 0
		(Shape_Range, -Shape_Range, -Shape_Range), // 1
		(Shape_Range,  Shape_Range, -Shape_Range), // 2
		(-Shape_Range,  Shape_Range, -Shape_Range), // 3
		(-Shape_Range, -Shape_Range,  Shape_Range), // 4
		(Shape_Range, -Shape_Range,  Shape_Range), // 5
		(Shape_Range,  Shape_Range,  Shape_Range), // 6
		(-Shape_Range,  Shape_Range,  Shape_Range)  // 7
	}
}
void SetupPyramidVertex();