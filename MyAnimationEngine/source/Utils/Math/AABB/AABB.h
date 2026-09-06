#ifndef AABB_H
#define AABB_H

#include "Utils/Math/Math.h"
#include "Utils/Math/Segment/Segment.h"
#include <glm/glm.hpp>
#include <vector>
#include <algorithm>

struct AABB_Region {
	glm::vec2 min;
	glm::vec2 max;
};

AABB_Region GetAABB(const std::vector<glm::vec2>& vertices);

bool Collision_Point_and_AABB(const glm::vec2& point, const AABB_Region& region);
bool Collision_Circle_and_AABB(const glm::vec2& circle_pos, const float& radius, const AABB_Region& region);
bool Collision_AABB_and_AABB(const AABB_Region& A, const AABB_Region& B);
bool Collision_Segment_and_AABB(const Segment& segment, const AABB_Region& region);
bool Collision_Segment_and_AABB(const Segment& segment, const float& global_radius, const AABB_Region& region);

#endif