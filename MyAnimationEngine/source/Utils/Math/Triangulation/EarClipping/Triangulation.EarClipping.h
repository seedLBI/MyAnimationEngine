#ifndef TRIANGULATION_EAR_CLIPPING_H
#define TRIANGULATION_EAR_CLIPPING_H

#include <glm/glm.hpp>
#include <vector>
#include "Utils/Math/Triangle/Triangle.h"

std::vector<Triangle> MakeTriangulationEarClipping(
	const std::vector<glm::vec2>& polygon);


#endif