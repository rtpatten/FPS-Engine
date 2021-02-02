#ifndef MAP_H
#define MAP_H

#include <iostream>
#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>
using namespace glm;

class Map {
private:

	std::vector<glm::vec3> map_vertices;
	std::vector<glm::vec2> map_uv_vertices;
	std::vector<glm::vec3> map_normals_vertices;
	GLuint textureID = -1;
	std::vector<glm::vec3> sun_vertices;
	glm::vec3 sun_position = vec3(15.0, 15.0, 15.0);

public:
	Map() {};
	void init(std::string path, std::string texture, std::string sunpath);
	std::vector<GLfloat> getMapCoords();
	std::vector<GLfloat> getSunCoords();
	GLuint getTextureID();
	GLuint getMapCoordsVecSize();
	GLuint getSunCoordsVecSize();
	vec3 getSunPosition();
};

#endif