#include "Math.Collision.h"


bool Collision::Check(const AABB_Region& A, const AABB_Region& B) {
	return (A.min.x <= B.max.x) && (A.max.x >= B.min.x) &&
		(A.min.y <= B.max.y) && (A.max.y >= B.min.y);
}
bool Collision::Check(const Circle& circle, const AABB_Region& region) {
	const float x = (std::max)(region.min.x, (std::min)(circle.pos.x, region.max.x));
	const float y = (std::max)(region.min.y, (std::min)(circle.pos.y, region.max.y));

	glm::vec2 pos_check(x, y);

	return glm::length(circle.pos - pos_check) < circle.radius;
}
bool Collision::Check(const Circle& A, const Circle& B) {
	return glm::length(A.pos - B.pos) - A.radius - B.radius <= 0.f;
}
bool Collision::Check(const Segment& segment, const AABB_Region& region) {
	AABB_Region region_from_segment = GetAABB({ segment.begin, segment.end });
	return Check(region_from_segment, region);
}
bool Collision::Check(const Capsule& capsule, const AABB_Region& region) {
	return Check(capsule.getAABB(false), region);
}
bool Collision::Check(const glm::vec2& point, const AABB_Region& region) {
	return point.x >= region.min.x && point.x <= region.max.x &&
		point.y >= region.min.y && point.y <= region.max.y;
}
bool Collision::Check(const glm::vec2& point, const Triangle& triangle) {
	float area = triangle.getArea();

	if (area == 0.f) return false;

	area = 1.f / area;

	float u = getAreaTriangle(triangle.p2, triangle.p3, point) * area;
	float v = getAreaTriangle(triangle.p3, triangle.p1, point) * area;
	float w = getAreaTriangle(triangle.p1, triangle.p2, point) * area;

	bool inTriangle = u >= 0.f && v >= 0.f && w >= 0.f;

	return inTriangle;
}
bool Collision::Check(const glm::vec2& point, const Circle& circle) {
	return glm::length(circle.pos - point) <= circle.radius;
}
bool Collision::Check(const glm::vec2& point, const Capsule& capsule) {

	glm::vec2 dir = capsule.segment.getDirection();
	float len = glm::length(dir);
	float dot = glm::dot(point, dir);
	float t = std::clamp(dot / (len * len),0.f,1.f);

	glm::vec2 proj = capsule.segment.begin + t * dir;

	float len2point = glm::length(proj - point);

	return len2point <= capsule.radius;
}
