#define _CRT_SECURE_NO_WARNINGS
#include "VarANDFunc_03_Shader16.h"

auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
std::default_random_engine dre(seed);
std::uniform_real_distribution<float> urd_0_1(0.0f, 1.0f);
std::uniform_int_distribution<int> uid_1_6(1, 6);
std::uniform_int_distribution<int> uid_7_10(7, 10);

GLuint shaderProgramID;
GLuint vertexShader;
GLuint fragmentShader;

GLuint VBO_axis, VAO_axis, IBO_axis;
GLuint VBO, VAO, IBO;

GLuint PerspectiveMatrixID, FigureTypeID, ComTransMatrixID;
glm::mat4 Perspective_Matrix(1.0f), ComTransMatrix(1.0f), UnitMatrix(1.0f);

PlaneManager plane_manager;

std::vector<Vertex_glm> all_vertices;

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition(500, 50);
	glutInitWindowSize(Window_width, Window_height);
	glutCreateWindow("Example16");

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

	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	std::cout << "Setup GL_CULL_FACE Completed\n";

	INIT_BUFFER();
	EnableAll(Figure_Type::CUBE);
	std::cout << "INIT BUFFER Completed\n";

	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(KeyBoard);
	glutSpecialFunc(SpecialKey);
	glutMouseFunc(MouseClick);
	glutIdleFunc(drawScene);

	glutMainLoop();
}

GLvoid drawScene() {
	GLfloat rColor{ 0.2f }, gColor{ 0.2f }, bColor{ 0.2f };
	glClearColor(rColor, gColor, bColor, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	MakeComTransMatrix();
	plane_manager.PrepareBuffer();

	glUseProgram(shaderProgramID);

	PerspectiveMatrixID = glGetUniformLocation(shaderProgramID, "Perspective_Matrix");
	glUniformMatrix4fv(PerspectiveMatrixID, 1, GL_FALSE, &Perspective_Matrix[0][0]);
	FigureTypeID = glGetUniformLocation(shaderProgramID, "Figure_Type");
	ComTransMatrixID = glGetUniformLocation(shaderProgramID, "ComTransMatrix");
	if (is_Traslate_Mode) {
		glUniformMatrix4fv(ComTransMatrixID, 1, GL_FALSE, &ComTransMatrix[0][0]);
	} else {
		glUniformMatrix4fv(ComTransMatrixID, 1, GL_FALSE, &UnitMatrix[0][0]);
	}

	// Draw Axis
	glBindVertexArray(VAO_axis);
	glUniform1i(FigureTypeID, 2);
	glLineWidth(1.0f);
	glDrawElements(GL_LINES, Axis_Index.size(), GL_UNSIGNED_INT, 0);

	// Draw Planes
	glBindVertexArray(VAO);
	glLineWidth(3.0f);
	size_t offset = 0;
	for (size_t i = 0; i < plane_manager.planes.size(); ++i) {
		if (plane_manager.planes[i].isSet) {
			glUniform1i(FigureTypeID, plane_manager.planes[i].Figure_Type);

			if (plane_manager.planes[i].indices.size() == 6)
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * offset));
			else if (plane_manager.planes[i].indices.size() == 3)
				glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * offset));
			else
				std::cout << "Error: plane indices size is not correct.\n";
		}
		offset += plane_manager.planes[i].indices.size();
	}

	glBindVertexArray(0);

	glutSwapBuffers();
}
GLvoid Reshape(int w, int h) {
	glViewport(0, 0, w, h);
}

void KeyBoard(unsigned char key, int x, int y) {
	switch (key) {
	case 'w':
	case 'W':
		// Draw type Wireframe on/off
		GLint polygonMode[2];
		glGetIntegerv(GL_POLYGON_MODE, polygonMode);

		if (polygonMode[0] == GL_LINE) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			std::cout << "GL_POLYGON_MODE : FILL\n";
		}
		else {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			std::cout << "GL_POLYGON_MODE : LINE\n";
		}

		glutPostRedisplay();
		break;
	case 'x':
		// rotate +X axis
		if (Rotate_Amount.x < 0) Rotate_Amount.x = 0;
		Rotate_Amount.x += 0.05f;

		std::cout << "Spin +X, Spin Amount : " << Rotate_Amount.x << std::endl;
		glutPostRedisplay();
		break;
	case 'X':
		// rotate -X axis
		if (Rotate_Amount.x > 0) Rotate_Amount.x = 0;
		Rotate_Amount.x -= 0.05f;

		std::cout << "Spin -X, Spin Amount : " << Rotate_Amount.x << std::endl;
		glutPostRedisplay();
		break;
	case 'y':
		// rotate +Y axis
		if (Rotate_Amount.y < 0) Rotate_Amount.y = 0;
		Rotate_Amount.y += 0.05f;

		std::cout << "Spin +Y, Spin Amount : " << Rotate_Amount.y << std::endl;
		glutPostRedisplay();
		break;
	case 'Y':
		// rotate -Y axis
		if (Rotate_Amount.y > 0) Rotate_Amount.y = 0;
		Rotate_Amount.y -= 0.05f;

		std::cout << "Spin -Y, Spin Amount : " << Rotate_Amount.y << std::endl;
		glutPostRedisplay();
		break;
	case 'c':
		DisableAll();
		EnableAll(Figure_Type::CUBE);

		std::cout << "Enable All Cube Planes\n";
		break;
	case 'p':
		DisableAll();
		EnableAll(Figure_Type::PYRAMID);

		std::cout << "Enable All Pyramid Planes\n";
		break;
	case 'u':
		// on/off back face culling
		if (glIsEnabled(GL_CULL_FACE)) {
			glDisable(GL_CULL_FACE);
			std::cout << "GL_CULL_FACE Disabled\n";
		}
		else {
			glEnable(GL_CULL_FACE);
			std::cout << "GL_CULL_FACE Enabled\n";
		}

		break;
	case 'h':
		// on/off depth test
		if (glIsEnabled(GL_DEPTH_TEST)) {
			glDisable(GL_DEPTH_TEST);
			std::cout << "GL_DEPTH_TEST Disabled\n";
		}
		else {
			glEnable(GL_DEPTH_TEST);
			std::cout << "GL_DEPTH_TEST Enabled\n";
		}

		break;
	case 's':
		Transform = glm::vec3(0.0f, 0.0f, 0.0f);
		Rotation_Angles = glm::vec3(0.0f, 0.0f, 0.0f);
		Rotate_Amount = glm::vec3(0.0f, 0.0f, 0.0f);
		Trans_Amount = glm::vec3(0.0f, 0.0f, 0.0f);
		ComTransMatrix = glm::mat4(1.0f);
		
		std::cout << "Reset Transformations\n";
		glutPostRedisplay();
		break;
	case 't':
		is_Traslate_Mode = !is_Traslate_Mode;

		std::cout << "Translate Mode : " << (is_Traslate_Mode ? "On" : "Off") << std::endl;
		break;
	case 'q':
		exit(0);
	}
}
void SpecialKey(int key, int x, int y) {
	switch (key) {
	case GLUT_KEY_UP:
		// move +Y axis
		Trans_Amount.y = 0;
		Trans_Amount.y += 0.004f;
		std::cout << "Move +Y, Move Amount: " << Trans_Amount.y << std::endl;

		glutPostRedisplay();
		break;
	case GLUT_KEY_DOWN:
		// move -Y axis
		Trans_Amount.y = 0;
		Trans_Amount.y -= 0.004f;
		std::cout << "Move -Y, Move Amount : " << Trans_Amount.y << std::endl;

		glutPostRedisplay();
		break;

	case GLUT_KEY_LEFT:
		// move -X axis
		Trans_Amount.x = 0;
		Trans_Amount.x += 0.004f;
		std::cout << "Move -X, Move Amount : " << Trans_Amount.x << std::endl;

		glutPostRedisplay();
		break;
	case GLUT_KEY_RIGHT:
		// move +X axis
		Trans_Amount.y = 0;
		Trans_Amount.x -= 0.004f;
		std::cout << "Move +X, Move Amount : " << Trans_Amount.x << std::endl;

		glutPostRedisplay();
		break;
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
	SetupVertices();

	plane_manager.planes.clear();
	SetupCube();
	SetupPyramid();

	MakePerspactiveMatrix();
	plane_manager.PrepareBuffer();

	std::cout << "Setup Axis...\n";
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

	std::cout << "Setup Planes...\n";
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &IBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, all_vertices.size() * sizeof(Vertex_glm), all_vertices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex_glm), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex_glm), (GLvoid*)offsetof(Vertex_glm, color));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, plane_manager.all_indices.size() * sizeof(unsigned int), plane_manager.all_indices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}
void UPDATE_BUFFER() {
	glBindVertexArray(VAO);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, plane_manager.all_indices.size() * sizeof(unsigned int), plane_manager.all_indices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
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

	Perspective_Matrix = glm::scale(Perspective_Matrix, glm::vec3(0.5f, 0.5f, 0.5f));
	Perspective_Matrix = glm::rotate(Perspective_Matrix, glm::radians(-30.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	Perspective_Matrix = glm::rotate(Perspective_Matrix, glm::radians(120.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	Perspective_Matrix = glm::rotate(Perspective_Matrix, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
}
void MakeComTransMatrix() {
	Transform += Trans_Amount;
	Rotation_Angles += Rotate_Amount;

	if (Rotation_Angles.x >= 360.0f) Rotation_Angles.x -= 360.0f;
	if (Rotation_Angles.y >= 360.0f) Rotation_Angles.y -= 360.0f;
	if (Rotation_Angles.z >= 360.0f) Rotation_Angles.z -= 360.0f;

	// 1. 이동 행렬을 먼저 만듭니다.
	glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), Transform);

	// 2. 회전 행렬을 만듭니다.
	glm::mat4 rotationMatrix = glm::mat4(1.0f);
	rotationMatrix = glm::rotate(rotationMatrix, glm::radians(Rotation_Angles.x), glm::vec3(1.0f, 0.0f, 0.0f));
	rotationMatrix = glm::rotate(rotationMatrix, glm::radians(Rotation_Angles.y), glm::vec3(0.0f, 1.0f, 0.0f));
	rotationMatrix = glm::rotate(rotationMatrix, glm::radians(Rotation_Angles.z), glm::vec3(0.0f, 0.0f, 1.0f));

	// 3. 최종 변환 행렬 = 이동 * 회전
	// 이 순서는 정점에 회전을 먼저 적용(제자리 회전)하고, 그 다음 이동시킵니다.
	ComTransMatrix = translationMatrix * rotationMatrix;
}

void SetupVertices() {
	all_vertices.clear();
	std::vector<Vertex_glm> temp_vertices;

	for (const auto& pos : All_Vertices) {
		Vertex_glm vertex;
		vertex.position = pos;
		vertex.color = glm::vec3(urd_0_1(dre), urd_0_1(dre), urd_0_1(dre));

		temp_vertices.push_back(vertex);
	}

	all_vertices = temp_vertices;
}
void SetupCube() {
	// make 6 planes
	// center at origin(0,0,0)
	// plane_loc 1 : +X axis and YZ plane, 2 : -X axis and YZ plane, 3 : +Y axis and XZ plane, 4 : -Y axis and XZ plane
	// plane_loc 5 : +Z axis and XY plane, 6 : -Z axis and XY plane
	const int plane_indices[6][6] = {
		{ 0, 4, 7, 0, 7, 3 },		// +X Plane
		{ 2, 6, 5, 2, 5, 1 },		// -X Plane
		{ 2, 1, 0, 2, 0, 3 },		// +Y Plane
		{ 5, 6, 7, 5, 7, 4 },		// -Y Plane
		{ 1, 5, 4, 1, 4, 0 },		// +Z Plane
		{ 3, 7, 6, 3, 6, 2 },		// -Z Plane
	};


	for (int i = 0; i < 6; ++i) {
		Plane plane(i + 1);
		plane.Figure_Type = Figure_Type::CUBE;
		for (int j = 0; j < 6; ++j) {
			//plane.vertices.push_back(all_vertices[plane_indices[i][j]]);
			plane.indices.push_back(plane_indices[i][j]);
		}

		//plane.Disable();
		plane.Enable();
		plane_manager.planes.push_back(plane);
	}
}
void SetupPyramid() {
	// make 4 planes
	// center at origin(0,0,0)
	// plane_loc 1 : facing +X axis, 2 : facing -X axis, 3 : facing +Y axis, 4 : facing -Y axis

	const int plane_indices[4][3] = {
		{ 8, 4, 7 },		// +X Plane
		{ 8, 6, 5 },		// -X Plane
		{ 8, 5, 4 },		// +Y Plane
		{ 8, 7, 6 },		// -Y Plane
	};

	for (int i = 0; i < 4; ++i) {
		int index = i + 7;
		if (index == 10) index = 0;
		Plane plane(index);
		plane.Figure_Type = Figure_Type::PYRAMID;
		for (int j = 0; j < 3; ++j) {
			//plane.vertices.push_back(all_vertices[plane_indices[i][j]]);
			plane.indices.push_back(plane_indices[i][j]);
		}

		//plane.Disable();
		plane.Enable();
		plane_manager.planes.push_back(plane);
	}
}
void DisableAll() {
	for (auto& plane : plane_manager.planes) {
		plane.Disable();
	}
}
void EnableAll() {
	for (auto& plane : plane_manager.planes) {
		plane.Enable();
	}
}
void EnableAll(int figure_type) {
	for (auto& plane : plane_manager.planes) {
		if (plane.Figure_Type == figure_type) {
			plane.Enable();
		}
		else {
			plane.Disable();
		}
	}
}