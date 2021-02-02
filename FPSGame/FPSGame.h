#ifndef FPSGAME_H
#define FPSGAME_H

#include "Map.h"
#include "Minimap.h"
// Include GLEW
#include <GL/glew.h>
// Include GLM
#include <glm/glm.hpp>

//Map variable;
//Initialize map
std::string mappath = "textures/mapv1.obj";
std::string textpath = "textures/texture.bmp";
std::string sunpath = "textures/sun.obj";
Map map;
Minimap minimap;

//Global variables for handling the map's buffers
GLuint vertexbuffer;
GLuint VertexArrayID;
GLuint vertexbuffer_size;
GLuint colorbuffer;

//Global variables for handling the sun around the map's buffers
GLuint sun_vertexbuffer;
GLuint sun_VertexArrayID;
GLuint sun_vertexbuffer_size;
GLuint sun_colorID;
glm::mat4 SunModel;
GLuint ModelSunID;

//global variables to handle the MVP matrix
GLuint MatrixID;
GLuint MatrixIDSunShader;
glm::mat4 MVP;
glm::mat4 Model;
glm::mat4 View;
glm::mat4 Projection;

//Camera position
GLuint cameraPosID;

//Sun position
GLuint sunPosID;
vec3 sunPosition;
GLuint sunDistance;

//Variables to recreate movement
float yaw = 0.0f;  // horizontal angle : toward -Z
float pitch = 0.0f; // vertical angle : 0, look at the horizon

GLfloat curr_x = 0;
GLfloat curr_y = 1.7;
GLfloat curr_z = 0;
float speed = 4.0f; // 3 units / second
float mouseSpeed = 4.0f;
double xpos, ypos;  //Mouse coordinates
int wheight = 0;
int wwidth = 0;
double oldTime;

//program ID of the shaders, required for handling the shaders with OpenGL
GLuint programID;

//ID of the shaders for the sun
GLuint sunShaderID;

//ID of the shaders for the minimap
GLuint minimapShaderID;

// Variables for the program shaders and handling the flashlight
GLuint scrID;
glm::vec2 scr_center;

// Variables for the minimap shaders
std::string minimapPath = "textures/map2d.obj";
GLuint mm_vertexbuffer;
GLuint mm_VertexArrayID;
GLuint mm_vertexbuffer_size;
glm::mat4 MinimapModel;
GLuint minimapMatrixID;
float minimapX = 0.6;
float minimapY = 0.5;
float minimapScaleX = 0.3;
float minimapScaleY = 0.37;
vec2 scrRes;
GLuint minimapScrResID;

int main( void ); //<<< main function, called at startup
void updateAnimationLoop(); //<<< updates the animation loop
void updateMVPLoop(); //<<< Updates the position of the vertices depending on key presses
void updateMap();
void updateSun();
void updateMinimap();
bool initializeMap(); //<<< initializes the vertex buffer array and binds it OpenGL for the map
bool initializeSun(); //<<< initializes the vertex buffer array and binds it OpenGL for the sun
bool initializeMinimap(); //<<< initializes the vertex buffer array and binds it OpenGL for the minimap
bool initializeMVPMatrix(); //<<< Generates the model-view-perspective matrix
bool initializeWindow(); //<<< initializes the window using GLFW and GLEW
bool cleanupVertexbuffer(); //<<< frees all recources from the vertex buffer
bool closeWindow(); //<<< Closes the OpenGL window and terminates GLFW

#endif