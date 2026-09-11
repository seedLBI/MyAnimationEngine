#include "Triangle.h"


float Triangle::getArea() const {
	return getAreaTriangle(p1, p2, p3);
}

glm::vec2 Triangle::getCenter() const{
	return (p1 + p2 + p3) / 3.f;
}

AABB_Region Triangle::getAABB() const {
	return GetAABB({ p1,p2,p3 });
}