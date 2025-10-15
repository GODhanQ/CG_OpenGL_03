#define _CRT_SECURE_NO_WARNINGS
#include "VarANDFunc_03_Shader15.h"

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

GLuint PerspectiveMatrixID;
glm::mat4 Perspective_Matrix(1.0f);

PlaneManager plane_manager;

std::vector<Vertex_glm> all_vertices;

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

	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	//glEnable(GL_CULL_FACE);
	std::cout << "Setup GL_CULL_FACE Completed\n";

	INIT_BUFFER();
	std::cout << "INIT BUFFER Completed\n";

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

	plane_manager.PrepareBuffer();

	PerspectiveMatrixID = glGetUniformLocation(shaderProgramID, "Perspective_Matrix");
	glUniformMatrix4fv(PerspectiveMatrixID, 1, GL_FALSE, &Perspective_Matrix[0][0]);

	glUseProgram(shaderProgramID);

	// Draw Axis
	glBindVertexArray(VAO_axis);
	glLineWidth(3.0f);
	glDrawElements(GL_LINES, Axis_Index.size(), GL_UNSIGNED_INT, 0);

	// Draw Planes
	glBindVertexArray(VAO);

	size_t offset = 0; // 오프셋 누적 변수
	for (size_t i = 0; i < plane_manager.planes.size(); ++i) {
		if (plane_manager.planes[i].isSet) {
			// i를 평면의 고유 인덱스로 사용하여 오프셋 계산
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
		// Cube
		// 1,2 for Z axis
	case '1':
	case '2':
		// 3,4 for Y axis
	case '3':
	case '4':
		// 5,6 for X axis
	case '5':
	case '6': {
		int index = key - '1';
		if (index >= 0 && index < plane_manager.planes.size()) {
			plane_manager.planes[index].isSet = !plane_manager.planes[index].isSet;
			std::cout << "Cube Plane " << key << (plane_manager.planes[index].isSet ? " Enabled\n" : " Disabled\n");
		}
		break;
	}
		// Pyramid
		// 7,8 for X axis
	case '7':
	case '8':
		// 9,0 for y axis
	case '9':
	case '0': {
		int index = -1;
		if (key >= '7' && key <= '9') {
			index = key - '7' + 6; // '7'~'9' -> index 6~8
		}
		else if (key == '0') {
			index = 9; // '0' -> index 9
		}

		if (index != -1 && index < plane_manager.planes.size()) {
			plane_manager.planes[index].isSet = !plane_manager.planes[index].isSet;
			std::cout << "Pyramid Plane " << (index - 5) << (plane_manager.planes[index].isSet ? " Enabled\n" : " Disabled\n");
		}
		break;
	}
	case 'c':
		DisableAll();
		glm::vec2 rand_number = { uid_1_6(dre), uid_1_6(dre) };
		KeyBoard(rand_number.x + '0', 0, 0);
		KeyBoard(rand_number.y + '0', 0, 0);

		break;
	case 't':
		DisableAll();
		rand_number = { uid_7_10(dre), 4 };
		if (rand_number.x == 10) rand_number.x = 0;

		KeyBoard(rand_number.x + '0', 0, 0);
		KeyBoard(rand_number.y + '0', 0, 0);
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
	SetupVertices();
	SetupCube();
	SetupPyramid();

	MakePerspactiveMatrix();
	plane_manager.PrepareBuffer();

	DisableAll();

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


	plane_manager.planes.clear();
	for (int i = 0; i < 6; ++i) {
		Plane plane(i + 1);
		for (int j = 0; j < 6; ++j) {
			//plane.vertices.push_back(all_vertices[plane_indices[i][j]]);
			plane.indices.push_back(plane_indices[i][j]);
		}

		//plane.Disable();
		plane.Enable();
		plane_manager.planes.push_back(plane);
	}
	
	//plane_manager.planes[0].Enable();
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