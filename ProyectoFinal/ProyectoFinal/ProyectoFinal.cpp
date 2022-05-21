#include <iostream>
#include <cmath>

// GLEW
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// Other Libs
#include "stb_image.h"

// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//Load Models
#include "SOIL2/SOIL2.h"

// Other includes
#include "Shader.h"
#include "Camera.h"
#include "Model.h"
#include "Texture.h"

// Function prototypes
void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mode);
void MouseCallback(GLFWwindow *window, double xPos, double yPos);
void DoMovement();
void recorrido();
void lanzamiento();

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;
int SCREEN_WIDTH, SCREEN_HEIGHT;

// Camera
Camera  camera(glm::vec3(-35.0f, 0.0f, 40.0f));
GLfloat lastX = WIDTH / 2.0;
GLfloat lastY = HEIGHT / 2.0;
bool keys[1024];
bool firstMouse = true;

// Light attributes
glm::vec3 Light1 = glm::vec3(0);  //Color
glm::vec3 pointLightPosIni(0.4f, 1.1f, -2.3f);  //Initial position - pointlight
glm::vec3 pointLightPos = pointLightPosIni;  //Actual position - pointlight

//Light off
bool lamparaOn = false;

//Cannonball movement
float movBalaX = 0.0f; //Posición x inicial de la bala
float movBalaY = 0.0f; //Posición y inicial de la bala
float movBalaZ = 0.0f; //Posición z inicial de la bala
bool lanzar = false;  //Desactivar lanzamiento
float rotCanion = 0.0f;  //ángulo inicial del cañón
double vi = 20.0f;  //velocidad inicial
#define g 9.81  //Constante gravedad

//Drawer movement
float movCajonZ = -0.611f;
bool cajonAbierto = false;

//Refrigerator door movement
float rotPuertaRefri = 0.0f;
bool refriAbierto = false;

// Keyframes
glm::vec3 PosIni(0.0f, 0.0f, 0.0f);  //Initial position elemnts
float posX = PosIni.x, posZ = PosIni.z, rotMerry = 0;  
#define MAX_FRAMES 60 //Max number of frames
int i_max_steps = 250;  //Max number of interpolated values
int i_curr_steps = 0;  //Current interpolation

typedef struct w
{
	//Variables to store keyframes values
	float posX;		//X Position
	float posZ;		//Z position
	float incX;		//x increment
	float incZ;		//z increment
	float rotInc;   //Interpolated values
	float rotMerry; //Rotation ship and elements
}FRAME;

FRAME KeyFrame[MAX_FRAMES];

int FrameIndex = 0;  //Index to access frame
bool play = false;  //Keyframes animation off
int playIndex = 0;  //

//Pre defined frame values
GLfloat valoresPosX[]     = { 0.0f, 0.15f, 0.3f,   0.3f,  0.15f, 0.0f,    0.0f,  0.15f, 0.3f,    0.3f,  0.15f, 0.0f,  //   
							  0.0f,  0.0f,  0.0f,  //x = 0 while ship turns to the left
							  0.0f, 10.0f, 20.0f,  30.0f, 40.0f, 50.0f,   60.0f, 70.0f, 80.0f,   90.0f, 100.0f, 110.0f,  //Leftward movement 
							  110.0f, 110.0f, 110.0f,  //x = 110 while ship turns to the left
							  110.0f, 109.85, 109.7, 109.7, 109.85, 110.0f,  110.0f, 109.85, 109.7, 109.7, 109.85, 110.0f,
							  110.0f, 110.0f, 110.0f,  //x = 110 while ship turns to the left
	                          110.0f, 100.0f, 90.0f, 80.0f, 70.0f, 60.0f, 50.0f, 40.0f, 30.0f, 20.0f, 10.0f, 0.0f,  //Rightward movement
							  0.0f,  0.0f,  0.0f,  //x = 0 while ship turns to the left
                            };

GLfloat valoresPosZ[]     = { 0.0f, 10.0f, 20.0f, 30.0f, 40.0f, 50.0f, 60.0f, 70.0f, 80.0f, 90.0f, 100.0f, 110.0f,  //Backward movement
							  110.0f, 110.0f, 110.0f,  //z = 110 while ship turns to the left
	                          110.0f, 109.85, 109.7, 109.7, 109.85, 110.0f,  110.0f, 109.85, 109.7,109.7, 109.85, 110.0f, 
							  110.0f, 110.0f, 110.0f,  //z = 110 while ship turns to the left
	                          110.0f, 100.0f, 90.0f, 80.0f, 70.0f, 60.0f, 50.0f, 40.0f, 30.0f, 20.0f, 10.0f, 0.0f,  //Forward movement
							  0.0f, 0.0f,  0.0f,  //z = 0 while ship turns to the left
							  0.0f, 0.15f, 0.3f,   0.3f,  0.15f, 0.0f,    0.0f,  0.15f, 0.3f, 0.3f,  0.15f, 0.0f,
							  0.0f, 0.0f,  0.0f //z = 0 while ship turns to the left
                            };

GLfloat valoresRotMerry[] = { 4.0f, 4.0f,  4.0f,  -8.0f, -8.0f, -8.0f,  4.0f,  4.0f,  4.0f,  -8.0f, -8.0f,  -8.0f, //slight rotation while moving
                              30.0f, 60.0f, 90.0f,   //turn to the left (90°)
							  94.0f, 94.0f,  94.0f, 86.0f, 86.0f, 86.0f,  94.0f,  94.0f,  94.0f,  86.0f, 86.0f,  90.0f, //slight rotation while moving
                              120.0f, 150.0f, 180.0f,  //turn to the left (180°)
	                          184.0f, 184.0f, 184.0f,  176.0f, 176.0f, 176.0f,  184.0f, 184.0f, 184.0f,  176.0f, 176.0f, 180.0f, //slight rotation while moving
							  210.0f, 240.0f, 270.0f,  //turn to the left (270°)
	                          274.0f, 274.0f, 274.0f,  266.0f, 266.0f, 266.0f, 274.0f, 274.0f, 274.0f,  276.0f, 276.0f, 300.0f, //slight rotation while moving
							  300.0f, 330.0f, 360.0f,  //turn to the left (360°)
							 };

//Load pre defined values function to the frames 
void asignarValores(void)
{
	int i = 0;
	while (FrameIndex < MAX_FRAMES) {
		KeyFrame[FrameIndex].posX = valoresPosX[i];
		KeyFrame[FrameIndex].posZ = valoresPosZ[i];
		KeyFrame[FrameIndex].rotMerry = valoresRotMerry[i];
		i++;
		FrameIndex++;
	}
}

//Put the models on the initial position
void resetElements(void)
{
	posX = KeyFrame[0].posX;
	posZ = KeyFrame[0].posZ;
	rotMerry = KeyFrame[0].rotMerry;
}

//Interpolation function. Calculates intermidiate values to the movement
void interpolation(void)
{
	KeyFrame[playIndex].incX = (KeyFrame[playIndex + 1].posX - KeyFrame[playIndex].posX) / i_max_steps;
	KeyFrame[playIndex].incZ = (KeyFrame[playIndex + 1].posZ - KeyFrame[playIndex].posZ) / i_max_steps;
	KeyFrame[playIndex].rotInc = (KeyFrame[playIndex + 1].rotMerry - KeyFrame[playIndex].rotMerry) / i_max_steps;
}

// Deltatime
GLfloat deltaTime = 0.0f;	// Time between current frame and last frame
GLfloat lastFrame = 0.0f;  	// Time of last frame

float rotPuerta = 0.0f;
bool puertaAbierta = false;
float tiempo;


int main()
{
	// Init GLFW
	glfwInit();

	// Create a GLFWwindow object that we can use for GLFW's functions
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "PROYECTO FINAL - GOING MERRY", nullptr, nullptr);

	if (nullptr == window)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return EXIT_FAILURE;
	}

	glfwMakeContextCurrent(window);

	glfwGetFramebufferSize(window, &SCREEN_WIDTH, &SCREEN_HEIGHT);

	// Set the required callback functions
	glfwSetKeyCallback(window, KeyCallback);
	glfwSetCursorPosCallback(window, MouseCallback);

	// GLFW Options
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;

	// Initialize GLEW to setup the OpenGL Function pointers
	if (GLEW_OK != glewInit())
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return EXIT_FAILURE;
	}

	// Define the viewport dimensions
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	Shader lightingShader("Shaders/lighting.vs", "Shaders/lighting.frag");
	Shader lampShader("Shaders/lamp.vs", "Shaders/lamp.frag");
	Shader SkyBoxshader("Shaders/SkyBox.vs", "Shaders/SkyBox.frag");
	Shader Anim("Shaders/anim.vs", "Shaders/anim.frag");
	
	//Inner Lounge elements
	Model lounge((char*)"Models/Merry/lounge/lounge.obj");
	Model cocina((char*)"Models/Merry/cocina/cocina.obj");
	Model refrigerador((char*)"Models/Merry/refrigerador/refri.obj");
	Model refriPuerta((char*)"Models/Merry/refrigerador/refri_puerta_abajo.obj");
	Model miniMerry((char*)"Models/Merry/miniMerry/miniMerry.obj");
	Model mesa((char*)"Models/Merry/mesa_silla/mesa_silla.obj");
	Model vidrios_ventanas((char*)"Models/Merry/lounge/vidrios_ventanas.obj");
	Model ventana_puerta((char*)"Models/Merry/lounge/ventana_puerta.obj");
	Model puerta((char*)"Models/Merry/lounge/puerta.obj");
	Model rack_vino((char*)"Models/Merry/rack_vino/rack_vino.obj");
	Model cajon((char*)"Models/Merry/cocina/cajon/cajon.obj");

	//Outer ship models
	Model merry((char*)"Models/Merry/GoingMerry/merry.obj");
	Model bandera_n1((char*)"Models/Merry/GoingMerry/banderas/bandera_negra1.obj");
	Model bandera_n2((char*)"Models/Merry/GoingMerry/banderas/bandera_negra2.obj");
	Model bandera_bicolor((char*)"Models/Merry/GoingMerry/banderas/bicolor/bandera_bicolor.obj");
	Model bandera_grande((char*)"Models/Merry/GoingMerry/banderas/grande/bandera_grande.obj");
	Model bala((char*)"Models/Merry/GoingMerry/bala/bala.obj");
	Model canion((char*)"Models/Merry/GoingMerry/bala/canion.obj");

	//Inner Men's Room elements
	Model armario((char*)"Models/Merry/armario/armario.obj");
	Model sillon((char*)"Models/Merry/sillon/sillon.obj");
	Model sillon2((char*)"Models/Merry/sillon/sillon2.obj");
	Model alfombra((char*)"Models/Merry/alfombra/alfombra.obj");
	Model mesa2((char*)"Models/Merry/mesa/mesa.obj");

	//Surface model
	Model mar((char*)"Models/mar/mar.obj");

	//Keyframes initialization 
	for (int i = 0; i < MAX_FRAMES; i++)
	{
		KeyFrame[i].posX = 0;
		KeyFrame[i].posZ = 0;
		KeyFrame[i].incX = 0;
		KeyFrame[i].incZ = 0;
		KeyFrame[i].rotInc = 0;
		KeyFrame[i].rotMerry = 0;
	}

	GLfloat vertices[] =
	{
		// Positions            // Normals              // Texture Coords
		-0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,     0.0f,  0.0f,
		0.5f, -0.5f, -0.5f,     0.0f,  0.0f, -1.0f,     1.0f,  0.0f,
		0.5f,  0.5f, -0.5f,     0.0f,  0.0f, -1.0f,     1.0f,  1.0f,
		0.5f,  0.5f, -0.5f,     0.0f,  0.0f, -1.0f,     1.0f,  1.0f,
		-0.5f,  0.5f, -0.5f,    0.0f,  0.0f, -1.0f,     0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,     0.0f,  0.0f,

		-0.5f, -0.5f,  0.5f,    0.0f,  0.0f,  1.0f,     0.0f,  0.0f,
		0.5f, -0.5f,  0.5f,     0.0f,  0.0f,  1.0f,     1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,     0.0f,  0.0f,  1.0f,     1.0f,  1.0f,
		0.5f,  0.5f,  0.5f,     0.0f,  0.0f,  1.0f,  	1.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,    0.0f,  0.0f,  1.0f,     0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,    0.0f,  0.0f,  1.0f,     0.0f,  0.0f,

		-0.5f,  0.5f,  0.5f,    -1.0f,  0.0f,  0.0f,    1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,    -1.0f,  0.0f,  0.0f,    1.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,    -1.0f,  0.0f,  0.0f,    0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,    -1.0f,  0.0f,  0.0f,    0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,    -1.0f,  0.0f,  0.0f,    0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,    -1.0f,  0.0f,  0.0f,    1.0f,  0.0f,

		0.5f,  0.5f,  0.5f,     1.0f,  0.0f,  0.0f,     1.0f,  0.0f,
		0.5f,  0.5f, -0.5f,     1.0f,  0.0f,  0.0f,     1.0f,  1.0f,
		0.5f, -0.5f, -0.5f,     1.0f,  0.0f,  0.0f,     0.0f,  1.0f,
		0.5f, -0.5f, -0.5f,     1.0f,  0.0f,  0.0f,     0.0f,  1.0f,
		0.5f, -0.5f,  0.5f,     1.0f,  0.0f,  0.0f,     0.0f,  0.0f,
		0.5f,  0.5f,  0.5f,     1.0f,  0.0f,  0.0f,     1.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,     0.0f,  1.0f,
		0.5f, -0.5f, -0.5f,     0.0f, -1.0f,  0.0f,     1.0f,  1.0f,
		0.5f, -0.5f,  0.5f,     0.0f, -1.0f,  0.0f,     1.0f,  0.0f,
		0.5f, -0.5f,  0.5f,     0.0f, -1.0f,  0.0f,     1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,    0.0f, -1.0f,  0.0f,     0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,     0.0f,  1.0f,

		-0.5f,  0.5f, -0.5f,    0.0f,  1.0f,  0.0f,     0.0f,  1.0f,
		0.5f,  0.5f, -0.5f,     0.0f,  1.0f,  0.0f,     1.0f,  1.0f,
		0.5f,  0.5f,  0.5f,     0.0f,  1.0f,  0.0f,     1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,     0.0f,  1.0f,  0.0f,     1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,    0.0f,  1.0f,  0.0f,     0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,    0.0f,  1.0f,  0.0f,     0.0f,  1.0f
	};

	//Skybox face positions
	GLfloat skyboxVertices[] = {
		// Positions
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f,  1.0f
	};


	GLuint indices[] =
	{  // Note that we start from 0!
		0,1,2,3,
		4,5,6,7,
		8,9,10,11,
		12,13,14,15,
		16,17,18,19,
		20,21,22,23,
		24,25,26,27,
		28,29,30,31,
		32,33,34,35
	};

	// Positions all containers
	glm::vec3 cubePositions[] = {
		glm::vec3(0.0f,  0.0f,  0.0f),
		glm::vec3(2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f,  2.0f, -2.5f),
		glm::vec3(1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f)
	};

	// First, set the container's VAO (and VBO)
	GLuint VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	// normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// Set texture units
	lightingShader.Use();
	glUniform1i(glGetUniformLocation(lightingShader.Program, "material.diffuse"), 0);
	glUniform1i(glGetUniformLocation(lightingShader.Program, "material.specular"),1);

	//SkyBox
	GLuint skyboxVBO, skyboxVAO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

	// Load textures Skybox
	vector<const GLchar*> faces;
	faces.push_back("SkyBox/right.tga");
	faces.push_back("SkyBox/left.tga");
	faces.push_back("SkyBox/top.tga");
	faces.push_back("SkyBox/bottom.tga");
	faces.push_back("SkyBox/back.tga");
	faces.push_back("SkyBox/front.tga");

	GLuint cubemapTexture = TextureLoading::LoadCubemap(faces);

	//Perspective projection
	glm::mat4 projection = glm::perspective(camera.GetZoom(), (GLfloat)SCREEN_WIDTH / (GLfloat)SCREEN_HEIGHT, 0.1f, 1000.0f);

	// Game loop
	while (!glfwWindowShouldClose(window))
	{

		// Calculate deltatime of current frame
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();
		DoMovement();
		recorrido();
		lanzamiento();

		// Clear the colorbuffer
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	   
		// OpenGL options
		glEnable(GL_DEPTH_TEST);

		// Use cooresponding shader when setting uniforms/drawing objects
		lightingShader.Use();
		GLint viewPosLoc = glGetUniformLocation(lightingShader.Program, "viewPos");
		glUniform3f(viewPosLoc, camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);

		// DIRECTIONAL LIGHT
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.direction"), -0.2f, -1.0f, -0.3f);  //Direccion de la luz
		//Iluminación del escenario (ambiental y difusa)
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.ambient"),0.5f,0.5f,0.5f);  
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.diffuse"), 0.5f, 0.5f, 0.5f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.specular"),0.0f, 0.0f, 0.0f);

		// POINT LIGHT
			//Posición point light
		pointLightPos = glm::vec3(pointLightPosIni.x + sin(0.5 * tiempo) + posX, pointLightPosIni.y, pointLightPosIni.z + posZ);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].position"), pointLightPos.x, pointLightPos.y, pointLightPos.z); 
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].ambient"), Light1.x, Light1.y, Light1.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].diffuse"), Light1.x, Light1.y, Light1.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].specular"), 0.0f, 0.0f, 0.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].constant"), 1.0f);
		//Dispersión de la luz
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].linear"), 0.7f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].quadratic"),1.8f);
	
		// Set material properties
		glUniform1f(glGetUniformLocation(lightingShader.Program, "material.shininess"), 16.0f);  //Brillo del objeto

		// Create camera transformations
		glm::mat4 view;
		view = camera.GetViewMatrix();

		// Get the uniform locations
		GLint modelLoc = glGetUniformLocation(lightingShader.Program, "model");
		GLint viewLoc = glGetUniformLocation(lightingShader.Program, "view");
		GLint projLoc = glGetUniformLocation(lightingShader.Program, "projection");

		// Pass the matrices to the shader
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		glBindVertexArray(VAO);

		tiempo = glfwGetTime();  //Obtención de tiempo de sistema
		glm::mat4 tmp, tmp2, tmp3 = glm::mat4(1.0f); //Matrices temporales 

		//Carga de modelos

		//LOUNGE
		glm::mat4 model(1); //Clean matriz model
		model = glm::translate(model, glm::vec3(sin(0.5 * tiempo), 0.0f, 0.0f));  //Oscilación 
		tmp = model = glm::translate(model, glm::vec3(posX, 0.0f, posZ));  //Model translation for the route (keyframes)
		model = glm::rotate(model, glm::radians(rotMerry), glm::vec3(0.0f, 1.0f, 0.0));  //Model rotation for the route (keyframes)
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));  //Cambios del modelo hacia el shader
		miniMerry.Draw(lightingShader);  //Modelo timón
		mesa.Draw(lightingShader);   //Modelo mesa
		cocina.Draw(lightingShader);  //Modelo cocina
		rack_vino.Draw(lightingShader);  //Modelo rack de vino
		merry.Draw(lightingShader);  //Modelo barco
		lounge.Draw(lightingShader);   //Modelo Lounge
		armario.Draw(lightingShader);   //Modelo armario
		sillon.Draw(lightingShader);   //Modelo sillon
		sillon2.Draw(lightingShader);   //Modelo sillon 2
		alfombra.Draw(lightingShader);   //Modelo alfombra
		mesa2.Draw(lightingShader);   //Modelo mesa


		//REFRIGERATOR
		model = glm::mat4(1);  //Clean matriz model
		model = glm::translate(model, glm::vec3(sin(0.5 * tiempo), 0.0f, 0.0f)); //Osiclation
		model = glm::translate(model, glm::vec3(posX, 0.0f, posZ));  //Model translation for the route (keyframes)
		model = glm::rotate(model, glm::radians(rotMerry), glm::vec3(0.0f, 1.0f, 0.0)); //Model rotation for the route (keyframes)
		model = glm::translate(model, glm::vec3(-2.318, -0.217f, -4.937f));  //Traslación de refrigerador a su posición inicial
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model)); //Cambios del modelo hacia el shader
		refrigerador.Draw(lightingShader);  //Modelo refrigerador

		model = glm::mat4(1);//Clean matriz model
		model = glm::translate(model, glm::vec3(sin(0.5 * tiempo), 0.0f, 0.0f)); //Osiclation
		model = glm::translate(model, glm::vec3(posX, 0.0f, posZ));  //Model translation for the route (keyframes)
		model = glm::rotate(model, glm::radians(rotMerry), glm::vec3(0.0f, 1.0f, 0.0)); //Model rotation for the route (keyframes)
		model = glm::translate(model, glm::vec3(-2.25, -0.201f, -4.939f ));  //Refrigerators door translation to initial position
		model = glm::rotate(model, glm::radians(rotPuertaRefri), glm::vec3(0.0f, 1.0f, 0.0));  //Door rotation to open or close
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model)); //Model changes to shader
		refriPuerta.Draw(lightingShader);  

		//RIGHT DRAWER
		model = glm::mat4(1); //Clean matriz model
		model = glm::translate(model, glm::vec3(sin(0.5 * tiempo), 0.0f, 0.0f)); //Osiclation 
		model = glm::translate(model, glm::vec3(posX, 0.0f, posZ));  //Model translation for the route (keyframes)
		model = glm::rotate(model, glm::radians(rotMerry), glm::vec3(0.0f, 1.0f, 0.0)); //Model rotation for the route (keyframes)
		model = glm::translate(model, glm::vec3(-0.638f, -0.335f, movCajonZ));  //Drawer rotation on z axis to open or close 
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model)); //Model changes to shader
		cajon.Draw(lightingShader);  

		//LEFT DRAWER
		model = glm::mat4(1); //Clean matriz model
		model = glm::translate(model, glm::vec3(sin(0.5 * tiempo), 0.0f, 0.0f)); //Osiclation
		model = glm::translate(model, glm::vec3(posX, 0.0f, posZ));  //Model translation for the route (keyframes)
		model = glm::rotate(model, glm::radians(rotMerry), glm::vec3(0.0f, 1.0f, 0.0)); //Model rotation for the route (keyframes)
		model = glm::translate(model, glm::vec3(-1.32f, -0.335f, -0.611f)); //Drawer translation to the initial position 
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model)); //Model changes to shader
		cajon.Draw(lightingShader); 

		//CANNONBALL
		model = glm::mat4(1); //Clean matriz model
		model = glm::translate(model, glm::vec3(sin(0.5 * tiempo), 0.0f, 0.0f)); //Osiclation
		model = glm::translate(model, glm::vec3(posX, 0.0f, posZ));  //Model translation for the route (keyframes)
		tmp2 = model = glm::rotate(tmp, glm::radians(rotMerry), glm::vec3(0.0f, 1.0f, 0.0)); //Model rotation for the route (keyframes)
		model = glm::translate(tmp2, glm::vec3(-7.0f, -2.0f, -3.5f));  //Cannonball translation to the initial position
		model = glm::translate(model, glm::vec3(movBalaX, movBalaY, movBalaZ)); //Cannonball translation 
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model)); //Model changes to shader
		bala.Draw(lightingShader);  

		//CANNON
		model = glm::mat4(1); //Clean matriz model
		model = glm::translate(model, glm::vec3(sin(0.5 * tiempo), 0.0f, posZ)); //Osiclation
		model = glm::translate(model, glm::vec3(posX, 0.0f, posZ));  //Model translation for the route (keyframes)
		tmp3 = model = glm::rotate(tmp, glm::radians(rotMerry), glm::vec3(0.0f, 1.0f, 0.0)); //Model rotation for the route (keyframes)
		model = glm::translate(tmp3, glm::vec3(-7.3f, -2.106f, -3.6f));  //Cannon translation to the initial position
		model = glm::rotate(model, glm::radians(rotCanion), glm::vec3(0.0f, 0.0f, 1.0));  //Connon rotation to angle adjustment
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model)); //Model changes to shader
		canion.Draw(lightingShader);  

		//LOUNGE DOOR
		model = glm::mat4(1); //Clean matriz model
		model = glm::translate(model, glm::vec3(sin(0.5 * tiempo), 0.0f, 0.0f)); //Osiclation
		model = glm::translate(model, glm::vec3(posX, 0.0f, posZ));  //Model translation for the route (keyframes)
		model = glm::rotate(model, glm::radians(rotMerry), glm::vec3(0.0f, 1.0f, 0.0)); //Model rotation for the route (keyframes)
		model = glm::rotate(model, glm::radians(rotPuerta), glm::vec3(0.0f, 1.0f, 0.0f)); //Door rotation to open or close 
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model)); //Model changes to shader
		puerta.Draw(lightingShader);  

		//TRANSPARENCY OBJECTS
		glEnable(GL_BLEND); //Use alfa channel 
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		model = glm::mat4(1); //Limpia matriz de modelo
		model = glm::translate(model, glm::vec3(sin(0.5 * tiempo), 0.0f, 0.0f)); //Osiclation
		model = glm::translate(model, glm::vec3(posX, 0.0f, posZ));  //Model translation for the route (keyframes)
		model = glm::rotate(model, glm::radians(rotMerry), glm::vec3(0.0f, 1.0f, 0.0)); //Model rotation for the route (keyframes)
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model)); //Model changes to shader
		glUniform1i(glGetUniformLocation(lightingShader.Program, "activaTransparencia"), 0); 
		glUniform4f(glGetUniformLocation(lightingShader.Program, "colorAlpha"), 0.78, 0.984, 1.0, 0.5);  //Transparency color
		vidrios_ventanas.Draw(lightingShader); 

		model = glm::mat4(1); //Clean matriz model
		model = glm::translate(model, glm::vec3(sin(0.5*tiempo), 0.0f, 0.0f)); //Osiclation
		model = glm::translate(model, glm::vec3(posX, 0.0f, posZ));  //Model translation for the route (keyframes)
		model = glm::rotate(model, glm::radians(rotPuerta), glm::vec3(0.0f, 1.0f, 0.0f)); //Window rotation when the door is opened or closed
		model = glm::rotate(model, glm::radians(rotMerry), glm::vec3(0.0f, 1.0f, 0.0));  //Model rotation for the route (keyframes)
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model)); //Model changes to the lighting shader
		ventana_puerta.Draw(lightingShader);  //Modelo ventana de la puerta

		glDisable(GL_BLEND);  //Desactiva el canal alfa 
		glUniform4f(glGetUniformLocation(lightingShader.Program, "colorAlpha"), 1.0, 1.0, 1.0, 1.0); //Transparency color

		Anim.Use();  //Shader Anim 

		modelLoc = glGetUniformLocation(Anim.Program, "model");
		viewLoc = glGetUniformLocation(Anim.Program, "view");
		projLoc = glGetUniformLocation(Anim.Program, "projection");

		// Set matrices
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		// Paso de parámetros hacia el shader Anim
		glUniform1f(glGetUniformLocation(Anim.Program, "time"), 2*tiempo); //Time value
		glUniform1i(glGetUniformLocation(Anim.Program, "option"), 1);  //Option 1 - Sea movement 

		//MAR
		model = glm::mat4(1); //Clean matriz model
		model = glm::translate(model, glm::vec3(0.0f, -7.0f, 0.0f));  //Sea surface translation
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));  //Model changes to the shader
		mar.Draw(Anim);  //Modelo mar

		//BANDERAS NEGRAS
		glUniform1i(glGetUniformLocation(Anim.Program, "option"), 2);
		model = glm::mat4(1); //Clean matriz model
		model = glm::translate(model, glm::vec3(sin(0.5 * tiempo), 0.0f, 0.0f)); //Osiclation
		model = glm::translate(model, glm::vec3(posX, 0.0f, posZ));  //Model translation for the route (keyframes)
		model = glm::rotate(model, glm::radians(rotMerry), glm::vec3(0.0f, 1.0f, 0.0)); //Model rotation for the route (keyframes)
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model)); //Model changes to the shader
		bandera_n1.Draw(Anim); 
		bandera_n2.Draw(Anim); 

		//BANDERA GRANDE Y BANDERA BICOLOR
		glUniform1i(glGetUniformLocation(Anim.Program, "option"), 3);
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(sin(0.5 * tiempo), 0.0f, 0.0f)); //Osiclation
		model = glm::translate(model, glm::vec3(posX, 0.0f, posZ));  //Model translation for the route (keyframes)
		model = glm::rotate(model, glm::radians(rotMerry), glm::vec3(0.0f, 1.0f, 0.0)); //Model rotation for the route (keyframes)
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model)); //Model changes to the shader
		bandera_bicolor.Draw(Anim); 
		bandera_grande.Draw(Anim);  

		glBindVertexArray(0);

		// Draw skybox as last
		glDepthFunc(GL_LEQUAL);  // Change depth function so depth test passes when values are equal to depth buffer's content
		SkyBoxshader.Use();
		view = glm::mat4(glm::mat3(camera.GetViewMatrix()));	// Remove any translation component of the view matrix
		glUniformMatrix4fv(glGetUniformLocation(SkyBoxshader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));  //Matríz de vista hacia el shader SkyBox
		glUniformMatrix4fv(glGetUniformLocation(SkyBoxshader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection)); //Matríz de proyeccion hacia el shader SkyBox

		// skybox cube
		glBindVertexArray(skyboxVAO);  
		glActiveTexture(GL_TEXTURE1);  //Texture 
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);  //Draw cube
		glBindVertexArray(0);  
		glDepthFunc(GL_LESS); // Set depth function back to default

		// Swap the screen buffers
		glfwSwapBuffers(window);
	}

	//Delated arrays and Buffers created
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteVertexArrays(1, &skyboxVAO);
	glDeleteBuffers(1, &skyboxVBO);

	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();

	return 0;
}

void recorrido()
{

	//Ship movement

	if (play)
	{
		if (i_curr_steps >= i_max_steps) //end of animation between frames?
		{
			playIndex++;
			if (playIndex > FrameIndex - 2)	//end of total animation?
			{
				playIndex = 0;
				play = false;
				resetElements();
			}
			else //Next frame interpolations
			{
				i_curr_steps = 0; //Reset counter
								  //Interpolation
				interpolation();
			}
		}
		else
		{
			//Draw animation
			posX += KeyFrame[playIndex].incX;
			posZ += KeyFrame[playIndex].incZ;
			rotMerry += KeyFrame[playIndex].rotInc;

			i_curr_steps++;
		}

	}
}

//Parabolic shooing for firing cannonball
void lanzamiento() {

	if (lanzar) {

		double ang = glm::radians(rotCanion);

		if(movBalaY > -5){
			//Depending on the position of the ship, the bullet will move in different X or Z axis.
			//It is calculated using the parabolic shot formula given a position coordinate,
			//an angle of rotation and the initial velocity of the launch.
			if (rotMerry >= -8 && rotMerry < 95) {
				movBalaY = tan(ang) * movBalaX - (g / (2 * vi * vi * cos(ang) * cos(ang))) * movBalaX * movBalaX;
				movBalaX -= 0.05f;
			}
			if (rotMerry >= 95 && rotMerry < 185) {
				movBalaY = tan(ang) * movBalaZ - (g / (2 * vi * vi * cos(ang) * cos(ang))) * movBalaZ * movBalaZ;
				movBalaZ += 0.05f;
		    }
			if (rotMerry >= 185 && rotMerry < 275) {
				movBalaY = tan(ang) * movBalaX - (g / (2 * vi * vi * cos(ang) * cos(ang))) * movBalaX * movBalaX;
				movBalaX += 0.05f;
			}
			if (rotMerry >= 275 && rotMerry < 360) {
				movBalaY = tan(ang) * movBalaZ - (g / (2 * vi * vi * cos(ang) * cos(ang))) * movBalaZ * movBalaZ;
				movBalaZ -= 0.05f;
			}
				
		}
		else { 
			//Reset variables to the initial position
				rotCanion = 0.0f;
				movBalaX = 0.0f;
				movBalaY = 0.0f;
				movBalaZ = 0.0f;
	     		lanzar = false;
		}
	}
}


// Moves/alters the camera positions based on user input
void DoMovement()
{

	// Camera controls
	if (keys[GLFW_KEY_W] || keys[GLFW_KEY_UP])
	{
		camera.ProcessKeyboard(FORWARD, deltaTime);
	}

	if (keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN])
	{
		camera.ProcessKeyboard(BACKWARD, deltaTime);

	}

	if (keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT])
	{
		camera.ProcessKeyboard(LEFT, deltaTime);
	}

	if (keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT])
	{
		camera.ProcessKeyboard(RIGHT, deltaTime);
	}

	//Cannon rotation 
	if (keys[GLFW_KEY_V]) {
		if (rotCanion >= -25 && rotCanion < 18) {  
			rotCanion += 0.1; // Cannon downward rotation if between -25° and 18°.
		}
		else {
			rotCanion = 18;  //Secures that the canon rotation is the upper limit value
		}
		
	}
	if (keys[GLFW_KEY_N]) {
		if (rotCanion > -25 && rotCanion <= 18) {
			rotCanion -= 0.1; //Rotation of the cannon upwards if between -25° and 18°.
		}
		else {
			rotCanion = -25; //Secures that the barrel rotation is the lower limit value
		}
	}
}

// Is called whenever a key is pressed/released via GLFW
void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
	if (GLFW_KEY_ESCAPE == key && GLFW_PRESS == action)  
	{
		glfwSetWindowShouldClose(window, GL_TRUE); //Cierre de ventana
	}

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
		{
			keys[key] = true;
		}
		else if (action == GLFW_RELEASE)
		{
			keys[key] = false;
		}
	}

	//Door Lounge movement
	if (keys[GLFW_KEY_P])
	{
		if (puertaAbierta) {
			rotPuerta = 0.0f;   //With no door rotation
			puertaAbierta = false; //Notifies door closed
		}
		else {
			rotPuerta = -90.0f;   //Rotation of the door 90°.
			puertaAbierta = true; // Notifies door opened
		}

	}

	//Door movement Refrigerator
	if (keys[GLFW_KEY_K])
	{
		if (refriAbierto) {
			rotPuertaRefri = 0.0f;  //With no door rotation
			refriAbierto = false;  //Notifies refrigerator closed
		}
		else {
			rotPuertaRefri = 90.0f;  //Rotation of the door 90°.
			refriAbierto = true;  // Notifies refrigerator opened
		}

	}

	//Drawer movement
	if (keys[GLFW_KEY_C]) {
		if (cajonAbierto) {
			movCajonZ += 0.3f;  // Drawer movement 0.3 units towards positive z-axis
			cajonAbierto = false;  //Notifies closed drawer
		}
		else {
			movCajonZ -= 0.3f; // Drawer movement 0.3 units towards negative axis z
			cajonAbierto = true; //Notifies open drawer
		}
	}

	//Luz de lámpara
	if (keys[GLFW_KEY_L])
	{
		if (lamparaOn) {
			Light1 = glm::vec3(0);  //Without light color
			lamparaOn = false;  //Notifies that the light is off
		}
		else {
			Light1 = glm::vec3(1.0f, 1.0f, 1.0f);   //White light color
			lamparaOn = true;  //Notifies that the light is on
		}

	}

	//Ship movement via keyframes
	if (keys[GLFW_KEY_M])
	{

		if (play == false)
		{

			asignarValores();   //Assigning values to each frame
			resetElements(); 
			//First Interpolation				
			interpolation();  //Obtaining first intermediate values by interpolation

			play = true;   //Activate variable to start the run
			playIndex = 0;  
			i_curr_steps = 0;
		}
		else
		{
			play = false;  //Deactivate variable to end the run
		}

	}

	//Cannonball firing
	if (keys[GLFW_KEY_B])
	{
		if (lanzar == false)
		{
			lanzar = true;  //Activate variable to start bullet launching
		}
	}
}

void MouseCallback(GLFWwindow *window, double xPos, double yPos)
{
	if (firstMouse)
	{
		lastX = xPos;
		lastY = yPos;
		firstMouse = false;
	}

	GLfloat xOffset = xPos - lastX;
	GLfloat yOffset = lastY - yPos;  // Reversed since y-coordinates go from bottom to left

	lastX = xPos;
	lastY = yPos;

	camera.ProcessMouseMovement(xOffset, yOffset);
}