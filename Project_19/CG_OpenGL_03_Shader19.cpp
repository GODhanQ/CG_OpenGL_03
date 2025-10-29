#define _CRT_SECURE_NO_WARNINGS
#include "VarANDFunc_03_Shader19.h"

auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
std::default_random_engine dre(seed);
std::uniform_real_distribution<float> urd_0_1(0.0f, 1.0f);
std::uniform_int_distribution<int> uid_0_4(0, 4);

glm::mat4 View_Matrix(1.0f), Perspective_Matrix(1.0f), Ortho_Matrix(1.0f);
glm::mat4 Axis_ModelMatrix(1.0f);
glm::mat4 Sun_ModelMatrix(1.0f);
glm::mat4 Planet1_ModelMatrix(1.0f), Planet2_ModelMatrix(1.0f), Planet3_ModelMatrix(1.0f);
glm::mat4 Sat1_ModelMatrix(1.0f), Sat2_ModelMatrix(1.0f), Sat3_ModelMatrix(1.0f);
glm::mat4 GlobalTransform_Matrix(1.0f);

PlaneManager plane_manager;
std::vector<Vertex_glm> all_vertices;

// OBJ Data
std::vector<ModelData> model_set;
std::vector<ModelData> models;

// OBJ Filenames
std::vector<std::string> model_filenames = {
	"Cube_Obj.obj", "Wooden_House.obj", "Anime_Chara.obj", "Planet.obj",
};

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition(500, 50);
	glutInitWindowSize(Window_width, Window_height);
	glutCreateWindow("Example19");

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
	glEnable(GL_DEPTH_TEST);
	std::cout << "Setup GL_CULL_FACE Completed\n";

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	INIT_BUFFER();
	std::cout << "INIT BUFFER Completed\n";

	MakeStaticMatrix();

	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(KeyBoard);
	glutMouseFunc(MouseClick);
	glutIdleFunc(drawScene);

	glutMainLoop();
}

GLvoid drawScene() {
	GLfloat rColor{ 0.2f }, gColor{ 0.2f }, bColor{ 0.2f };
	glClearColor(rColor, gColor, bColor, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	MakeDynamicMatrix();
	plane_manager.PrepareBuffer();

	glUseProgram(shaderProgramID);

	ComposeUniformVar();

	// Draw Axis
	glBindVertexArray(VAO_axis);
	glLineWidth(1.0f);
	glUniform1i(FigureTypeID, Figure_Type::AXIS);
	glDrawElements(GL_LINES, Axis_Index.size(), GL_UNSIGNED_INT, 0);

	// Draw Planes
	glBindVertexArray(VAO);

	// OBJ Draw Planes 

	for (auto const& model : models) {
		glBindVertexArray(model.vao);
		glUniform1i(FigureTypeID, Figure_Type::CUSTOM_OBJ);
		glUniform1i(ModelIndexID, model.Model_Index);
		glDrawElements(GL_TRIANGLES, model.indices.size(), GL_UNSIGNED_INT, 0);
	}

	glBindVertexArray(0);

	glutSwapBuffers();
}
GLvoid Reshape(int w, int h) {
	glViewport(0, 0, w, h);
	if (h == 0) h = 1;
	float aspect = (float)w / (float)h;

	Perspective_Matrix = glm::perspective(glm::radians(60.0f), aspect, 0.1f, 100.0f);

	float ortho_height = 5.0f;
	float ortho_width = ortho_height * aspect;
	Ortho_Matrix = glm::ortho(-ortho_width, ortho_width, -ortho_height, ortho_height, 0.1f, 100.0f);

}


void KeyBoard(unsigned char key, int x, int y) {
	switch (key) {
	case 'p':
		Perspective_Flag = false;
		std::cout << "Ortho Projection Mode\n";
		break;
	case 'P':
		Perspective_Flag = true;
		std::cout << "Perspective Projection Mode\n";
		break;

	case 'm':
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		std::cout << "Fill Mode\n";
		break;
	case 'M':
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		std::cout << "Wireframe Mode\n";
		break;


	case 'w':
		Global_Translation.y += 0.1f;
		break;
	case 's':
		Global_Translation.y -= 0.1f;
		break;
	case 'a':
		Global_Translation.x -= 0.1f;
		break;
	case 'd':
		Global_Translation.x += 0.1f;
		break;
	case '+':
		Global_Translation.z += 0.5f;
		break;
	case '-':
		Global_Translation.z -= 0.5f;
		break;


	case 'y':
		Lv1_Radius += 0.5f; Lv2_Radius += 0.5f;
		std::cout << "Increase Orbit Radius: Lv1_Radius = " << Lv1_Radius << ", Lv2_Radius = " << Lv2_Radius << std::endl;
		break;
	case 'Y':
		Lv1_Radius -= 0.5f; Lv2_Radius -= 0.5f;
		std::cout << "Decrease Orbit Radius: Lv1_Radius = " << Lv1_Radius << ", Lv2_Radius = " << Lv2_Radius << std::endl;
		break;

	case 'z':
		Global_Rotation.z += 5.0f;
		break;
	case 'Z':
		Global_Rotation.z -= 5.0f;
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
	std::cout << "Setup OBJ Buffers...\n";
	for (const auto& name : model_filenames) {
		ModelData model;
		std::vector<glm::vec3> temp_positions;
		if (loadOBJ(name.c_str(), temp_positions, model.indices)) {
			std::cout << name << " 파일 로드 성공!" << std::endl;

			glm::vec3 color{ urd_0_1(dre), urd_0_1(dre), urd_0_1(dre) };
			model.name = name;
			for (const auto& pos : temp_positions) {
				model.vertices.push_back({ pos, color });
			}

			model_set.push_back(model);
		}
	}
	PushModels();
	ModelDivision();
	for (auto& model : models) {
		std::cout << "Model Index: " << model.Model_Index << ", Name: " << model.name << ", Vertices: " << model.vertices.size() << ", Indices: " << model.indices.size() << std::endl;
	}

	SetupVertices();

	plane_manager.planes.clear();
	SetupCube();
	SetupPyramid();

	MakeStaticMatrix();
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

void MakeStaticMatrix() {
	View_Matrix = glm::mat4(1.0f);
	glm::vec3 EYE(20.0f, 10.0f, 20.0f);
	glm::vec3 AT(0.0f, 0.0f, 0.0f);
	glm::vec3 UP(0.0f, 1.0f, 0.0f);
	glm::vec3 cameraDirection = glm::normalize(AT - EYE);
	glm::vec3 cameraRight = glm::normalize(glm::cross(UP, cameraDirection));
	glm::vec3 cameraUp = glm::cross(cameraDirection, cameraRight);
	View_Matrix =  glm::lookAt(EYE,	AT,	UP);
}
void MakeDynamicMatrix() {
	// Sun
	Sun_ModelMatrix = glm::mat4(1.0f);
	Sun_ModelMatrix = glm::scale(Sun_ModelMatrix, glm::vec3(Sun_scale, Sun_scale, Sun_scale));

	// --- Planet 1: 기울어진 궤도 (45도) ---
	Planet1_ModelMatrix = glm::mat4(1.0f);
	Planet1_Orbit_Angle += Lv1_Rotation_Speed;
	if (Planet1_Orbit_Angle >= 360.0f) Planet1_Orbit_Angle -= 360.0f;
	glm::mat4 tiltMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	glm::mat4 revolutionMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(Planet1_Orbit_Angle), glm::vec3(0.0f, 1.0f, 0.0f));
	float planet1_distance = Lv1_Radius;
	glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(planet1_distance, 0.0f, 0.0f));
	glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(planet_scale, planet_scale, planet_scale));
	Planet1_ModelMatrix = tiltMatrix * revolutionMatrix * translationMatrix * scaleMatrix;

	// --- Planet 2: 기울어진 궤도 (-45도) ---
	Planet2_ModelMatrix = glm::mat4(1.0f);
	Planet2_Orbit_Angle += Lv1_Rotation_Speed;
	if (Planet2_Orbit_Angle >= 360.0f) Planet2_Orbit_Angle -= 360.0f;
	tiltMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(-45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	revolutionMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(Planet2_Orbit_Angle), glm::vec3(0.0f, 1.0f, 0.0f));
	float planet2_distance = Lv1_Radius + 2.0f;
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(planet2_distance + 2.0f, 0.0f, 0.0f));
	scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(planet_scale, planet_scale, planet_scale));
	Planet2_ModelMatrix = tiltMatrix * revolutionMatrix * translationMatrix * scaleMatrix;


	// --- Planet 3: 일반적인 수평 궤도 (XZ 평면) ---
	Planet3_ModelMatrix = glm::mat4(1.0f);
	Planet3_Orbit_Angle += Lv1_Rotation_Speed;
	if (Planet3_Orbit_Angle >= 360.0f) Planet3_Orbit_Angle -= 360.0f;
	revolutionMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(Planet3_Orbit_Angle), glm::vec3(0.0f, 1.0f, 0.0f));
	float planet3_distance = Lv1_Radius + 4.0f;
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(planet3_distance, 0.0f, 0.0f));
	scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(planet_scale, planet_scale, planet_scale));
	Planet3_ModelMatrix = revolutionMatrix * translationMatrix * scaleMatrix;

	// --- Satellites 1: Planet 1 기준으로 공전 ---
	Sat1_ModelMatrix = Planet1_ModelMatrix;
	Sat1_ModelMatrix = glm::rotate(Sat1_ModelMatrix, glm::radians(Planet1_Orbit_Angle * 4.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	Sat1_ModelMatrix = glm::translate(Sat1_ModelMatrix, glm::vec3(Lv2_Radius, 0.0f, 0.0f));
	Sat1_ModelMatrix = glm::scale(Sat1_ModelMatrix, glm::vec3(satlite_scale, satlite_scale, satlite_scale));

	// --- Satellites 2: Planet 2 기준으로 공전 ---
	Sat2_ModelMatrix = Planet2_ModelMatrix;
	Sat2_ModelMatrix = glm::rotate(Sat2_ModelMatrix, glm::radians(Planet2_Orbit_Angle * 4.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	Sat2_ModelMatrix = glm::translate(Sat2_ModelMatrix, glm::vec3(Lv2_Radius, 0.0f, 0.0f));
	Sat2_ModelMatrix = glm::scale(Sat2_ModelMatrix, glm::vec3(satlite_scale, satlite_scale, satlite_scale));

	// --- Satellites 3: Planet 3 기준으로 공전 ---
	Sat3_ModelMatrix = Planet3_ModelMatrix;
	Sat3_ModelMatrix = glm::rotate(Sat3_ModelMatrix, glm::radians(Planet3_Orbit_Angle * 4.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	Sat3_ModelMatrix = glm::translate(Sat3_ModelMatrix, glm::vec3(Lv2_Radius, 0.0f, 0.0f));
	Sat3_ModelMatrix = glm::scale(Sat3_ModelMatrix, glm::vec3(satlite_scale, satlite_scale, satlite_scale));

	// Global Transform
	GlobalTransform_Matrix = glm::mat4(1.0f);
	GlobalTransform_Matrix = glm::rotate(GlobalTransform_Matrix, glm::radians(Global_Rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
	GlobalTransform_Matrix = glm::translate(GlobalTransform_Matrix, Global_Translation);
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
	const int bottom_plane_indices[6] = {
		4, 5, 6, 4, 6, 7,
	};

	for (int i = 0; i < 4; ++i) {
		int index = i + 7;
		if (index == 10) index = 0;
		Plane plane(index);
		plane.Figure_Type = Figure_Type::PYRAMID;
		for (int j = 0; j < 3; ++j) {
			plane.indices.push_back(plane_indices[i][j]);
		}

		plane.Enable();
		plane_manager.planes.push_back(plane);
	}

	Plane bottom_plane(11);
	bottom_plane.Figure_Type = Figure_Type::PYRAMID;
	for (int j = 0; j < 6; ++j) {
		bottom_plane.indices.push_back(bottom_plane_indices[j]);
	}

	bottom_plane.Enable();
	plane_manager.planes.push_back(bottom_plane);
}
bool loadOBJ(const char* path, std::vector<glm::vec3>& out_vertices, std::vector<unsigned int>& out_indices) {
	FILE* file = fopen(path, "r");
	if (file == NULL) {
		printf("파일을 열 수 없습니다! 경로를 확인하세요: %s\n", path);
		return false;
	}

	std::vector<glm::vec3> temp_vertices;
	std::vector<unsigned int> vertexIndices;

	while (1) {
		char lineHeader[128];
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break;

		if (strcmp(lineHeader, "v") == 0) {
			glm::vec3 vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			temp_vertices.push_back(vertex);
		}
		else if (strcmp(lineHeader, "f") == 0) {
			unsigned int vertexIndex[4], textureIndex[4], normalIndex[4];

			// 사각형 면(v/vt/vn 형식)을 읽기 시도
			int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d\n",
				&vertexIndex[0], &textureIndex[0], &normalIndex[0],
				&vertexIndex[1], &textureIndex[1], &normalIndex[1],
				&vertexIndex[2], &textureIndex[2], &normalIndex[2],
				&vertexIndex[3], &textureIndex[3], &normalIndex[3]);

			if (matches == 12) { // 사각형 면을 성공적으로 읽었을 경우
				// 첫 번째 삼각형
				out_indices.push_back(vertexIndex[0] - 1);
				out_indices.push_back(vertexIndex[1] - 1);
				out_indices.push_back(vertexIndex[2] - 1);
				// 두 번째 삼각형
				out_indices.push_back(vertexIndex[0] - 1);
				out_indices.push_back(vertexIndex[2] - 1);
				out_indices.push_back(vertexIndex[3] - 1);
			}
			else {
				// 사각형 읽기 실패 시, 삼각형 면(v/vt/vn 형식)을 읽기 시도
				fseek(file, -(static_cast<long>(strlen(lineHeader)) + 1), SEEK_CUR);
				matches = fscanf(file, "f %d/%d/%d %d/%d/%d %d/%d/%d\n",
					&vertexIndex[0], &textureIndex[0], &normalIndex[0],
					&vertexIndex[1], &textureIndex[1], &normalIndex[1],
					&vertexIndex[2], &textureIndex[2], &normalIndex[2]);

				if (matches == 9) { // 삼각형 면을 성공적으로 읽었을 경우
					out_indices.push_back(vertexIndex[0] - 1);
					out_indices.push_back(vertexIndex[1] - 1);
					out_indices.push_back(vertexIndex[2] - 1);
				}
				else {
					// 간단한 'f v1 v2 v3' 형식도 처리
					fseek(file, -(static_cast<long>(strlen(lineHeader)) + 1), SEEK_CUR);
					matches = fscanf(file, "f %d %d %d\n", &vertexIndex[0], &vertexIndex[1], &vertexIndex[2]);
					if (matches == 3) {
						out_indices.push_back(vertexIndex[0] - 1);
						out_indices.push_back(vertexIndex[1] - 1);
						out_indices.push_back(vertexIndex[2] - 1);
					}
					else {
						// 지원하지 않는 형식의 줄은 건너뜁니다.
						char buffer[1024];
						fgets(buffer, 1024, file);
					}
				}
			}
		}
		else {
			// 관련 없는 데이터(주석 등)가 있는 줄은 건너뜁니다.
			char buffer[1024];
			fgets(buffer, 1024, file);
		}
	}

	out_vertices = temp_vertices;
	fclose(file);
	return true;
}
void PushModels() {
	models.clear();

	std::cout << "Push Models...\n";
	for (auto const& model : model_set) {
		if (model.name == "Planet.obj") {
			for (int i = 0; i < 7; ++i) {
				glm::vec3 color{ urd_0_1(dre), urd_0_1(dre), urd_0_1(dre) };
				ModelData planet_model = model;
				for (auto& vertex : planet_model.vertices) {
					vertex.color = color;
				}

				glGenVertexArrays(1, &planet_model.vao);
				glGenBuffers(1, &planet_model.vbo);
				glGenBuffers(1, &planet_model.ibo);

				glBindVertexArray(planet_model.vao);

				glBindBuffer(GL_ARRAY_BUFFER, planet_model.vbo);
				glBufferData(GL_ARRAY_BUFFER, planet_model.vertices.size() * sizeof(Vertex_glm), planet_model.vertices.data(), GL_STATIC_DRAW);
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex_glm), (GLvoid*)0);
				glEnableVertexAttribArray(0);
				glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex_glm), (GLvoid*)offsetof(Vertex_glm, color));
				glEnableVertexAttribArray(1);

				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, planet_model.ibo);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, planet_model.indices.size() * sizeof(unsigned int), planet_model.indices.data(), GL_STATIC_DRAW);

				models.push_back(planet_model);
				std::cout << "Push Model: " << model.name << std::endl;

			}
		}
	}
}
void ModelDivision() {
	// model name 0 : Sun,
	// 1 : planet1,				2 : planet2,				3 : planet3
	// 4 : planet1's satellite,	5 : planet2's satellite,	6 : planet3's satellite

	int i = 0;
	for (auto& model : models) {
		model.Model_Index = i++;

	}
}


void ComposeUniformVar() {
	// Static Uniforms
	ViewTransformMatrixID = glGetUniformLocation(shaderProgramID, "View_Matrix");
	PerspectiveMatrixID = glGetUniformLocation(shaderProgramID, "Perspective_Matrix");
	OrthoMatrixID = glGetUniformLocation(shaderProgramID, "Ortho_Matrix");
	SunMatrixID = glGetUniformLocation(shaderProgramID, "Sun_ModelMatrix");
	Planet1MatrixID = glGetUniformLocation(shaderProgramID, "Planet1_ModelMatrix");
	Planet2MatrixID = glGetUniformLocation(shaderProgramID, "Planet2_ModelMatrix");
	Planet3MatrixID = glGetUniformLocation(shaderProgramID, "Planet3_ModelMatrix");
	Sat1MatrixID = glGetUniformLocation(shaderProgramID, "Sat1_ModelMatrix");
	Sat2MatrixID = glGetUniformLocation(shaderProgramID, "Sat2_ModelMatrix");
	Sat3MatrixID = glGetUniformLocation(shaderProgramID, "Sat3_ModelMatrix");
	PerspectiveFlagID = glGetUniformLocation(shaderProgramID, "Perspective_Flag");
	GlobalTransformMatrixID = glGetUniformLocation(shaderProgramID, "GlobalTransform_Matrix");

	glUniformMatrix4fv(ViewTransformMatrixID, 1, GL_FALSE, &View_Matrix[0][0]);
	glUniformMatrix4fv(PerspectiveMatrixID, 1, GL_FALSE, &Perspective_Matrix[0][0]);
	glUniformMatrix4fv(OrthoMatrixID, 1, GL_FALSE, &Ortho_Matrix[0][0]);
	glUniformMatrix4fv(SunMatrixID, 1, GL_FALSE, &Sun_ModelMatrix[0][0]);
	glUniformMatrix4fv(Planet1MatrixID, 1, GL_FALSE, &Planet1_ModelMatrix[0][0]);
	glUniformMatrix4fv(Planet2MatrixID, 1, GL_FALSE, &Planet2_ModelMatrix[0][0]);
	glUniformMatrix4fv(Planet3MatrixID, 1, GL_FALSE, &Planet3_ModelMatrix[0][0]);
	glUniformMatrix4fv(Sat1MatrixID, 1, GL_FALSE, &Sat1_ModelMatrix[0][0]);
	glUniformMatrix4fv(Sat2MatrixID, 1, GL_FALSE, &Sat2_ModelMatrix[0][0]);
	glUniformMatrix4fv(Sat3MatrixID, 1, GL_FALSE, &Sat3_ModelMatrix[0][0]);
	glUniform1i(PerspectiveFlagID, Perspective_Flag);
	glUniformMatrix4fv(GlobalTransformMatrixID, 1, GL_FALSE, &GlobalTransform_Matrix[0][0]);

	if (ViewTransformMatrixID == -1) std::cerr << "Could not bind View_Matrix uniform variable." << std::endl;
	if (PerspectiveMatrixID == -1) std::cerr << "Could not bind Perspective_Matrix uniform variable." << std::endl;
	if (OrthoMatrixID == -1) std::cerr << "Could not bind Ortho_Matrix uniform variable." << std::endl;
	if (SunMatrixID == -1) std::cerr << "Could not bind Sun_ModelMatrix uniform variable." << std::endl;
	if (Planet1MatrixID == -1) std::cerr << "Could not bind Planet1_ModelMatrix uniform variable." << std::endl;
	if (Planet2MatrixID == -1) std::cerr << "Could not bind Planet2_ModelMatrix uniform variable." << std::endl;
	if (Planet3MatrixID == -1) std::cerr << "Could not bind Planet3_ModelMatrix uniform variable." << std::endl;
	if (Sat1MatrixID == -1) std::cerr << "Could not bind Sat1_ModelMatrix uniform variable." << std::endl;
	if (Sat2MatrixID == -1) std::cerr << "Could not bind Sat2_ModelMatrix uniform variable." << std::endl;
	if (Sat3MatrixID == -1) std::cerr << "Could not bind Sat3_ModelMatrix uniform variable." << std::endl;
	if (PerspectiveFlagID == -1) std::cerr << "Could not bind Perspective_Flag uniform variable." << std::endl;
	if (GlobalTransformMatrixID == -1) std::cerr << "Could not bind GlobalTransform_Matrix uniform variable." << std::endl;
	
	// Dynamic Uniforms
	FigureTypeID = glGetUniformLocation(shaderProgramID, "Figure_Type");
	ModelIndexID = glGetUniformLocation(shaderProgramID, "Model_Index");

}
