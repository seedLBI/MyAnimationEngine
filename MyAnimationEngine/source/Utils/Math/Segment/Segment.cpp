#include "Segment.h"

glm::vec2 Segment::getDirection() const {
	return end - begin;
}

float Segment::getLength() const {
	return glm::length(getDirection());
}

float Segment::getAngle() const {
	glm::vec2 dir = getDirection();
	return atan2f(dir.y, dir.x);
}

AABB_Region Segment::getAABB() const {
	return GetAABB({ begin,end });
}