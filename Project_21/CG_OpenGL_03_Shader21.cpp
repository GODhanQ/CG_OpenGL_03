#define _CRT_SECURE_NO_WARNINGS
#include "VarANDFunc_03_Shader21.h"

auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
std::default_random_engine dre(seed);
std::uniform_real_distribution<float> urd_0_1(0.0f, 1.0f);
std::uniform_int_distribution<int> uid_0_3(0, 3);

glm::mat4 Perspective_Matrix(1.0f), View_Matrix;
glm::mat4 Model_Matrix(1.0f), Box_Matrix(1.0f);

std::vector<OBJ_File> g_OBJ_Files;
std::vector<Custom_OBJ> g_OBJ_Objects;

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition(500, 50);
	glutInitWindowSize(Window_width, Window_height);
	glutCreateWindow("Example21");

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

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	std::cout << "Setup GL_POLYGON_MODE Completed\n";

	INIT_BUFFER();
	std::cout << "INIT BUFFER Completed\n";

	MakeStaticMatrix();
	std::cout << "Make Static Matrix Completed\n";

	ComposeUniformVar();
	std::cout << "Compose Uniform Variable Completed\n";

	InitializeBoxPlanes();

	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(KeyBoard);
	glutSpecialFunc(SpecialKeyBoard);
	glutMouseFunc(MouseClick);
	glutMotionFunc(MouseDrag);
	glutPassiveMotionFunc(MousePassiveMotion);
	glutIdleFunc(drawScene);

	glutMainLoop();
}

GLvoid drawScene() {
	GLfloat rColor{ 0.4f }, gColor{ 0.4f }, bColor{ 0.6f };
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
	for (const auto& object : g_OBJ_Objects) {
		if (object.name == "Box") {
			glBindVertexArray(object.VAO);
			glm::mat4 OBJ_Transform_Matrix = glm::mat4(1.0f);
			OBJ_Transform_Matrix = glm::translate(OBJ_Transform_Matrix, object.base_location);
			glUniform3fv(OBJCenterOffsetID, 1, &object.center_offset[0]);
			glUniformMatrix4fv(OBJModelTransformMatrixID, 1, GL_FALSE, &OBJ_Transform_Matrix[0][0]);
			glUniform1i(FigureTypeID, Figure_Type::BOX);

			GLuint all_indices_count = object.indices.size();
			GLuint floor_face_start = 24;   // �� 4 ���� �ε���
			GLuint floor_indices_count = 6;  // �� 4�� (�� 5�� ����)

			// �ٴڸ�(�� 4) �����ϰ� �׸��� (�� 0~3, �� 5)
			glDrawElements(GL_TRIANGLES, floor_face_start, GL_UNSIGNED_INT, 0);

			// �� 5 �׸��� (����)
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * 30));

			// �� 4(Ʈ������) ���� �׸���
			glm::mat4 Floor_Matrix = OBJ_Transform_Matrix;
			Floor_Matrix = glm::translate(Floor_Matrix, glm::vec3(0.0f, -3.0f, 3.0f));
			Floor_Matrix = glm::rotate(Floor_Matrix, glm::radians(-Floor_Open_Angle), glm::vec3(1.0f, 0.0f, 0.0f));
			Floor_Matrix = glm::translate(Floor_Matrix, glm::vec3(0.0f, 3.0f, -3.0f));

			glUniformMatrix4fv(OBJModelTransformMatrixID, 1, GL_FALSE, &Floor_Matrix[0][0]);
			glDrawElements(GL_TRIANGLES, floor_indices_count, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * floor_face_start));
		}
		else {
			glBindVertexArray(object.VAO);
			const int Figure_Type_ID =
				(object.name.substr(0, 4) == "Cube") ? Figure_Type::CUBE :
				(object.name.substr(0, 4) == "Ball") ? Figure_Type::BALL :
				Figure_Type::IDK;

			glm::mat4 OBJ_Transform_Matrix = glm::mat4(1.0f);
			OBJ_Transform_Matrix = glm::translate(OBJ_Transform_Matrix, object.base_location);
			glUniform3fv(OBJCenterOffsetID, 1, &object.center_offset[0]);
			glUniformMatrix4fv(OBJModelTransformMatrixID, 1, GL_FALSE, &OBJ_Transform_Matrix[0][0]);
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
	case 's':
		for (auto& object : g_OBJ_Objects) {
			object.movement_vector = glm::vec3(0.0f, 0.0f, 0.0f);
			object.base_location = glm::vec3(0.0f, 0.0f, 0.0f);
		}
		Open_Floor = false;
		Floor_Open_Angle = 0.0f;
		Box_Rotate_Factor = 0.0f; Box_Rotate_Sum = 0.0f;
		Model_Transform = glm::vec3(0.0f, 0.0f, 0.0f);

		break;
	case 'b':
		// delete ball object only one each time
		for (int i = 0; i < g_OBJ_Objects.size(); ++i) {
			if (g_OBJ_Objects[i].name.substr(0, 4) == "Ball") {
				g_OBJ_Objects.erase(g_OBJ_Objects.begin() + i);
				std::cout << "Deleted one Ball object. Remaining objects: " << g_OBJ_Objects.size() << "\n";
				break;
			}
		}
		break;
	case 'B': {
		// create ball object which is coply of Ball_1
		int Ball_Count = 0;
		for (const auto& object : g_OBJ_Objects) {
			if (object.name.substr(0, 4) == "Ball") {
				++Ball_Count;
			}
		}

		if (Ball_Count >= 5) {
			std::cout << "Maximum number of Ball objects reached (10). Cannot add more.\n";
			break;
		}
		std::uniform_int_distribution<int> uid_0_4(0, 4);
		std::string random_ball = std::to_string(uid_0_4(dre));
		Custom_OBJ ball_copy;
		for (const auto& file : g_OBJ_Files) {
			for (const auto& object : file.objects) {
				if (object.name == "Ball_" + random_ball) {
					ball_copy = object;
					break;
				}
			}
		}
		g_OBJ_Objects.push_back(ball_copy);
		ComposeOBJColor();
		LoadObjects();
		std::cout << "Added one Ball object. Total objects: " << g_OBJ_Objects.size() << "\n";
		break;
	}
	
	case 'z':
		Model_Transform.z += 0.5f;

		break;
	case 'Z':
		Model_Transform.z -= 0.5f;

		break;
	case 'y':
		Model_Rotation.y += 5.0f;

		break;
	case 'Y':
		Model_Rotation.y -= 5.0f;

		break;
		
	case 'k':
		Open_Floor = !Open_Floor;

		break;
	case 'q':
		exit(0);
	}
}
void SpecialKeyBoard(int key, int x, int y) {
	switch (key) {
	
	}
}
void MouseClick(int button, int state, int x, int y) {
	switch (button) {
	case GLUT_LEFT_BUTTON:
		if (state == GLUT_DOWN) {
			Box_Rotate_Factor += 0.03f;

			std::cout << "Box_Rotate_Factor: " << Box_Rotate_Factor << "\n";
		}

		break;
	case GLUT_RIGHT_BUTTON:
		if (state == GLUT_DOWN) {
			Box_Rotate_Factor -= 0.03f;
			
			std::cout << "Box_Rotate_Factor: " << Box_Rotate_Factor << "\n";
		}

		break;

	default:
		break;
	}
}
void MouseDrag(int x, int y) {
	if (x < 0.25 * Window_width) {
		if (Box_Rotate_Sum > -Max_Camera_Revolution_Angle) {
			Box_Rotate_Factor -= 0.03f;
		}
		
	}
	else if (x > 0.75 * Window_width) {
		if (Box_Rotate_Sum < Max_Camera_Revolution_Angle) {
			Box_Rotate_Factor += 0.03f;
		}
	}
	else {
		Camera_Revolution_Factor = 0.0f;
	}
}
void MousePassiveMotion(int x, int y) {
	Mouse_X = x;
	Mouse_Y = y;

	// ���ϴ� ���� �߰�
	if (x < 0.25 * Window_width) {
		if (Box_Rotate_Sum > -60.0f) {
			Box_Rotate_Factor -= 0.003f;
		}
	}
	else if (x > 0.75 * Window_width) {
		if (Box_Rotate_Sum < 60.0f) {
			Box_Rotate_Factor += 0.003f;
		}
	}
	else {
		Box_Rotate_Factor = 0.0f;
	}
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
		"Figures.obj",
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

	for (auto& file : g_OBJ_Files) {
		for (auto& object : file.objects) {
			//if (!(object.name == "Box" || object.name == "Cube_3" || object.name == "Ball_1")) continue;
			g_OBJ_Objects.push_back(object);
		}
	}

	ComposeOBJColor();

	LoadObjects();

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
					vertex.color = glm::vec3(1.0f, 1.0f, 1.0f);

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
				// �ٸ� ������ �� �����ʹ� ���� �������� ����
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

	for (auto& object : outFile.objects) {
		if (object.vertices.empty()) {
			object.collision_radius = 0.5f;
			object.center_offset = glm::vec3(0.0f);
			continue;
		}

		// �ٿ���ڽ� ���
		glm::vec3 minBounds = object.vertices[0].position;
		glm::vec3 maxBounds = object.vertices[0].position;

		for (const auto& vertex : object.vertices) {
			minBounds = glm::min(minBounds, vertex.position);
			maxBounds = glm::max(maxBounds, vertex.position);
		}

		// �ٿ���ڽ��� �߽� ��� (���� ��ǥ)
		glm::vec3 boxCenter = (minBounds + maxBounds) * 0.5f;
		object.center_offset = boxCenter;  // ���� ���� �߽� ������ ����

		// �ٿ���ڽ��� ũ��
		glm::vec3 boxSize = maxBounds - minBounds;

		// �浹 ������ = �ٿ���ڽ� �밢���� ����
		float diagonal = glm::length(boxSize);
		object.collision_radius = diagonal * 0.5f;

		std::cout << "  Object: " << object.name
			<< ", Center Offset: (" << boxCenter.x << "," << boxCenter.y << "," << boxCenter.z << ")"
			<< ", Collision Radius: " << object.collision_radius << "\n";
	}

	return true;
}
void LoadObjects() {
	for (auto& object : g_OBJ_Objects) {
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
	glBindVertexArray(0);
}

void MakeStaticMatrix() {
	Perspective_Matrix = glm::perspective(glm::radians(45.0f), (float)Window_width / (float)Window_height, 0.1f, 100.0f);
}
void MakeDynamicMatrix() {
	View_Matrix = glm::mat4(1.0f);
	View_Matrix = glm::lookAt(EYE, AT, UP);

	Model_Matrix = glm::mat4(1.0f);
	Model_Matrix = glm::translate(Model_Matrix, Model_Transform);
	Model_Matrix = glm::rotate(Model_Matrix, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	Model_Matrix = glm::rotate(Model_Matrix, glm::radians(Model_Rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));

	Box_Matrix = glm::mat4(1.0f);
	if (Box_Rotate_Sum >= -60.0f && Box_Rotate_Sum <= 60.0f) {
		Box_Rotate_Sum += Box_Rotate_Factor;
	}
	else if (Box_Rotate_Sum > 60.0f) {
		Box_Rotate_Sum = 59.9f;
		Box_Rotate_Factor = 0.0f;
	}
	else if (Box_Rotate_Sum < -60.0f) {
		Box_Rotate_Sum = -59.9f;
		Box_Rotate_Factor = 0.0f;
	}
	Box_Matrix = glm::rotate(Box_Matrix, glm::radians(Box_Rotate_Sum), glm::vec3(1.0f, 0.0f, 0.0f));

	if (Open_Floor) {
		if (Floor_Open_Angle < 90.0f) {
			Floor_Open_Angle += Floor_Open_Angle_Factor;
		}
	}
	else {
		if (Floor_Open_Angle > 0.0f) {
			Floor_Open_Angle -= Floor_Open_Angle_Factor;
		}
		else {
			Floor_Open_Angle = 0.0f;
		}
	}

	UpdateBoxPlanes(Box_Matrix);

	const float friction = 0.999f;  // ���� ���

	for (auto& object : g_OBJ_Objects) {
		if (object.name == "Box") continue;

		object.movement_vector.y -= Gravity_Factor;
		object.base_location += object.movement_vector;

		glm::vec3 collision_center = object.base_location + object.center_offset;

		bool isBall = (object.name.substr(0, 4) == "Ball");
		const float restitution = 0.8f;

		for (auto& plane : Box_Planes) {
			float distance = plane.getSignedDistance(collision_center);

			if (distance < object.collision_radius) {
				object.base_location += plane.normal * (object.collision_radius - distance);

				float velocityDotNormal = glm::dot(object.movement_vector, plane.normal);
				if (velocityDotNormal < 0.0f && isBall) {
					object.movement_vector -= plane.normal * velocityDotNormal * (1.0f + restitution);
				}

				// Cube�� �浹 �� ���� ����
				if (!isBall) {
					object.movement_vector *= friction;
				}
			}
		}

		// Cube�� �׻� ���� ����
		if (!isBall) {
			object.movement_vector *= friction;
		}
	}
}

void ComposeUniformVar() {
	// static uniform variable
	PerspectiveMatrixID = glGetUniformLocation(shaderProgramID, "Perspective_Matrix");
	ViewMatrixID = glGetUniformLocation(shaderProgramID, "View_Matrix");
	ModelMatrixID = glGetUniformLocation(shaderProgramID, "Model_Matrix");
	BoxMatrixID = glGetUniformLocation(shaderProgramID, "Box_Matrix");
	
	glUniformMatrix4fv(PerspectiveMatrixID, 1, GL_FALSE, &Perspective_Matrix[0][0]);
	glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &View_Matrix[0][0]);
	glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &Model_Matrix[0][0]);
	glUniformMatrix4fv(BoxMatrixID, 1, GL_FALSE, &Box_Matrix[0][0]);
	
	if (PerspectiveMatrixID == -1) std::cerr << "Could not bind uniform Perspective_Matrix\n";
	if (ViewMatrixID == -1) std::cerr << "Could not bind uniform View_Matrix\n";
	if (ModelMatrixID == -1) std::cerr << "Could not bind uniform Model_Matrix\n";
	if (BoxMatrixID == -1) std::cerr << "Could not bind uniform Box_Matrix\n";

	// dynamic uniform variable
	FigureTypeID = glGetUniformLocation(shaderProgramID, "Figure_Type");
	OBJModelTransformMatrixID = glGetUniformLocation(shaderProgramID, "OBJ_Model_Transform_Matrix");
	OBJCenterOffsetID = glGetUniformLocation(shaderProgramID, "OBJ_Center_Offset");
}
void ComposeOBJColor() {
	for (auto& object : g_OBJ_Objects) {
		if (object.name == "Box") {
			glm::vec3 faceColors[6] = {
				glm::vec3(0.6f, 0.6f, 0.6f),
				glm::vec3(0.2f, 0.2f, 0.2f),
				glm::vec3(0.6f, 0.6f, 0.6f),
				glm::vec3(0.2f, 0.2f, 0.2f),
				glm::vec3(0.4f, 0.4f, 0.4f),
				glm::vec3(0.4f, 0.4f, 0.4f)
			};

			int trianglePerFace = 6;
			for (size_t i = 0; i < object.indices.size(); ++i) {
				unsigned int faceIndex = i / trianglePerFace;
				faceIndex = faceIndex % 6;
				object.vertices[object.indices[i]].color = faceColors[faceIndex];
			}
		}
		else if (object.name.substr(0, 4) == "Cube") {
			std::uniform_real_distribution<float> urd_color(0.8f, 1.5f);
			float randomFactor = urd_color(dre);
			glm::vec3 cubeColor(0.8f, 0.3f, 0.3f);
			for (auto& vertex : object.vertices) {
				vertex.color = cubeColor * glm::vec3(randomFactor, randomFactor, randomFactor);
			}
		}
		else if (object.name.substr(0, 4) == "Ball") {
			std::uniform_real_distribution<float> urd_color(0.8f, 1.5f);
			float randomFactor = urd_color(dre);
			glm::vec3 cubeColor(0.3f, 0.3f, 0.8f);
			for (auto& vertex : object.vertices) {
				vertex.color = cubeColor * glm::vec3(randomFactor, randomFactor, randomFactor);
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
void InitializeBoxPlanes() {
	Box_Planes.clear();
}

void UpdateBoxPlanes(const glm::mat4& rotationMatrix) {
	// �ʱ� ������ �ٽ� ����
	std::vector<Plane> initialPlanes;
	float halfSize = 3.5f;
	initialPlanes.push_back(Plane(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, -halfSize, 0.0f)));    // �ٴ� (�� 0)
	initialPlanes.push_back(Plane(glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, halfSize, 0.0f)));    // õ�� (�� 1)
	initialPlanes.push_back(Plane(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(-halfSize, 0.0f, 0.0f)));    // ���� (�� 2)
	initialPlanes.push_back(Plane(glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(halfSize, 0.0f, 0.0f)));    // ������ (�� 3)
	initialPlanes.push_back(Plane(glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, -halfSize)));    // �� (�� 4 - Ʈ������)
	initialPlanes.push_back(Plane(glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 0.0f, halfSize)));    // �� (�� 5)

	// ȸ�� ��� ����
	Box_Planes.clear();
	for (int i = 0; i < initialPlanes.size(); ++i) {
		const auto& plane = initialPlanes[i];
		Plane rotatedPlane;
		rotatedPlane.normal = glm::normalize(glm::vec3(rotationMatrix * glm::vec4(plane.normal, 0.0f)));
		rotatedPlane.point = glm::vec3(rotationMatrix * glm::vec4(plane.point, 1.0f));

		// �� 4(�ε��� 4)�� Ʈ�������̹Ƿ� �߰� ȸ�� ����
		if (i == 0) {
			// Ʈ������ ȸ��: y�� �߽����� -Floor_Open_Angle��ŭ ȸ��
			glm::mat4 trapdoorRotation = glm::rotate(glm::mat4(1.0f), glm::radians(-Floor_Open_Angle), glm::vec3(1.0f, 0.0f, 0.0f));
			rotatedPlane.normal = glm::normalize(glm::vec3(trapdoorRotation * glm::vec4(rotatedPlane.normal, 0.0f)));
			rotatedPlane.point = glm::vec3(trapdoorRotation * glm::vec4(rotatedPlane.point, 1.0f));
		}

		Box_Planes.push_back(rotatedPlane);
	}
}