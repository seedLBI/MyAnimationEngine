#include "Triangle.h"


float Triangle::getArea() const {
	return getAreaTriangle(p1, p2, p3);
}

glm::vec2 Triangle::getCenter() {
	return (p1 + p2 + p3) / 3.f;
}

bool Collision_Point_and_Triangle(const glm::vec2& point, const Triangle& triangle) {

	float area = triangle.getArea();

	if (area == 0.f) return false;

	area = 1.f / area;

	float u = getAreaTriangle(triangle.p2, triangle.p3, point) * area;
	float v = getAreaTriangle(triangle.p3, triangle.p1, point) * area;
	float w = getAreaTriangle(triangle.p1, triangle.p2, point) * area;

	bool inTriangle = u >= 0.f && v >= 0.f && w >= 0.f;

	return inTriangle;
}