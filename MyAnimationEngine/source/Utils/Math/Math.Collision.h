#ifndef MATH_COLLISION_H
#define MATH_COLLISION_H

#include "AABB/AABB.h"
#include "Triangle/Triangle.h"
#include "Segment/Segment.h"
#include "Circle/Circle.h"
#include "Capsule/Capsule.h"

namespace Collision {
	bool Check(const AABB_Region& A, const AABB_Region& B);
	bool Check(const Circle& A, const Circle& B);
	bool Check(const Circle& circle, const AABB_Region& region);
	bool Check(const Segment& segment, const AABB_Region& region);
	bool Check(const Capsule& capsule, const AABB_Region& region);
	bool Check(const glm::vec2& point, const AABB_Region& region);
	bool Check(const glm::vec2& point, const Triangle& triangle);
	bool Check(const glm::vec2& point, const Circle& circle);
	bool Check(const glm::vec2& point, const Capsule& capsule);
}

#endif 