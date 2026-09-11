#ifndef AABB_H
#define AABB_H

#include <glm/glm.hpp>
#include <vector>
#include <algorithm>

struct AABB_Region {
	glm::vec2 min;
	glm::vec2 max;
};

AABB_Region GetAABB(const std::vector<glm::vec2>& vertices);


#endif