#include "Minimap.h"
#include "common/objloader.hpp"

Minimap::Minimap(std::string path) {
	if (!loadOBJ(path.c_str(), miniMapCoords)) throw "Error loading minimap";

}

std::vector<GLfloat> Minimap::getMiniMapCoords() {
	if (miniMapCoords.size() == 0) throw "Error retrieving Minimap coordinates";

	GLfloat highestValueX = std::numeric_limits<float>::min();
	GLfloat lowestValueX = std::numeric_limits<float>::max();
	GLfloat highestValueZ = std::numeric_limits<float>::min();
	GLfloat lowestValueZ = std::numeric_limits<float>::max();
	std::vector<GLfloat> result;
	GLfloat minmax;

	//Let's find the values of the perimeter for normalizing
	for (int i = 0; i < miniMapCoords.size(); i++) {
		//Capture range of values in x-axis, for posterior transformation
		if (miniMapCoords.at(i).x < lowestValueX) lowestValueX = miniMapCoords.at(i).x;
		if (miniMapCoords.at(i).x > highestValueX) highestValueX = miniMapCoords.at(i).x;

		//Capture range of values in y-axis, for posterior transformation
		if (miniMapCoords.at(i).z < lowestValueZ) lowestValueZ = miniMapCoords.at(i).z;
		if (miniMapCoords.at(i).z > highestValueZ) highestValueZ = miniMapCoords.at(i).z;
	}

	//Let's normalize and push back to the result, with colors included
	//Note that we transform the z-axis to the y-axis for 2D representation on the program
	for (int i = 0; i < miniMapCoords.size(); i++) {
		//Normalize x
		minmax = 2*(miniMapCoords.at(i).x - lowestValueX) / (highestValueX - lowestValueX) - 1; //Normalize to [0, 1]
		result.push_back(minmax);

		//Normalize y
		minmax = 2*(miniMapCoords.at(i).z - lowestValueZ) / (highestValueZ - lowestValueZ) - 1; //Normalize to [0, 1]
		result.push_back(minmax);

		//Push back y-axis as z-axis for keeping depth from the model
		result.push_back(miniMapCoords.at(i).y);

		//Push back color, depending on depth is either floor or house
		if (miniMapCoords.at(i).y == 0.0) {
			//It's the ground, of course
			result.push_back(floorColor.r); result.push_back(floorColor.g); result.push_back(floorColor.b);
		} else {
			result.push_back(houseColor.r); result.push_back(houseColor.g); result.push_back(houseColor.b);
		}
	}

	return result;
}