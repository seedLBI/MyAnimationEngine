#ifndef CAPSULE_H
#define CAPSULE_H

#include <glm/glm.hpp>
#include <Utils/Math/Segment/Segment.h>
#include "Utils/Math/AABB/AABB.h"

struct Capsule {
	Segment segment;
	float radius;

	float getArea() const;
	AABB_Region getAABB(bool crop) const;

};

#endif