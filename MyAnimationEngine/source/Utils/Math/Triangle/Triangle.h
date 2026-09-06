#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <glm/glm.hpp>
#include "Utils/Math/Math.h"

struct Triangle {
	glm::vec2 p1, p2, p3;

	float getArea() const;
	glm::vec2 getCenter();
};

bool Collision_Point_and_Triangle(const glm::vec2& point, const Triangle& triangle);

#endif
