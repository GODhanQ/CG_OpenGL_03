#define _CRT_SECURE_NO_WARNINGS
#include "VarANDFunc_03_Shader18.h"

auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
std::default_random_engine dre(seed);
std::uniform_real_distribution<float> urd_0_1(0.0f, 1.0f);
std::uniform_int_distribution<int> uid_0_3(0, 3);

glm::mat4 Perspective_Matrix(1.0f), TranslationCube_Matrix(1.0f), TranslationPyramid_Matrix(1.0f), TranslationSphere_Matrix(1.0f);
glm::mat4 Rotation_Matrix(1.0f), Revolution_Matrix(1.0f), Scaling_Matrix(1.0f), Translation_Matrix(1.0f);
glm::mat4 ScalingOriginF_Matrix(1.0f), ScalingOriginS_Matrix(1.0f);
glm::mat4 StrangeRevolutionF_Matrix(1.0f), StrangeRevolutionS_Matrix(1.0f);

glm::vec3 Sphere_Color;

PlaneManager plane_manager;
std::vector<Vertex_glm> all_vertices;

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition(500, 50);
	glutInitWindowSize(Window_width, Window_height);
	glutCreateWindow("Example18");

	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		return 1;
	}
	std::cout << "glew initialized\n";

	qobj_s = gluNewQuadric();
	qobj_c = gluNewQuadric();
	gluQuadricDrawStyle(qobj_s, GLU_FILL);
	gluQuadricDrawStyle(qobj_c, GLU_FILL);


	shaderProgramID = make_shaderProgram("Vertex_Shader.glsl", "Fragment_Shader.glsl");
	std::cout << "Make Shader Program Completed\n";

	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	std::cout << "Setup GL_CULL_FACE Completed\n";

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

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
	GLfloat rColor{ 0.2f }, gColor{ 0.2f }, bColor{ 0.2f };
	glClearColor(rColor, gColor, bColor, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	MakeDynamicMatrix();
	plane_manager.PrepareBuffer();

	glUseProgram(shaderProgramID);

	ComposeUniformVar();

	// Draw Axis
	glBindVertexArray(VAO_axis);
	glUniform1d(isGLUID, false);
	glUniform1i(FigureTypeID, Figure_Type::AXIS);
	glLineWidth(1.0f);
	glDrawElements(GL_LINES, Axis_Index.size(), GL_UNSIGNED_INT, 0);

	// Draw Planes
	glBindVertexArray(VAO);
	size_t offset = 0;
	for (size_t i = 0; i < plane_manager.planes.size(); ++i) {
		if (plane_manager.planes[i].isSet) {
			int transed_Figure{};
			if (plane_manager.planes[i].Figure_Type == Display_Figure[0] ||
				plane_manager.planes[i].Figure_Type == Display_Figure[1]) {
				if (plane_manager.planes[i].Figure_Type == Display_Figure[0]) transed_Figure = 0;
				else if (plane_manager.planes[i].Figure_Type == Display_Figure[1]) transed_Figure = 1;
			}

			glUniform1i(isGLUID, false);
			glUniform1i(FigureTypeID, transed_Figure);

			if (plane_manager.planes[i].indices.size() == 6)
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * offset));
			else if (plane_manager.planes[i].indices.size() == 3)
				glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * offset));
			else
				std::cout << "Error: plane indices size is not correct.\n";
		}
		offset += plane_manager.planes[i].indices.size();
	}

	// Draw GLU Objects
	if (Figure_Type::SPHERE == Display_Figure[0] ||
		Figure_Type::SPHERE == Display_Figure[1]) {
		int transed_Figure;
		if (Figure_Type::SPHERE == Display_Figure[0]) transed_Figure = 0;
		else if (Figure_Type::SPHERE == Display_Figure[1]) transed_Figure = 1;

		Sphere_Color = glm::vec3(urd_0_1(dre), urd_0_1(dre), urd_0_1(dre));
		glUniform1i(isGLUID, true);
		glUniform1i(FigureTypeID, transed_Figure);
		gluSphere(qobj_s, 0.5, 10, 10);
	}

	// Draw Cylinder
	if (Figure_Type::CYLINDER == Display_Figure[0] ||
		Figure_Type::CYLINDER == Display_Figure[1]) {
		int transed_Figure;
		if (Figure_Type::CYLINDER == Display_Figure[0]) transed_Figure = 0;
		else if (Figure_Type::CYLINDER == Display_Figure[1]) transed_Figure = 1;

		Sphere_Color = glm::vec3(urd_0_1(dre), urd_0_1(dre), urd_0_1(dre));
		glUniform1i(isGLUID, true);
		glUniform1i(FigureTypeID, transed_Figure);
		gluCylinder(qobj_c, 0.3, 0.3, 1.0, 20, 10);
	}

	glBindVertexArray(0);

	glutSwapBuffers();
}
GLvoid Reshape(int w, int h) {
	glViewport(0, 0, w, h);
}

void KeyBoard(unsigned char key, int x, int y) {
	switch (key) {
	case '1':
		Applicate_to = 0;
		std::cout << "Applicate_to : First Figure\n";
		break;
	case '2':
		Applicate_to = 1;
		std::cout << "Applicate_to : Second Figure\n";
		break;
	case '3':
		Applicate_to = 2;
		std::cout << "Applicate_to : All Figures\n";
		break;


	case 'x':
		Rotation_Mode = true;
		if (Rotate_Amount.x < 0) Rotate_Amount.x = 0.0f;
		Rotate_Amount.x += 0.05f;

		std::cout << "+X Rotation Angle: " << Rotate_Amount.x << std::endl;
		glutPostRedisplay();
		break;
	case 'X':
		Rotation_Mode = true;
		if (Rotate_Amount.x > 0) Rotate_Amount.x = 0.0f;
		Rotate_Amount.x -= 0.05f;

		std::cout << "-X Rotation Angle: " << Rotate_Amount.x << std::endl;
		glutPostRedisplay();
		break;
	case 'y':
		Rotation_Mode = true;
		if (Rotate_Amount.y < 0) Rotate_Amount.y = 0.0f;
		Rotate_Amount.y += 0.05f;

		std::cout << "+Y Rotation Angle: " << Rotate_Amount.y << std::endl;
		glutPostRedisplay();
		break;
	case 'Y':
		Rotation_Mode = true;
		if (Rotate_Amount.y > 0) Rotate_Amount.y = 0.0f;
		Rotate_Amount.y -= 0.05f;

		std::cout << "-Y Rotation Angle: " << Rotate_Amount.y << std::endl;
		glutPostRedisplay();
		break;
	case 'r':
		Revolution_Mode = true;
		if (Revolution_Amount.y < 0) Revolution_Amount.y = 0.0f;
		Revolution_Amount.y += 0.05f;

		std::cout << "+Y Revolution Angle: " << Revolution_Amount.y << std::endl;
		glutPostRedisplay();
		break;
	case 'R':
		Revolution_Mode = true;
		if (Revolution_Amount.y > 0) Revolution_Amount.y = 0.0f;
		Revolution_Amount.y -= 0.05f;

		std::cout << "-Y Revolution Angle: " << Revolution_Amount.y << std::endl;
		glutPostRedisplay();
		break;
	case 'a':
		Scaling_Mode = true;
		ScaleOrigin_Factor += ScaleOrigin_Amount;

		std::cout << "Scaling Origin Up, Scale Origin Factor: " << glm::length(ScaleOrigin_Factor) << std::endl;
		glutPostRedisplay();
		break;
	case 'A':
		Scaling_Mode = true;
		ScaleOrigin_Factor -= ScaleOrigin_Amount;

		std::cout << "Scaling Origin Down, Scale Origin Factor: " << glm::length(ScaleOrigin_Factor) << std::endl;
		glutPostRedisplay();
		break;
	case 'b':
		Scaling_Mode = true;
		Scale_Factor += Scale_Amount;

		std::cout << "Scaling Up, Scale Factor: " << glm::length(Scale_Factor) << std::endl;
		glutPostRedisplay();
		break;
	case 'B':
		Scaling_Mode = true;
		Scale_Factor -= Scale_Amount;

		std::cout << "Scaling Down, Scale Factor: " << glm::length(Scale_Factor) << std::endl;
		glutPostRedisplay();
		break;
	case 'd':
		Translate_Factor.x += Translate_Amount.x;
		
		std::cout << "Translate +X, Translate Factor: " << glm::length(Translate_Factor) << std::endl;
		glutPostRedisplay();
		break;
	case 'D':
		Translate_Factor.x -= Translate_Amount.x;
		
		std::cout << "Translate -X, Translate Factor: " << glm::length(Translate_Factor) << std::endl;
		glutPostRedisplay();
		break;
	case 'e':
		Translate_Factor.y += Translate_Amount.y;

		std::cout << "Translate +Y, Translate Factor: " << glm::length(Translate_Factor) << std::endl;
		glutPostRedisplay();
		break;
	case 'E':
		Translate_Factor.y -= Translate_Amount.y;

		std::cout << "Translate -Y, Translate Factor: " << glm::length(Translate_Factor) << std::endl;
		glutPostRedisplay();
		break;

	case 'c':
		Display_Figure[0] = uid_0_3(dre);
		Display_Figure[1] = uid_0_3(dre);
		while (Display_Figure[1] == Display_Figure[0])
			Display_Figure[1] = uid_0_3(dre);
		ResettingFigures();

		std::cout << "Randomly Change Display_Figure : " << Display_Figure[0] << ", " << Display_Figure[1] << std::endl;
		glutPostRedisplay();
		break;


	case 't':
		Applicate_to = 2;
		ConvertLoc_Mode = !ConvertLoc_Mode;
		SquareMoving_Mode = false;

		std::cout << "Toggle Convert Location Mode : " << (ConvertLoc_Mode ? "ON" : "OFF") << std::endl;
		glutPostRedisplay();
		break;
	case 'k':
		Applicate_to = 2;
		SquareMoving_Mode = !SquareMoving_Mode;
		ConvertLoc_Mode = false;

		std::cout << "Toggle Square Moving Mode : " << (SquareMoving_Mode ? "ON" : "OFF") << std::endl;
		glutPostRedisplay();
		break;
	case 'v':
		Applicate_to = 2;
		StrangeRevolution_Mode = !StrangeRevolution_Mode;
		if (StrangeRevolution_Mode) {
			Revolution_Angles = glm::vec3(0.0f, 0.0f, 0.0f);
			Revolution_Amount.y = 0.03f;
			Revolution_Mode = true;
		}
		else {
			Revolution_Angles = glm::vec3(0.0f, 0.0f, 0.0f);
			Revolution_Amount = glm::vec3(0.0f, 0.0f, 0.0f);
			Revolution_Mode = false;
		}

		std::cout << "Toggle Strange Revolution Mode : " << (StrangeRevolution_Mode ? "ON" : "OFF") << std::endl;
		break;

	case 'u':
		if (glIsEnabled(GL_CULL_FACE)) glDisable(GL_CULL_FACE);
		else glEnable(GL_CULL_FACE);

		std::cout << "Toggle GL_CULL_FACE\n";
		glutPostRedisplay();
		break;
	case 's':
		Rotation_Mode = false; Revolution_Mode = false; Scaling_Mode = false; StrangeRevolution_Mode = false;
		ConvertLoc_Mode = false; SquareMoving_Mode = false;
		Rotation_Angles = glm::vec3(0.0f, 0.0f, 0.0f); Revolution_Angles = glm::vec3(0.0f, 0.0f, 0.0f);
		Rotate_Amount = glm::vec3(0.0f, 0.0f, 0.0f); Revolution_Amount = glm::vec3(0.0f, 0.0f, 0.0f);
		Scale_Factor = glm::vec3(1.0f, 1.0f, 1.0f); Translate_Factor = glm::vec3(0.0f, 0.0f, 0.0f);
		ScaleOrigin_Factor = glm::vec3(1.0f, 1.0f, 1.0f);

		INIT_BUFFER();
		glutPostRedisplay();
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
	// showing view is rotated x axis by 30 degree, y axis by 30 degree and z axis by 0 degree
	Perspective_Matrix = glm::mat4(1.0f);
	Perspective_Matrix = glm::scale(Perspective_Matrix, glm::vec3(0.5f, 0.5f, 0.5f));
	Perspective_Matrix = glm::rotate(Perspective_Matrix, glm::radians(-30.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	Perspective_Matrix = glm::rotate(Perspective_Matrix, glm::radians(120.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	Perspective_Matrix = glm::rotate(Perspective_Matrix, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));

	TranslationCube_Matrix = glm::mat4(1.0f);
	TranslationCube_Matrix = glm::translate(TranslationCube_Matrix, glm::vec3(0.0f, 0.0f, -Base_Range));

	TranslationPyramid_Matrix = glm::mat4(1.0f);
	TranslationPyramid_Matrix = glm::translate(TranslationPyramid_Matrix, glm::vec3(0.0f, 0.0f, Base_Range));
}
void MakeDynamicMatrix() {
	// Rotation
	Rotation_Angles += Rotate_Amount;
	if (Rotation_Angles.x >= 360.0f) Rotation_Angles.x -= 360.0f;
	if (Rotation_Angles.y >= 360.0f) Rotation_Angles.y -= 360.0f;
	if (Rotation_Angles.z >= 360.0f) Rotation_Angles.z -= 360.0f;

	if (!Rotation_Mode) Rotation_Matrix = glm::mat4(1.0f);
	else {
		Rotation_Matrix = glm::mat4(1.0f);
		Rotation_Matrix = glm::rotate(Rotation_Matrix, glm::radians(Rotation_Angles.x), glm::vec3(1.0f, 0.0f, 0.0f));
		Rotation_Matrix = glm::rotate(Rotation_Matrix, glm::radians(Rotation_Angles.y), glm::vec3(0.0f, 1.0f, 0.0f));
	}

	// Revolution
	Revolution_Angles += Revolution_Amount;
	if (Revolution_Angles.x >= 360.0f) Revolution_Angles.x -= 360.0f;
	if (Revolution_Angles.y >= 360.0f) Revolution_Angles.y -= 360.0f;
	if (Revolution_Angles.z >= 360.0f) Revolution_Angles.z -= 360.0f;

	if (!Revolution_Mode) Revolution_Matrix = glm::mat4(1.0f);
	else {
		Revolution_Matrix = glm::mat4(1.0f);
		Revolution_Matrix = glm::rotate(Revolution_Matrix, glm::radians(Revolution_Angles.y), glm::vec3(0.0f, 1.0f, 0.0f));
	}

	// Scaling
	if (!Scaling_Mode) Scaling_Matrix = glm::mat4(1.0f);
	else {
		Scaling_Matrix = glm::mat4(1.0f);
		Scaling_Matrix = glm::scale(Scaling_Matrix, Scale_Factor);
	}

	// Scaling around origin
	if (!Scaling_Mode) {
		ScalingOriginF_Matrix = glm::mat4(1.0f);
		ScalingOriginS_Matrix = glm::mat4(1.0f);
	}
	else {
		ScalingOriginF_Matrix = glm::mat4(1.0f);
		ScalingOriginF_Matrix = glm::scale(ScalingOriginF_Matrix, ScaleOrigin_Factor);

		ScalingOriginS_Matrix = glm::mat4(1.0f);
		ScalingOriginS_Matrix = glm::scale(ScalingOriginS_Matrix, ScaleOrigin_Factor);
	}

	// Translating
	Translation_Matrix = glm::mat4(1.0f);
	Translation_Matrix = glm::translate(Translation_Matrix, Translate_Factor);

	// Strange Revolution
	if (StrangeRevolution_Mode) {
		float elapsedTime = glutGet(GLUT_ELAPSED_TIME);
		float normalizedSin = (sin(elapsedTime * StrangeRevolutionProgressAmount) + 1.0f) / 2.0f;
		float scaleF = 1.0f + normalizedSin * (StrangeRevolutionMaxScale - 1.0f);
		float scaleS = 1.0f + (1.0f - normalizedSin) * (StrangeRevolutionMaxScale - 1.0f);


		StrangeRevolutionF_Matrix = glm::mat4(1.0f); StrangeRevolutionS_Matrix = glm::mat4(1.0f);
		StrangeRevolutionF_Matrix = glm::scale(StrangeRevolutionF_Matrix, glm::vec3(scaleF, scaleF, scaleF));
		StrangeRevolutionS_Matrix = glm::scale(StrangeRevolutionS_Matrix, glm::vec3(scaleS, scaleS, scaleS));
	}
	else {
		StrangeRevolutionF_Matrix = glm::mat4(1.0f);
		StrangeRevolutionS_Matrix = glm::mat4(1.0f);
	}

	// Convert each location
	if (ConvertLoc_Mode) {
		glm::vec3 FirstFigure_Loc = glm::vec3(0.0f, 0.0f, -Base_Range);
		glm::vec3 SecondFigure_Loc = glm::vec3(0.0f, 0.0f, Base_Range);
		glm::vec3 Move_Vector = SecondFigure_Loc - FirstFigure_Loc;
		float elapsedTime = glutGet(GLUT_ELAPSED_TIME);
		float normalizedSin = (sin(elapsedTime * ConvertLocProgressAmount) + 1.0f) / 2.0f;
		glm::vec3 Current_Offset = Move_Vector * normalizedSin;

		TranslationCube_Matrix = glm::mat4(1.0f);
		TranslationCube_Matrix = glm::translate(TranslationCube_Matrix, FirstFigure_Loc + Current_Offset);
		TranslationPyramid_Matrix = glm::mat4(1.0f);
		TranslationPyramid_Matrix = glm::translate(TranslationPyramid_Matrix, SecondFigure_Loc - Current_Offset);
	}

	// Square Moving
	// Move in Rectangle path
	// Moving Point : (0, 0, Base_Range) -> (0, -Base_Range, 0) -> (0, 0, -Base_Range) -> (0, Base_Range, 0) -> (0, 0, Base_Range) ...
	else if (SquareMoving_Mode) {
		SquareMovingProgress += SquareMovingProgressAmount;

		if (SquareMovingProgress >= 1.0f) {
			SquareMovingProgress = 0.0f;
			F_phase = (F_phase + 1) % 4;
			S_phase = (S_phase + 1) % 4;
		}

		glm::vec3 F_startPos = SquareMoving_TargetLoc[(F_phase + 3) % 4];
		glm::vec3 F_endPos = SquareMoving_TargetLoc[F_phase];
		glm::vec3 F_currentPos = glm::mix(F_startPos, F_endPos, SquareMovingProgress);
		TranslationCube_Matrix = glm::translate(glm::mat4(1.0f), F_currentPos);

		glm::vec3 S_startPos = SquareMoving_TargetLoc[(S_phase + 3) % 4];
		glm::vec3 S_endPos = SquareMoving_TargetLoc[S_phase];
		glm::vec3 S_currentPos = glm::mix(S_startPos, S_endPos, SquareMovingProgress);
		TranslationPyramid_Matrix = glm::translate(glm::mat4(1.0f), S_currentPos);
	}
	else {
		TranslationCube_Matrix = glm::mat4(1.0f);
		TranslationCube_Matrix = glm::translate(TranslationCube_Matrix, glm::vec3(0.0f, 0.0f, -Base_Range));
		TranslationPyramid_Matrix = glm::mat4(1.0f);
		TranslationPyramid_Matrix = glm::translate(TranslationPyramid_Matrix, glm::vec3(0.0f, 0.0f, Base_Range));
	}
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

void ComposeUniformVar() {
	FigureTypeID = glGetUniformLocation(shaderProgramID, "Figure_Type");
	PerspectiveMatrixID = glGetUniformLocation(shaderProgramID, "Perspective_Matrix");
	TranslationCubeMatrixID = glGetUniformLocation(shaderProgramID, "TranslationCube_Matrix");
	TranslationPyramidMatrixID = glGetUniformLocation(shaderProgramID, "TranslationPyramid_Matrix");
	SphereColorID = glGetUniformLocation(shaderProgramID, "Sphere_Color");
	RotationMatrixID = glGetUniformLocation(shaderProgramID, "Rotation_Matrix");
	RevolutionMatrixID = glGetUniformLocation(shaderProgramID, "Revolution_Matrix");
	ScalingMatrixID = glGetUniformLocation(shaderProgramID, "Scaling_Matrix");
	TranslationMatrixID = glGetUniformLocation(shaderProgramID, "Translation_Matrix");
	isGLUID = glGetUniformLocation(shaderProgramID, "is_GLU");
	ScalingOriginFMatrixID = glGetUniformLocation(shaderProgramID, "ScalingOriginF_Matrix");
	ScalingOriginSMatrixID = glGetUniformLocation(shaderProgramID, "ScalingOriginS_Matrix");
	ApplicateID = glGetUniformLocation(shaderProgramID, "Applicate_to");
	StrangeRevolutionFMatrixID = glGetUniformLocation(shaderProgramID, "StrangeRevolutionF_Matrix");
	StrangeRevolutionSMatrixID = glGetUniformLocation(shaderProgramID, "StrangeRevolutionS_Matrix");

	glUniformMatrix4fv(PerspectiveMatrixID, 1, GL_FALSE, &Perspective_Matrix[0][0]);
	glUniformMatrix4fv(TranslationCubeMatrixID, 1, GL_FALSE, &TranslationCube_Matrix[0][0]);
	glUniformMatrix4fv(TranslationPyramidMatrixID, 1, GL_FALSE, &TranslationPyramid_Matrix[0][0]);
	glUniform3f(SphereColorID, Sphere_Color.x, Sphere_Color.y, Sphere_Color.z);
	glUniformMatrix4fv(RotationMatrixID, 1, GL_FALSE, &Rotation_Matrix[0][0]);
	glUniformMatrix4fv(RevolutionMatrixID, 1, GL_FALSE, &Revolution_Matrix[0][0]);
	glUniformMatrix4fv(ScalingMatrixID, 1, GL_FALSE, &Scaling_Matrix[0][0]);
	glUniformMatrix4fv(TranslationMatrixID, 1, GL_FALSE, &Translation_Matrix[0][0]);
	glUniformMatrix4fv(ScalingOriginFMatrixID, 1, GL_FALSE, &ScalingOriginF_Matrix[0][0]);
	glUniformMatrix4fv(ScalingOriginSMatrixID, 1, GL_FALSE, &ScalingOriginS_Matrix[0][0]);
	glUniform1i(ApplicateID, Applicate_to);
	glUniformMatrix4fv(StrangeRevolutionFMatrixID, 1, GL_FALSE, &StrangeRevolutionF_Matrix[0][0]);
	glUniformMatrix4fv(StrangeRevolutionSMatrixID, 1, GL_FALSE, &StrangeRevolutionS_Matrix[0][0]);

	if (FigureTypeID == -1) std::cout << "FigureTypeID is not valid.\n";
	if (PerspectiveMatrixID == -1) std::cout << "PerspectiveMatrixID is not valid.\n";
	if (TranslationCubeMatrixID == -1) std::cout << "TranslationCubeMatrixID is not valid.\n";
	if (TranslationPyramidMatrixID == -1) std::cout << "TranslationPyramidMatrixID is not valid.\n";
	if (SphereColorID == -1) std::cout << "SphereColorID is not valid.\n";
	if (RotationMatrixID == -1) std::cout << "RotationMatrixID is not valid.\n";
	if (RevolutionMatrixID == -1) std::cout << "RevolutionMatrixID is not valid.\n";
	if (ScalingMatrixID == -1) std::cout << "ScalingMatrixID is not valid.\n";
	if (TranslationMatrixID == -1) std::cout << "TranslationMatrixID is not valid.\n";
	if (isGLUID == -1) std::cout << "isGLUID is not valid.\n";
	if (ScalingOriginFMatrixID == -1) std::cout << "ScalingOriginMatrixID is not valid.\n";
	if (ScalingOriginSMatrixID == -1) std::cout << "ScalingOriginSMatrixID is not valid.\n";
	if (ApplicateID == -1) std::cout << "ApplicateID is not valid.\n";
	if (StrangeRevolutionFMatrixID == -1) std::cout << "StrangeRevolutionFMatrixID is not valid.\n";
	if (StrangeRevolutionSMatrixID == -1) std::cout << "StrangeRevolutionSMatrixID is not valid.\n";
}
void ResettingFigures() {
	for(auto& plane : plane_manager.planes) {
		if (plane.Figure_Type == Display_Figure[0] ||
			plane.Figure_Type == Display_Figure[1]) {
			plane.Enable();
		}
		else {
			plane.Disable();
		}
	}
}