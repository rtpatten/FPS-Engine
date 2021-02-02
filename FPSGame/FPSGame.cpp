#include "FPSGame.h"

// Include standard headers
#include <stdio.h>
#include <stdlib.h>

// Include GLFW
#include <glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include <common/shader.hpp>

int main(void)
{
	//Initialize window
	bool windowInitialized = initializeWindow();
	if (!windowInitialized) return -1;
	
	// Create and compile our GLSL program from the shaders
	programID = LoadShaders("Shaders/MapVertexShader.vertexshader", "Shaders/MapFragmentShader.fragmentshader");
	// Create and compile sun shader
	sunShaderID = LoadShaders("Shaders/SunVertexShader.vertexshader", "Shaders/SunFragmentShader.fragmentshader");
	// Create and compile minimap shader
	minimapShaderID = LoadShaders("Shaders/MiniMapVertexShader.vertexshader", "Shaders/MiniMapFragmentShader.fragmentshader");
	
	// Map initialization
	bool mapInitialized = initializeMap();
	if (!mapInitialized) return -1;

	//Initialize vertex buffer
	bool sunInitialized = initializeSun();
	if (!sunInitialized) return -1;

	//Initialize minimap
	bool minimapInitialized = initializeMinimap();
	if (!minimapInitialized) return -1;

	
	//Initialize MVP matrix
	initializeMVPMatrix();

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	//start animation loop until escape key is pressed
	do {
		// Update position of the camera modifying the view matrix depending on user input
		updateMVPLoop();

		// openGL directives to draw everything
		updateAnimationLoop();

	} // Check if the ESC key was pressed or the window was closed
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);

	//Cleanup and close window
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	cleanupVertexbuffer();
	glDeleteProgram(programID);
	glDeleteProgram(sunShaderID);
	glDeleteProgram(minimapShaderID);
	closeWindow();
	return 0;
}

/*
	UPDATE ANIMATION LOOP
*/
void updateAnimationLoop()
{
	// Clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	// draw map
	updateMap();

	// draw sun
	updateSun();

	glDisable(GL_DEPTH_TEST);
	// draw minimap
	updateMinimap();

	// Swap buffers
	glfwSwapBuffers(window);
	glfwPollEvents();
}

/*
	DRAW THE MAP
*/
void updateMap() {
	// Use our main shader
	glUseProgram(programID);

	//Set the matrix as the uniform
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

	//Set the vector of the position of the camera in world space
	GLfloat cameraPosition[] = { curr_x, curr_y, curr_z };
	glUniform3fv(cameraPosID, 1, &cameraPosition[0]);

	glUniform2fv(scrID, 1, &scr_center[0]);
	/*
		//Update sun position depending on time
		double currentTime = glfwGetTime();
		sunPosition.y = sin(glm::radians(currentTime)) * sunDistance;
		sunPosition.x = cos(glm::radians(currentTime)) * sunDistance;
	*/
	//Set the uniform vector for sun position
	glUniform3fv(sunPosID, 1, &sunPosition[0]);

	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)0);

	// 2nd attribute buffer: uv vertices of map
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));

	// 3rd attribute buffer: normals vertices
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(5 * sizeof(GLfloat)));

	// Draw the triangle !
	glDrawArrays(GL_TRIANGLES, 0, vertexbuffer_size); // 3 indices starting at 0 -> 1 triangle

	//Disable vertexAttribArray
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
}

/*
	DRAW THE SUN
*/
void updateSun() {
	//Set the configuration for the sun. Using two sets of shaders, one for the sun only
	glUseProgram(sunShaderID);

	//Set the matrix as the uniform
	glUniformMatrix4fv(MatrixIDSunShader, 1, GL_FALSE, &MVP[0][0]);
	/*
		//Update and set the Model matrix of the sun
		SunModel = glm::translate(mat4(1.0), vec3(sunDistance*glm::radians(currentTime), 0, sunDistance * glm::radians(currentTime)));
		glUniformMatrix4fv(ModelSunID, 1, GL_FALSE, &SunModel[0][0]);
	*/
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, sun_vertexbuffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glDrawArrays(GL_TRIANGLES, 0, sun_vertexbuffer_size);

	glDisableVertexAttribArray(0);
}

/*
	DRAW THE MINIMAP
*/
void updateMinimap() {
	//Set the configuration for the sun. Using two sets of shaders, one for the sun only
	glUseProgram(minimapShaderID);

	//Set the matrix as the uniform
	glUniformMatrix4fv(minimapMatrixID, 1, GL_FALSE, &MinimapModel[0][0]);

	//Set the screen's resolution as uniform
	glUniform2fv(minimapScrResID, 1, &scrRes.x);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, mm_vertexbuffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void *)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void *)(3 * sizeof(GLfloat)));

	glDrawArrays(GL_TRIANGLES, 0, mm_vertexbuffer_size);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}

/*
	UPDATE POSITION OF ELEMENTS
*/
void updateMVPLoop() {
	/*
	* Get current time to calculate the delta time respecting last rendering iteration,
	* thus all positional and directional changes can adapt to the hardware speed we're
	* executing on
	*/
	double currentTime = glfwGetTime();
	double deltaTime = (currentTime - oldTime);

	//Get mouse actions
	glfwGetCursorPos(window, &xpos, &ypos);
	glfwSetCursorPos(window, wwidth / 2, wheight / 2);
	
	//Get viewing angles
	yaw += mouseSpeed * deltaTime * float((wwidth / 2) - xpos);
	pitch += mouseSpeed * deltaTime * float((wheight / 2) - ypos);

	// Limit movement along Y axis
	if (pitch > 89.0f) pitch = 89.0f;	
	if (pitch < -89.0f) pitch = -89.0f;

	// Calculate horizontal deplacements
	float dx = sin(glm::radians(yaw))*speed*deltaTime;	// We move in X axis in the same proportion we align to it
	float dz = cos(glm::radians(yaw))*speed*deltaTime;  // We move in Z axis in the same proportion we align to it

	// Capture horizontal movements matrix
	if (glfwGetKey(window, GLFW_KEY_A)) { curr_x -= dz; curr_z += dx; }
	else if (glfwGetKey(window, GLFW_KEY_D)) { curr_x += dz; curr_z -= dx; }
	else if (glfwGetKey(window, GLFW_KEY_W)) { curr_x -= dx; curr_z -= dz; }
	else if (glfwGetKey(window, GLFW_KEY_S)) { curr_x += dx; curr_z += dz; }

	/* Create view matrix to position and rotate world around the camera
	* Rotations and translation apply as such:
	* Camera position and orientation: View = Translation * Y axis rotation * X axis rotation
	* Actually: View = X axis rotation * Y axis rotation * Translation
	* Rotation angles and translation values are multiplied by -1 because the world has to move in the
	* opposite direction in which the camera is "moving".
	* "Engines don't move cars, they move the world around the car" principle
	*/
	View = glm::rotate(mat4(), -1 * glm::radians(pitch), vec3(1.0, 0.0, 0.0));	// Rotate by x
	View = glm::rotate(View, -1 * glm::radians(yaw), vec3(0.0, 1.0, 0.0));		// Rotate by y
	View = glm::translate(View, vec3(-curr_x, -curr_y, -curr_z));				// Translate to current values
	
	// Our ModelViewProjection : multiplication of our 3 matrices
	MVP = Projection * View * Model; // Remember, matrix multiplication is the other way around
	

	//Transform matrix to desired position
	MinimapModel = glm::translate(mat4(), vec3(minimapX, -minimapY, 0.0));
	MinimapModel = glm::scale(MinimapModel, vec3(minimapScaleX, minimapScaleY, 0.0));

	//Let's move the minimap accordingly
	MinimapModel = glm::rotate(MinimapModel, -glm::radians(yaw), vec3(0.0, 0.0, 1.0));

	// Translate to current values
	MinimapModel = glm::translate(MinimapModel, vec3(-curr_x/20, curr_z / 20, 0.0));

	oldTime = currentTime;
}

/*
	INITIALIZE MAP AND MINIMAP
*/
bool initializeMap() {
	//initialize before calling vertex buffer, init loads the objand .bmp objectsand fills the data
	map.init(mappath, textpath, sunpath);

	//Generate vertices for map
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	std::vector<GLfloat> g_vertex_buffer_data = map.getMapCoords(); //Gets primitives coordinates, UV texture coordinates and normals
	vertexbuffer_size = g_vertex_buffer_data.size() / 8;	//Divided by 8: 3 p. coords + 2 uv. coords + 3 normals

	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, g_vertex_buffer_data.size() * sizeof(GLfloat), &g_vertex_buffer_data.front(), GL_STATIC_DRAW);

	//Get a handle for the camera position
	cameraPosID = glGetUniformLocation(programID, "cameraPos");

	// Get a handle for the center of the screen's center coordinates
	scrID = glGetUniformLocation(programID, "scr_center");
	scr_center = vec2(wwidth / 2, wheight / 2);

	// Get a handle for the position of the sun (light source)
	sunPosID = glGetUniformLocation(programID, "lightPos");
	sunPosition = map.getSunPosition();
	sunDistance = (GLuint)sunPosition.y;

	return true;
}

/*
	INITIALIZE SUN
*/
bool initializeSun()
{
	//Generate vertices for sun
	glGenVertexArrays(1, &sun_VertexArrayID);
	glBindVertexArray(sun_VertexArrayID);

	std::vector<GLfloat> g_sun_vertex_buffer_data = map.getSunCoords();	//Gets only the coordinates for the sun
	sun_vertexbuffer_size = g_sun_vertex_buffer_data.size() / 3;	//3 floats per vertex

	glGenBuffers(1, &sun_vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, sun_vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, g_sun_vertex_buffer_data.size() * sizeof(GLfloat), &g_sun_vertex_buffer_data.front(), GL_STATIC_DRAW);

	// Get a handle for the sun's model transformation
	ModelSunID = glGetUniformLocation(sunShaderID, "sunPosition_modelspace");

	return true;
}


/*
	INITIALIZE MINIMAP
*/
bool initializeMinimap() {

	minimap = Minimap::Minimap(minimapPath);

	// Get a handle for vertex array
	glGenVertexArrays(1, &mm_VertexArrayID);
	glBindVertexArray(mm_VertexArrayID);

	std::vector<GLfloat> g_mm_vertex_buffer_data = minimap.getMiniMapCoords();

	mm_vertexbuffer_size = g_mm_vertex_buffer_data.size() / 6;
	
	// Bind buffers
	glGenBuffers(1, &mm_vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, mm_vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, g_mm_vertex_buffer_data.size() * sizeof(GLfloat), &g_mm_vertex_buffer_data.front(), GL_STATIC_DRAW);

	// Get the uniform location for the model matrix
	minimapMatrixID = glGetUniformLocation(minimapShaderID, "ModelMatrix");

	// Get a handle for the clipping plane
	minimapScrResID = glGetUniformLocation(minimapShaderID, "screenRes");
	scrRes.x = wwidth; scrRes.y = wheight;

	return true;
}

/*
	INITIALIZE MVP MATRIX
*/
bool initializeMVPMatrix()
{
	// Get a handle for our "MVP" uniform
	MatrixID = glGetUniformLocation(programID, "MVP"); 
	MatrixIDSunShader = glGetUniformLocation(sunShaderID, "MVP");

	// Projection matrix : 45� Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	Projection = glm::perspective(glm::radians(60.0f), 16.0f / 9.0f, 0.1f, 100.0f);

	// View Matrix
	View = glm::translate(mat4(), vec3(-curr_x, -curr_y, -curr_z));

	// Model matrix
	Model = mat4();

	// Our ModelViewProjection : multiplication of our 3 matrices
	MVP = Projection * View * Model; // Remember, matrix multiplication is the other way around

	return true;

}


/*
	INITIALIZE WINDOW
*/
bool initializeWindow()
{
	// Initialise GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		getchar();
		return false;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(1024, 720, "FPS Demo", NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		getchar();
		glfwTerminate();
		return false;
	}
	glfwMakeContextCurrent(window);

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return false;
	}

	//Obtain resolution of screen
	glfwGetWindowSize(window, &wwidth, &wheight);

	//Set cursor in the middle of the screen
	glfwSetCursorPos(window, wwidth / 2, wheight / 2);

	// Dark background
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	return true;
}

/*
	CLEANUP VERTEX BUFFERS
*/
bool cleanupVertexbuffer()
{
	// Cleanup VBO
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &colorbuffer);
	glDeleteVertexArrays(1, &VertexArrayID);
	glDeleteBuffers(1, &sun_vertexbuffer);
	glDeleteBuffers(1, &sun_VertexArrayID);

	return true;
}

/*
	CLOSE WINDOW
*/
bool closeWindow()
{
	glfwTerminate();
	return true;
}

