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
void animacion();
void lanzamiento();

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;
int SCREEN_WIDTH, SCREEN_HEIGHT;

// Camera
Camera  camera(glm::vec3(0.0f, 0.0f, 2.0f));
GLfloat lastX = WIDTH / 2.0;
GLfloat lastY = HEIGHT / 2.0;
bool keys[1024];
bool firstMouse = true;
// Light attributes
glm::vec3 lightPos(0.0f, 0.0f, 0.0f);
bool active;
bool lamparaOn = false;

bool detenerMerry = false;

//Animación de bala
float movBalaX = 0.0f;
float movBalaY = 0.0f;
bool lanzar = false;
float rotCanion = 0.0f;
double vi = 20.0f;
#define g 9.81

glm::vec3 PosIni(0.0f, 0.0f, 3.0f);

// Keyframes
float posX = PosIni.x, posZ = PosIni.z, rotMerry = 0;
#define MAX_FRAMES 60
int i_max_steps = 190;
int i_curr_steps = 0;


typedef struct w
{
	//Variables para GUARDAR Key Frames
	float posX;		//Variable para PosicionX
	float posZ;		//Variable para PosicionZ
	float posX2;		//Variable para PosicionX
	float posY2;		//Variable para PosicionY
	float posZ2;		//Variable para PosicionZ
	float incX;		//Variable para IncrementoX
	float incZ;		//Variable para IncrementoZ
	float incX2;		//Variable para IncrementoX
	float incY2;		//Variable para IncrementoY
	float incZ2;		//Variable para IncrementoZ
	float rotInc;
	float rotInc2;
	float rotMerry;
	float rotColaMerry;

}FRAME;

FRAME KeyFrame[MAX_FRAMES];

int FrameIndex = 0;			//introducir datos
bool play = false;
int playIndex = 0;

// Positions of the point lights
glm::vec3 pointLightPositions[] = {
	glm::vec3(0.0f,1.1f, 0.7f),

};

GLfloat valoresPosX[]     = { 0.0f, 0.15f, 0.3f,   0.3f,  0.15f, 0.0f,    0.0f,  0.15f, 0.3f,    0.3f,  0.15f, 0.0f,  //   
							  2.0f,  4.0f,  2.0f,  2.0f,  4.0f,  2.0f,    2.0f,  4.0f,  2.0f,    2.0f,  4.0f,  2.0f,
							  0.0f, 10.0f, 20.0f,  30.0f, 40.0f, 50.0f,   60.0f, 70.0f, 80.0f,   90.0f, 100.0f, 110.0f,  //Movimiento hacia la derecha
	                            
	                          
	                          4.0f,  2.0f,  0.0f,  0.0f, 0.15f, 0.3f,    0.3f,  0.15f, 0.0f,    0.0f, 0.15f,  0.3f,    0.3f, 0.15f, 0.0f, -2.0f,-4.0f,-8.0f,  -4.0f,-2.0f,0.0f, };

GLfloat valoresPosZ[]     = { 0.0f, 10.0f, 20.0f, 30.0f, 40.0f, 50.0f, 60.0f, 70.0f, 80.0f, 90.0f, 100.0f, 110.0f,  //Movimiento hacia adelante
							  110.0f, 110.0f, 110.0f,  //Se mantiene en la posición z = 110 mientras gira a la derecha
	                          110.0f, 109.85, 109.7, 109.7, 109.85, 110.0f,  110.0f, 109.85, 109.7,109.7, 109.85, 110.0f, 
							  110.0f, 110.0f, 110.0f,  //Se mantiene en la posición z = 110 mientras gira a la derecha
	                          110.0f, 100.0f, 90.0f, 80.0f, 70.0f, 60.0f, 50.0f, 40.0f, 30.0f, 20.0f, 10.0f, 0.0f,  //Movimiento hacia atrás
							  0.0f, 0.0f,  0.0f,  //Se mantiene en la posición z = 0 mientras gira a la derecha
							  0.0f, 0.15f, 0.3f,   0.3f,  0.15f, 0.0f,    0.0f,  0.15f, 0.3f,    0.3f,  0.15f, 0.0f,
							  0.0f, 0.0f,  0.0f  //Se mantiene en la posición z = 0 mientras gira a la derecha
                            };

GLfloat valoresRotMerry[] = { 4.0f, 4.0f,  4.0f,  -8.0f, -8.0f, -8.0f,  4.0f,  4.0f,  4.0f,  -8.0f, -8.0f,  0.0f,    //Rotación ligera mientras avanza hacia adelante
                              -30.0f, -60.0f, -90.0f,   //Giro hacia la derecha
							  -94.0f, -94.0f,  -94.0f, -90.0f, -90.0f, -90.0f,  -84.0f,  -84.0f,  -84.0f,  -90.0f, -90.0f,  -90.0f,    //Rotación ligera mientras avanza hacia la derecha
                              -120.0f, -150.0f, -180.0f,  //Giro hacia la derecha
	                          -184.0f, -184.0f, 184.0f,  -180.0f, -180.0f, -180.0f,  -184.0f, -184.0f, 184.0f,  -180.0f, -180.0f, -180.0f, //Rotación ligera mientras avanza hacia atrás
							  -210.0f, -240.0f, -270.0f,  //Giro hacia la derecha
	                          -274.0f, -274.0f, -274.0f,  -270.0f, -270.0f, -270.0f, -274.0f, -274.0f, -274.0f,  -270.0f, -270.0f, -270.0f, //Rotación ligera mientras avanza hacia la izquierda
							  -300.0f, -330.0f, -360.0f,  //Giro hacia la derecha
							 };

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

void resetElements(void)
{
	posX = KeyFrame[0].posX;
	posZ = KeyFrame[0].posZ;
	rotMerry = KeyFrame[0].rotMerry;
}

void interpolation(void)
{
	KeyFrame[playIndex].incX = (KeyFrame[playIndex + 1].posX - KeyFrame[playIndex].posX) / i_max_steps;
	KeyFrame[playIndex].incZ = (KeyFrame[playIndex + 1].posZ - KeyFrame[playIndex].posZ) / i_max_steps;
	KeyFrame[playIndex].rotInc = (KeyFrame[playIndex + 1].rotMerry - KeyFrame[playIndex].rotMerry) / i_max_steps;
}

glm::vec3 Light1 = glm::vec3(0);

// Deltatime
GLfloat deltaTime = 0.0f;	// Time between current frame and last frame
GLfloat lastFrame = 0.0f;  	// Time of last frame

float rotPuerta = 0.0f;
bool puertaAbierta = false;
float tiempo;
float tiempo2 = glfwGetTime();;


int main()
{
	// Init GLFW
	glfwInit();
	// Set all the required options for GLFW
	/*glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);*/

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
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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
	
	//Modelos dentro del Lounge
	Model lounge((char*)"Models/Merry/lounge/lounge.obj");
	Model cocina((char*)"Models/Merry/cocina/cocina.obj");
	Model refrigerador((char*)"Models/Merry/refrigerador/refri_completo.obj");
	Model miniMerry((char*)"Models/Merry/miniMerry/miniMerry.obj");
	Model mesa((char*)"Models/Merry/mesa_silla/mesa_silla.obj");
	Model vidrios_ventanas((char*)"Models/Merry/lounge/vidrios_ventanas.obj");
	Model ventana_puerta((char*)"Models/Merry/lounge/ventana_puerta.obj");
	Model puerta((char*)"Models/Merry/lounge/puerta.obj");
	Model rack_vino((char*)"Models/Merry/rack_vino/rack_vino.obj");

	//Modelos externos del barco
	Model merry((char*)"Models/Merry/GoingMerry/merry.obj");
	Model bandera_n1((char*)"Models/Merry/GoingMerry/banderas/bandera_negra1.obj");
	Model bandera_n2((char*)"Models/Merry/GoingMerry/banderas/bandera_negra2.obj");
	Model bandera_bicolor((char*)"Models/Merry/GoingMerry/banderas/bicolor/bandera_bicolor.obj");
	Model bandera_grande((char*)"Models/Merry/GoingMerry/banderas/grande/bandera_grande.obj");
	Model bala((char*)"Models/Merry/GoingMerry/bala/bala.obj");
	Model canion((char*)"Models/Merry/GoingMerry/bala/canion.obj");

	Model mar((char*)"Models/mar/mar.obj");

	//Inicialización de KeyFrames
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

	// Load textures
	vector<const GLchar*> faces;
	faces.push_back("SkyBox/right.tga");
	faces.push_back("SkyBox/left.tga");
	faces.push_back("SkyBox/top.tga");
	faces.push_back("SkyBox/bottom.tga");
	faces.push_back("SkyBox/back.tga");
	faces.push_back("SkyBox/front.tga");

	/*faces.push_back("SkyBox/1/right.tga");
	faces.push_back("SkyBox/1/left.tga");
	faces.push_back("SkyBox/1/top.tga");
	faces.push_back("SkyBox/1/bottom.tga");
	faces.push_back("SkyBox/1/back.tga");
	faces.push_back("SkyBox/1/front.tga");*/

	GLuint cubemapTexture = TextureLoading::LoadCubemap(faces);

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
		animacion();
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


		// Directional light
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.direction"), -0.2f, -1.0f, -0.3f);  //Direccion de la luz
		//Iluminación del escenario (ambiental y difusa)
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.ambient"),0.3f,0.3f,0.3f);  
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.diffuse"), 0.3f, 0.3f, 0.3f);

		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.specular"),0.0f, 0.0f, 0.0f);


		// Point light 1
	    glm::vec3 lightColor;
		lightColor.x= abs(sin(glfwGetTime() *Light1.x));
		lightColor.y= abs(sin(glfwGetTime() *Light1.y));
		lightColor.z= sin(glfwGetTime() *Light1.z);

		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].position"), pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);  //Posición 0
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].ambient"), Light1.x, Light1.y, Light1.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].diffuse"), Light1.x, Light1.y, Light1.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].specular"), 0.0f, 0.0f, 0.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].constant"), 1.0f);
		//que tanto abarca la iluminación (disperción)
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].linear"), 0.7f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].quadratic"),1.8f);
	
		// Set material properties
		glUniform1f(glGetUniformLocation(lightingShader.Program, "material.shininess"), 32.0f);  //Brillo del objeto

		tiempo = glfwGetTime();

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
		glm::mat4 tmp = glm::mat4(1.0f); //Temp

		//Carga de modelo 
		//Personaje
		view = camera.GetViewMatrix();
		glm::mat4 model(1);
		model = glm::translate(model, glm::vec3(sin(0.5 * tiempo), 0.0f, 0.0f));
		model = glm::translate(model, glm::vec3(posX, 0.0f, posZ));
		model = glm::rotate(model, glm::radians(rotMerry), glm::vec3(0.0f, 1.0f, 0.0));
		//glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		miniMerry.Draw(lightingShader);
		refrigerador.Draw(lightingShader);
		mesa.Draw(lightingShader);
		cocina.Draw(lightingShader);
		rack_vino.Draw(lightingShader);
		merry.Draw(lightingShader);
		lounge.Draw(lightingShader);

		//Cañón y bala
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(movBalaX, movBalaY, 0.0f));
		model = glm::translate(model, glm::vec3(sin(0.5 * tiempo), 0.0f, posZ));
		model = glm::rotate(model, glm::radians(rotMerry), glm::vec3(0.0f, 1.0f, 0.0));
		
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		bala.Draw(lightingShader);

		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(sin(0.5 * tiempo), 0.0f, posZ));
		model = glm::rotate(model, glm::radians(rotMerry), glm::vec3(0.0f, 1.0f, 0.0));
		model = glm::rotate(model, glm::radians(rotCanion), glm::vec3(0.0f, 0.0f, 1.0));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		canion.Draw(lightingShader);

		//Movimiento de puerta
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(sin(0.5 * tiempo), 0.0f, posZ));
		model = glm::rotate(model, glm::radians(rotPuerta), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(rotMerry), glm::vec3(0.0f, 1.0f, 0.0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		puerta.Draw(lightingShader);

		//OBJETOS CON TRANSPARENCIA
		glEnable(GL_BLEND);//Avtiva la funcionalidad para trabajar el canal alfa
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(sin(0.5 * tiempo), 0.0f, posZ));
		model = glm::rotate(model, glm::radians(rotMerry), glm::vec3(0.0f, 1.0f, 0.0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniform1i(glGetUniformLocation(lightingShader.Program, "activaTransparencia"), 0);
		glUniform4f(glGetUniformLocation(lightingShader.Program, "colorAlpha"), 0.78, 0.984, 1.0, 0.5);
		vidrios_ventanas.Draw(lightingShader);

		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(sin(0.5*tiempo), 0.0f, posZ));
		model = glm::rotate(model, glm::radians(rotPuerta), glm::vec3(0.0f, 1.0f, 0.0f));
	
		model = glm::rotate(model, glm::radians(rotMerry), glm::vec3(0.0f, 1.0f, 0.0));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		ventana_puerta.Draw(lightingShader);

		glDisable(GL_BLEND);  //Desactiva el canal alfa 
		glUniform4f(glGetUniformLocation(lightingShader.Program, "colorAlpha"), 1.0, 1.0, 1.0, 1.0);


		Anim.Use();

		modelLoc = glGetUniformLocation(Anim.Program, "model");
		viewLoc = glGetUniformLocation(Anim.Program, "view");
		projLoc = glGetUniformLocation(Anim.Program, "projection");

		// Set matrices
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniform1f(glGetUniformLocation(Anim.Program, "time"), tiempo);
		glUniform1i(glGetUniformLocation(Anim.Program, "option"), 1);

		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(0.0f, -5.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		mar.Draw(Anim);

		glUniform1i(glGetUniformLocation(Anim.Program, "option"), 2);
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(sin(0.5 * tiempo), 0.0f, posZ));
		model = glm::rotate(model, glm::radians(rotMerry), glm::vec3(0.0f, 1.0f, 0.0));
		//glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		bandera_n1.Draw(Anim);
		bandera_n2.Draw(Anim);

		glUniform1i(glGetUniformLocation(Anim.Program, "option"), 3);
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(sin(0.5 * tiempo), 0.0f, posZ));
		model = glm::rotate(model, glm::radians(rotMerry), glm::vec3(0.0f, 1.0f, 0.0));
		//glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		bandera_bicolor.Draw(Anim);
		bandera_grande.Draw(Anim);

		glBindVertexArray(0);


		// Also draw the lamp object, again binding the appropriate shader
		lampShader.Use();
		// Get location objects for the matrices on the lamp shader (these could be different on a different shader)
		modelLoc = glGetUniformLocation(lampShader.Program, "model");
		viewLoc = glGetUniformLocation(lampShader.Program, "view");
		projLoc = glGetUniformLocation(lampShader.Program, "projection");

		// Set matrices
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
		tmp = model = glm::mat4(1);
		model = glm::translate(model, pointLightPositions[0]);
		model = glm::scale(model, glm::vec3(0.05f)); // Make it a smaller cube
		//pointLightPositions[0] = glm::vec3(sin(0.5 * tiempo), 0.0f, 0.0f);
		//pointLightPositions[0] = glm::vec3(posX, 0.0f, posZ);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		
		glBindVertexArray(0);


		// Draw skybox as last
		glDepthFunc(GL_LEQUAL);  // Change depth function so depth test passes when values are equal to depth buffer's content
		SkyBoxshader.Use();
		view = glm::mat4(glm::mat3(camera.GetViewMatrix()));	// Remove any translation component of the view matrix
		glUniformMatrix4fv(glGetUniformLocation(SkyBoxshader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(SkyBoxshader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

		// skybox cube
		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS); // Set depth function back to default

		// Swap the screen buffers
		glfwSwapBuffers(window);
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteVertexArrays(1, &skyboxVAO);
	glDeleteBuffers(1, &skyboxVBO);

	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();

	return 0;
}


void animacion()
{

	//Movimiento del personaje

	if (play)
	{
		if (i_curr_steps >= i_max_steps) //end of animation between frames?
		{
			playIndex++;
			if (playIndex > FrameIndex - 2)	//end of total animation?
			{
				playIndex = 0;
				play = false;
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

void lanzamiento() {
	if (lanzar) {

		double ang = glm::radians(rotCanion);

		if(movBalaY > -5){
			
		movBalaY = tan(ang) * movBalaX - (g / (2*vi*vi *cos(ang)*cos(ang)))* movBalaX* movBalaX;
		movBalaX += 0.05f;

		}
		else {
			rotCanion = 0.0f;
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

	

}

// Is called whenever a key is pressed/released via GLFW
void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
	if (keys[GLFW_KEY_M])
	{
		
		if (play == false)
		{
			
			asignarValores();
			resetElements();
			//First Interpolation				
			interpolation();

			play = true;
			playIndex = 0;
			i_curr_steps = 0;
		}
		else
		{
			play = false;
		}

	}

	if (keys[GLFW_KEY_B])
	{
		if (lanzar)
		{
			lanzar = false;
		}
		else {
			lanzar = true;
			rotCanion = 30.0f;
			movBalaX = 0.0f;
			movBalaY = 0.0f;
		}
	}

	


	if (GLFW_KEY_ESCAPE == key && GLFW_PRESS == action)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
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

	if (keys[GLFW_KEY_SPACE])
	{
		active = !active;
		if (active)
		{
			Light1 = glm::vec3(1.0f, 0.0f, 1.0f);

		}
		else
		{
			Light1 = glm::vec3(0);//Cuado es solo un valor en los 3 vectores pueden dejar solo una componente

		}
	}

	if (keys[GLFW_KEY_P])
	{
		if (puertaAbierta) {
			rotPuerta = 0.0f;
			puertaAbierta = false;
		}
		else {
			rotPuerta = -90.0f;
			puertaAbierta = true;
		}

	}

	if (keys[GLFW_KEY_L])
	{
		if (lamparaOn) {
			Light1 = glm::vec3(0);
			lamparaOn = false;
		}
		else {
			Light1 = glm::vec3(1.0f, 1.0f, 1.0f);
			lamparaOn = true;
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