#define _CRT_SECURE_NO_WARNINGS
#include "VarANDFunc_03_Shader20.h"

auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
std::default_random_engine dre(seed);
std::uniform_real_distribution<float> urd_0_1(0.0f, 1.0f);
std::uniform_int_distribution<int> uid_0_3(0, 3);

glm::mat4 Perspective_Matrix(1.0f), View_Matrix;
glm::mat4 Model_Matrix(1.0f);
glm::mat4 Body_Matrix(1.0f), Neck_Matrix(1.0f), Head1_Matrix(1.0f), Head2_Matrix(1.0f);
glm::mat4 Mouth1_Matrix(1.0f), Mouth2_Matrix(1.0f), Flag1_Matrix(1.0f), Flag2_Matrix(1.0f);

std::vector<OBJ_File> g_OBJ_Files;

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition(500, 50);
	glutInitWindowSize(Window_width, Window_height);
	glutCreateWindow("Example20");

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
	std::cout << "Setup GL_POLYGON_MODE Completed\n";

	INIT_BUFFER();
	std::cout << "INIT BUFFER Completed\n";

	MakeStaticMatrix();
	std::cout << "Make Static Matrix Completed\n";

	ComposeUniformVar();
	std::cout << "Compose Uniform Variable Completed\n";

	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(KeyBoard);
	glutSpecialFunc(SpecialKeyBoard);
	glutMouseFunc(MouseClick);
	glutIdleFunc(drawScene);

	glutMainLoop();
}

GLvoid drawScene() {
	GLfloat rColor{ 0.5f }, gColor{ 0.5f }, bColor{ 0.7f };
	glClearColor(rColor, gColor, bColor, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(shaderProgramID);
	MakeDynamicMatrix();

	ComposeUniformVar();

	// Draw Axis
	glBindVertexArray(VAO_axis);
	glUniform1i(FigureTypeID, Figure_Type::AXIS);
	glLineWidth(2.0f);
	glDrawElements(GL_LINES, Axis_Index.size(), GL_UNSIGNED_INT, 0);

	// Draw Figures
	for (const auto& file : g_OBJ_Files) {
		for (const auto& object : file.objects) {
			glBindVertexArray(object.VAO);
			const int Figure_Type_ID = 
				(object.name == "Floor") ? Figure_Type::FLOOR :
				(object.name == "body.001") ? Figure_Type::BODY :
				(object.name == "neck.001") ? Figure_Type::NECK :
				(object.name == "head_1.001") ? Figure_Type::HEAD_1 :
				(object.name == "head_2.001") ? Figure_Type::HEAD_2 :
				(object.name == "mouth_1.001") ? Figure_Type::MOUTH_1 :
				(object.name == "mouth_2.001") ? Figure_Type::MOUTH_2 :
				(object.name == "flag_1.001") ? Figure_Type::FLAG_1 :
				(object.name == "flag_2.001") ? Figure_Type::FLAG_2 :
				Figure_Type::IDK;

			glUniform1i(FigureTypeID, Figure_Type_ID);
			glDrawElements(GL_TRIANGLES, object.indices.size(), GL_UNSIGNED_INT, 0);
		}
	}
	

	glBindVertexArray(0);

	glutSwapBuffers();
}
GLvoid Reshape(int w, int h) {
	glViewport(0, 0, w, h);
	MakeStaticMatrix();
	glUniformMatrix4fv(PerspectiveMatrixID, 1, GL_FALSE, &Perspective_Matrix[0][0]);
}

void KeyBoard(unsigned char key, int x, int y) {
	switch (key) {
	// model part rotations
	case 't':
		Neck_Rotate_Factor.y += 5.0f;
		break;
	case 'T':
		Neck_Rotate_Factor.y -= 5.0f;
		break;
	case 'g':
		Head_Rotate_Factor.y += 5.0f;
		break;
	case 'G':
		Head_Rotate_Factor.y -= 5.0f;
		break;
	case 'p':
		Flag_Rotate_Factor.x += 5.0f;
		break;
	case 'P':
		Flag_Rotate_Factor.x -= 5.0f;
		break;

	// camera actions
	case 'x':
		EYE.x += 1.0f;
		AT.x += 1.0f;
		MakeStaticMatrix();
		break;
	case 'X':
		EYE.x -= 1.0f;
		AT.x -= 1.0f;
		MakeStaticMatrix();
		break;
	case 'z':
		EYE.z += 1.0f;
		AT.z += 1.0f;
		MakeStaticMatrix();
		break;
	case 'Z':
		EYE.z -= 1.0f;
		AT.z -= 1.0f;
		MakeStaticMatrix();
		break;

	case 'y':
		// Rotate AT based on EYE
		// Camera rotation
		Camera_Rotation_Factor += 0.03f;

		break;
	case 'Y':
		// Rotate AT based on EYE, reverse direction
		// Camera rotation
		Camera_Rotation_Factor -= 0.03f;

		break;
	case 'r':
		// Rotate EYE around AT
		// Camera revolution
		Camera_Revolution_Factor += 0.03f;

		break;
	case 'R':
		// Rotate EYE around AT, reverse direction
		// Camera revolution
		Camera_Revolution_Factor -= 0.03f;

		break;

	case 'l':
		Rotating_Head = !Rotating_Head;

		std::cout << "Rotating Head: " << (Rotating_Head ? "ON" : "OFF") << std::endl;
		break;

	case 'c':
		// reset All
		Model_Movement_Factor = glm::vec3(0.0f, 0.0f, 0.0f);
		Neck_Rotate_Factor = glm::vec3(0.0f, 0.0f, 0.0f);
		Head_Rotate_Factor = glm::vec3(0.0f, 0.0f, 0.0f);
		Flag_Rotate_Factor = glm::vec3(0.0f, 0.0f, 0.0f);
		EYE = glm::vec3(15.0, 10.0, 15.0);
		AT = glm::vec3(0.0, 0.0, 0.0);
		Camera_Rotation_Sum = 0.0f; Camera_Rotation_Factor = 0.0f;
		Camera_Revolution_Sum = 0.0f; Camera_Revolution_Factor = 0.0f;
		Rotating_Head = false;
		Head_Movement_Factor_Param = 0.0f;
		MakeStaticMatrix();
		break;
	case 'o':
		// reset Factor
		Camera_Rotation_Factor = 0.0f;
		Camera_Revolution_Factor = 0.0f;
		Rotating_Head = false;
		break;
	// exit
	case 'q':
		exit(0);
	}
}
void SpecialKeyBoard(int key, int x, int y) {
	switch (key) {
	case GLUT_KEY_UP:
		Model_Movement_Factor.x -= 1.0f;

		break;
	case GLUT_KEY_DOWN:
		Model_Movement_Factor.x += 1.0f;

		break;
	case GLUT_KEY_LEFT:
		Model_Movement_Factor.z += 1.0f;

		break;
	case GLUT_KEY_RIGHT:
		Model_Movement_Factor.z -= 1.0f;

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
	std::vector<std::string> obj_filenames = {
		"Floor.obj",
		"body.obj",
		"neck.obj",
		"head.obj"
	};

	for (const auto& filename : obj_filenames) {
		OBJ_File objFile;
		if (ReadObj(filename, objFile)) {
			std::cout << "Loaded OBJ File: " << filename << " with " << objFile.objects.size() << " objects.\n";
			g_OBJ_Files.push_back(objFile);
		}
		else {
			std::cerr << "Failed to load OBJ File: " << filename << "\n";
		}
	}
	ComposeOBJColor();
	for (auto& file : g_OBJ_Files) {
		for (auto& object : file.objects) {
			glGenVertexArrays(1, &object.VAO);
			glGenBuffers(1, &object.VBO);
			glGenBuffers(1, &object.IBO);

			glBindVertexArray(object.VAO);

			glBindBuffer(GL_ARRAY_BUFFER, object.VBO);
			glBufferData(GL_ARRAY_BUFFER, object.vertices.size() * sizeof(Vertex_glm), object.vertices.data(), GL_STATIC_DRAW);

			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex_glm), (GLvoid*)offsetof(Vertex_glm, position));
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex_glm), (GLvoid*)offsetof(Vertex_glm, color));
			glEnableVertexAttribArray(1);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, object.IBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, object.indices.size() * sizeof(unsigned int), object.indices.data(), GL_STATIC_DRAW);

			std::cout << "  L  Buffered Object: " << object.name << ", VAO: " << object.VAO << "\n";
		}
	}
	glBindVertexArray(0);


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
bool ReadObj(const std::string& path, OBJ_File& outFile) {
	outFile.file_name = path;
	outFile.objects.clear();
	std::string real_path = "OBJ/" + path;


	std::vector<glm::vec3> temp_vertices;
	std::vector<glm::vec2> temp_uvs;
	std::vector<glm::vec3> temp_normals;

	FILE* file = fopen(real_path.c_str(), "r");
	if (file == NULL) {
		printf("Impossible to open the file !\n");
		return false;
	}

	Custom_OBJ* currentObject = nullptr;

	while (1) {
		char lineHeader[128];
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF) {
			break;
		}

		if (strcmp(lineHeader, "o") == 0) {
			char objectName[128];
			fscanf(file, "%s\n", objectName);
			outFile.objects.emplace_back();
			currentObject = &outFile.objects.back();
			currentObject->name = objectName;
		}
		else if (strcmp(lineHeader, "v") == 0) {
			glm::vec3 vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			temp_vertices.push_back(vertex);
		}
		else if (strcmp(lineHeader, "vt") == 0) {
			glm::vec2 uv;
			fscanf(file, "%f %f\n", &uv.x, &uv.y);
			temp_uvs.push_back(uv);
		}
		else if (strcmp(lineHeader, "vn") == 0) {
			glm::vec3 normal;
			fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
			temp_normals.push_back(normal);
		}
		else if (strcmp(lineHeader, "f") == 0) {
			if (currentObject == nullptr) {
				// 'o' �±� ���� 'f'�� ���� ������ ��츦 ����� �⺻ ��ü ����
				outFile.objects.emplace_back();
				currentObject = &outFile.objects.back();
				currentObject->name = "default_object";
			}

			unsigned int vertexIndex[4], uvIndex[4], normalIndex[4];
			int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d\n",
				&vertexIndex[0], &uvIndex[0], &normalIndex[0],
				&vertexIndex[1], &uvIndex[1], &normalIndex[1],
				&vertexIndex[2], &uvIndex[2], &normalIndex[2],
				&vertexIndex[3], &uvIndex[3], &normalIndex[3]);

			auto processFace = [&](int count) {
				for (int i = 0; i < count; ++i) {
					Vertex_glm vertex;
					vertex.position = temp_vertices[vertexIndex[i] - 1];
					// UV, Normal ������ �ִٸ� ���⿡ �߰��� �� �ֽ��ϴ�.
					// vertex.uv = temp_uvs[uvIndex[i] - 1];
					// vertex.normal = temp_normals[normalIndex[i] - 1];
					vertex.color = glm::vec3(1.0f, 1.0f, 1.0f); // �⺻ ����

					currentObject->vertices.push_back(vertex);
					currentObject->indices.push_back(currentObject->vertices.size() - 1);
				}
				};

			if (matches == 9) { // �ﰢ��
				processFace(3);
			}
			else if (matches == 12) { // �簢�� -> �ﰢ�� 2���� ����
				unsigned int v_indices[] = { 0, 1, 2, 0, 2, 3 };
				for (int i = 0; i < 6; ++i) {
					int idx = v_indices[i];
					Vertex_glm vertex;
					vertex.position = temp_vertices[vertexIndex[idx] - 1];
					vertex.color = glm::vec3(1.0f, 1.0f, 1.0f);
					currentObject->vertices.push_back(vertex);
					currentObject->indices.push_back(currentObject->vertices.size() - 1);
				}
			}
			else {
				// �ٸ� ������ �� �����ʹ� ���� �ļ����� �������� ����
				char buffer[1024];
				fgets(buffer, 1024, file); // �ش� ������ �������� �а� ����
			}
		}
		else {
			// �ּ� �Ǵ� �������� �ʴ� ���� ��ŵ
			char buffer[1024];
			fgets(buffer, 1024, file);
		}
	}

	fclose(file);
	return true;
}

void MakeStaticMatrix() {
	Perspective_Matrix = glm::perspective(glm::radians(45.0f), (float)Window_width / (float)Window_height, 0.1f, 100.0f);
}
void MakeDynamicMatrix() {
	static glm::vec3 initialEYE = glm::vec3(15.0f, 10.0f, 15.0f);
	static float distanceEyeToAT = glm::distance(initialEYE, glm::vec3(0.0f, 0.0f, 0.0f));
	static float distanceATToOrigin = glm::distance(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f));

	if (Camera_Rotation_Factor != 0.0f) {
		glm::vec3 direction = glm::normalize(EYE - AT);

		glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f),
			glm::radians(Camera_Rotation_Factor),
			glm::vec3(0.0f, 1.0f, 0.0f));

		glm::vec4 rotatedDirection = rotationMatrix * glm::vec4(direction, 0.0f);
		EYE = AT + glm::normalize(glm::vec3(rotatedDirection)) * distanceEyeToAT;
	}

	if (Camera_Revolution_Factor != 0.0f) {
		glm::vec3 direction = glm::normalize(AT - EYE);

		glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f),
			glm::radians(Camera_Revolution_Factor),
			glm::vec3(0.0f, 1.0f, 0.0f));

		glm::vec4 rotatedDirection = rotationMatrix * glm::vec4(direction, 0.0f);
		AT = EYE + glm::normalize(glm::vec3(rotatedDirection)) * distanceEyeToAT;
	}

	View_Matrix = glm::mat4(1.0f);
	View_Matrix = glm::lookAt(EYE, AT, UP);

	Model_Matrix = glm::mat4(1.0f);
	Model_Matrix = glm::translate(Model_Matrix, Model_Movement_Factor);
	
	Body_Matrix = glm::mat4(1.0f);

	Neck_Matrix = glm::mat4(1.0f);
	Neck_Matrix = glm::rotate(Neck_Matrix, glm::radians(Neck_Rotate_Factor.y), glm::vec3(0.0f, 1.0f, 0.0f));

	if (Rotating_Head) {
		Head_Movement_Factor_Param += 0.0002f;
		if (Head_Movement_Factor_Param > 1.0f)
			Head_Movement_Factor_Param = 0.0f;
	}
	glm::vec3 boundaryPos = glm::vec3(0.0f, 0.0f, 0.0f);

	if (Rotating_Head) {
		float param = Head_Movement_Factor_Param;  // 0.0 ~ 1.0

		// �� ���� ���� ����
		float side1_len = 1.4f;  // Z�� ���� (0.7*2)
		float side2_len = 3.0f;  // X�� ���� ���� (1.5*2)
		float side3_len = 1.4f;  // Z�� ���� ����
		float side4_len = 3.0f;  // X�� ���� ����
		float total_len = side1_len + side2_len + side3_len + side4_len;  // 8.8

		// ���� ���� ����
		float t1 = side1_len / total_len;           // 0.159
		float t2 = (side1_len + side2_len) / total_len;  // 0.500
		float t3 = (side1_len + side2_len + side3_len) / total_len;  // 0.659

		if (param <= t1) {
			// �� 1: (1.5, 0.7) �� (1.5, -0.7)
			float localT = param / t1;
			boundaryPos.x = 1.5f;
			boundaryPos.z = glm::mix(0.7f, -0.7f, localT);
		}
		else if (param <= t2) {
			// �� 2: (1.5, -0.7) �� (-1.5, -0.7)
			float localT = (param - t1) / (t2 - t1);
			boundaryPos.x = glm::mix(1.5f, -1.5f, localT);
			boundaryPos.z = -0.7f;
		}
		else if (param <= t3) {
			// �� 3: (-1.5, -0.7) �� (-1.5, 0.7)
			float localT = (param - t2) / (t3 - t2);
			boundaryPos.x = -1.5f;
			boundaryPos.z = glm::mix(-0.7f, 0.7f, localT);
		}
		else {
			// �� 4: (-1.5, 0.7) �� (1.5, 0.7)
			float localT = (param - t3) / (1.0f - t3);
			boundaryPos.x = glm::mix(-1.5f, 1.5f, localT);
			boundaryPos.z = 0.7f;
		}
	}

	Head1_Matrix = Neck_Matrix;
	if (Rotating_Head) {
		Head1_Matrix = glm::translate(Head1_Matrix, glm::vec3(-1.5, 0.0, 0.0));
		Head1_Matrix = glm::translate(Head1_Matrix, boundaryPos);
	}
	Head1_Matrix = glm::translate(Head1_Matrix, glm::vec3(1.5f, 0.0f, 0.0f));
	Head1_Matrix = glm::rotate(Head1_Matrix, glm::radians(Head_Rotate_Factor.y), glm::vec3(0.0f, 1.0f, 0.0f));
	Head1_Matrix = glm::translate(Head1_Matrix, glm::vec3(-1.5f, 0.0f, 0.0f));

	Head2_Matrix = Neck_Matrix;
	if (Rotating_Head) {
		Head2_Matrix = glm::translate(Head2_Matrix, glm::vec3(1.5, 0.0, 0.0));
		Head2_Matrix = glm::translate(Head2_Matrix, -boundaryPos);
	}
	Head2_Matrix = glm::translate(Head2_Matrix, glm::vec3(-1.5f, 0.0f, 0.0f));
	Head2_Matrix = glm::rotate(Head2_Matrix, glm::radians(-Head_Rotate_Factor.y), glm::vec3(0.0f, 1.0f, 0.0f));
	Head2_Matrix = glm::translate(Head2_Matrix, glm::vec3(1.5f, 0.0f, 0.0f));

	Mouth1_Matrix = Head1_Matrix;
	
	Mouth2_Matrix = Head2_Matrix;

	Flag1_Matrix = Head1_Matrix;
	Flag1_Matrix = glm::translate(Flag1_Matrix, glm::vec3(1.5, 1.75f, 0.0f));
	Flag1_Matrix = glm::rotate(Flag1_Matrix, glm::radians(Flag_Rotate_Factor.x), glm::vec3(1.0f, 0.0f, 0.0f));
	Flag1_Matrix = glm::translate(Flag1_Matrix, glm::vec3(-1.5, -1.75f, 0.0f));

	Flag2_Matrix = Head2_Matrix;
	Flag2_Matrix = glm::translate(Flag2_Matrix, glm::vec3(-1.5, 1.75f, 0.0f));
	Flag2_Matrix = glm::rotate(Flag2_Matrix, glm::radians(-Flag_Rotate_Factor.x), glm::vec3(1.0f, 0.0f, 0.0f));
	Flag2_Matrix = glm::translate(Flag2_Matrix, glm::vec3(1.5, -1.75f, 0.0f));
}

void ComposeUniformVar() {
	// static uniform variable
	PerspectiveMatrixID = glGetUniformLocation(shaderProgramID, "Perspective_Matrix");
	ViewMatrixID = glGetUniformLocation(shaderProgramID, "View_Matrix");
	ModelMatrixID = glGetUniformLocation(shaderProgramID, "Model_Matrix");
	BodyMatrixID = glGetUniformLocation(shaderProgramID, "Body_Matrix");
	NeckMatrixID = glGetUniformLocation(shaderProgramID, "Neck_Matrix");
	Head1MatrixID = glGetUniformLocation(shaderProgramID, "Head1_Matrix");
	Head2MatrixID = glGetUniformLocation(shaderProgramID, "Head2_Matrix");
	Mouth1MatrixID = glGetUniformLocation(shaderProgramID, "Mouth1_Matrix");
	Mouth2MatrixID = glGetUniformLocation(shaderProgramID, "Mouth2_Matrix");
	Flag1MatrixID = glGetUniformLocation(shaderProgramID, "Flag1_Matrix");
	Flag2MatrixID = glGetUniformLocation(shaderProgramID, "Flag2_Matrix");

	glUniformMatrix4fv(PerspectiveMatrixID, 1, GL_FALSE, &Perspective_Matrix[0][0]);
	glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &View_Matrix[0][0]);
	glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &Model_Matrix[0][0]);
	glUniformMatrix4fv(BodyMatrixID, 1, GL_FALSE, &Body_Matrix[0][0]);
	glUniformMatrix4fv(NeckMatrixID, 1, GL_FALSE, &Neck_Matrix[0][0]);
	glUniformMatrix4fv(Head1MatrixID, 1, GL_FALSE, &Head1_Matrix[0][0]);
	glUniformMatrix4fv(Head2MatrixID, 1, GL_FALSE, &Head2_Matrix[0][0]);
	glUniformMatrix4fv(Mouth1MatrixID, 1, GL_FALSE, &Mouth1_Matrix[0][0]);
	glUniformMatrix4fv(Mouth2MatrixID, 1, GL_FALSE, &Mouth2_Matrix[0][0]);
	glUniformMatrix4fv(Flag1MatrixID, 1, GL_FALSE, &Flag1_Matrix[0][0]);
	glUniformMatrix4fv(Flag2MatrixID, 1, GL_FALSE, &Flag2_Matrix[0][0]);

	if (PerspectiveMatrixID == -1) std::cerr << "Could not bind uniform Perspective_Matrix\n";
	if (ViewMatrixID == -1) std::cerr << "Could not bind uniform View_Matrix\n";
	if (ModelMatrixID == -1) std::cerr << "Could not bind uniform Model_Matrix\n";
	if (BodyMatrixID == -1) std::cerr << "Could not bind uniform Body_Matrix\n";
	if (NeckMatrixID == -1) std::cerr << "Could not bind uniform Neck_Matrix\n";
	if (Head1MatrixID == -1) std::cerr << "Could not bind uniform Head1_Matrix\n";
	if (Head2MatrixID == -1) std::cerr << "Could not bind uniform Head2_Matrix\n";
	if (Mouth1MatrixID == -1) std::cerr << "Could not bind uniform Mouth1_Matrix\n";
	if (Mouth2MatrixID == -1) std::cerr << "Could not bind uniform Mouth2_Matrix\n";
	if (Flag1MatrixID == -1) std::cerr << "Could not bind uniform Flag1_Matrix\n";
	if (Flag2MatrixID == -1) std::cerr << "Could not bind uniform Flag2_Matrix\n";

	// dynamic uniform variable
	FigureTypeID = glGetUniformLocation(shaderProgramID, "Figure_Type");
}
void ComposeOBJColor() {
	for(auto& file : g_OBJ_Files) {
		for (auto& object : file.objects) {
			if (object.name == "Floor") {
				for (auto& vertex : object.vertices) {
					vertex.color = glm::vec3(0.4f, 0.55f, 0.4f);
				}
			}
			else if (object.name == "body.001") {
				for (auto& vertex : object.vertices) {
					vertex.color = glm::vec3(0.25f, 0.25f, 0.25f);
				}
			}
			else if (object.name == "neck.001") {
				for (auto& vertex : object.vertices) {
					vertex.color = glm::vec3(0.7f, 0.7f, 0.7f);
				}
			}
			else if (object.name == "head_1.001" || object.name == "head_2.001") {
				for (auto& vertex : object.vertices) {	
					vertex.color = glm::vec3(0.2f, 0.7f, 0.2f);
				}
			}
			else if (object.name == "mouth_1.001" || object.name == "mouth_2.001") {
				for (auto& vertex : object.vertices) {
					vertex.color = glm::vec3(0.8f, 0.8f, 0.4f);
				}
			}
			else if (object.name == "flag_1.001" || object.name == "flag_2.001") {
				for (auto& vertex : object.vertices) {
					vertex.color = glm::vec3(0.9f, 0.5f, 0.2f);
				}
			}
			else {
				// ��Ÿ ��ü�� ���� ������ ���� �Ҵ�
				glm::vec3 randomColor(urd_0_1(dre), urd_0_1(dre), urd_0_1(dre));
				for (auto& vertex : object.vertices) {
					vertex.color = randomColor;
				}
			}
		}
	}
}