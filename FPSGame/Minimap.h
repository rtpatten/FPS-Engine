#ifndef MINIMAP_h
#define MINIMAP_h

#include "Map.h"
#include <GL/glew.h>
#include <glm/glm.hpp>


class Minimap {
private:
	glm::vec4 cplane_x1;
	glm::vec4 cplane_x2;
	glm::vec4 cplane_y1;
	glm::vec4 cplane_y2;

	std::vector<vec3> miniMapCoords;
	GLfloat x_pos = (GLfloat) 0.7;
	GLfloat y_pos = -0.7;
	GLfloat edgeSize = 0.2;
	vec3 houseColor = { 0.44, 0.225, 0.03 };
	vec3 floorColor = { 0.6, 0.6, 0.6 };

public:
	Minimap() {};
	Minimap(std::string path);
	std::vector<GLfloat> getMiniMapCoords();
};

#endif