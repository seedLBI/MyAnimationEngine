#ifndef CIRCLE_H
#define CIRCLE_H

#include <glm/glm.hpp>

#include "Utils/Math/AABB/AABB.h"

struct Circle {
	glm::vec2 pos;
	float radius;

	float getArea() const;
	AABB_Region getAABB() const;
};

#endif