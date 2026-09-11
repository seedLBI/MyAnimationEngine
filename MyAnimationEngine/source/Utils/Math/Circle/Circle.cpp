#include "Circle.h"

#include <glm/gtc/constants.hpp>

float Circle::getArea() const {
	return glm::pi<float>() *radius * radius;
}

AABB_Region Circle::getAABB() const {
	return {pos - glm::vec2{radius}, pos + glm::vec2{radius}};
}