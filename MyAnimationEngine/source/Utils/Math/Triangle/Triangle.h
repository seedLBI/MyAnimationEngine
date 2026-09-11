#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <glm/glm.hpp>
#include "Utils/Math/Math.h"
#include "Utils/Math/AABB/AABB.h"

struct Triangle {
	glm::vec2 p1, p2, p3;

	float getArea() const;
	glm::vec2 getCenter() const;
	AABB_Region getAABB() const;
};


#endif
