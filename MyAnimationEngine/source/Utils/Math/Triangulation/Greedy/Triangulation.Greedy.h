#ifndef TRINGULATION_GREEDY_H
#define TRINGULATION_GREEDY_H

#include <glm/glm.hpp>
#include <vector>
#include "Utils/Math/Triangle/Triangle.h"

std::vector<Triangle> MakeTriangulationGreedy(
	std::vector<glm::vec2>& polygon);


#endif