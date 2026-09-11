#ifndef SEGMENT_H
#define SEGMENT_H

#include <glm/glm.hpp>

#include "Utils/Math/AABB/AABB.h"

struct Segment {
	glm::vec2 begin;
	glm::vec2 end;

	glm::vec2 getDirection() const;
	float getAngle() const;
	float getLength() const;
	AABB_Region getAABB() const;

};




#endif