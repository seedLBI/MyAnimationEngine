#ifndef ARKANOID_MATH_H
#define ARKANOID_MATH_H


#include <glm/glm.hpp>
#include <vector>
#include <optional>
#include <algorithm>

#include "Segment/Segment.h"

bool haveCollision(const Segment& A, const Segment& B);
float getTimeCollisionBetweenTwoSegment(const Segment& A, const Segment& B);
glm::vec2 lerp(const Segment& s, const float& time);
glm::vec2 lerp(const glm::vec2& A, const glm::vec2& B, const float& time);

float cross2d(glm::vec2 a, glm::vec2 b);
glm::vec2 rotate(const glm::vec2& dir1, const float& angle);
glm::vec2 perp_normalized(const glm::vec2& p);
float angleBetweenVectors(const glm::vec2& dir1, const glm::vec2& dir2);
float atan2(const glm::vec2& dir);

glm::vec2 getPointOnCircle(const float& angle);

bool Collision_Point_and_Circle(const glm::vec2& point,const glm::vec2& posCircle, const float& radius);
bool isIntersectPointPolygon(const glm::vec2& point, const std::vector<glm::vec2>& polygon);

glm::vec2 closestPointOnSegment(const glm::vec2& A, const glm::vec2& B, const glm::vec2& P);
glm::vec2 findClosestPointOnPolygon(const std::vector<glm::vec2>& polygon, const glm::vec2& P, glm::vec2& OutNormal);

std::optional<glm::vec2> findClosestIntersection_Segment_and_Polygon(const std::vector<glm::vec2>& polygon, const Segment& segment, glm::vec2& OutNormal, glm::vec2& OutDirection);


glm::vec2 getDirection(const glm::vec2& A, const glm::vec2& B);

float getAreaParallelogram(const glm::vec2& A, const glm::vec2& B, const glm::vec2& C);
float getAreaTriangle(const glm::vec2& A, const glm::vec2& B, const glm::vec2& C);

bool isClockwise(const std::vector<glm::vec2>& polygon, bool yAxisUp = true);

std::vector<glm::vec2> GenerateRadiusBorder(const std::vector<glm::vec2>& original_border, const float& radius, const bool& outer_border);


#endif