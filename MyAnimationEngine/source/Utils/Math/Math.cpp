#include "Math.h"
#include <glm/gtc/constants.hpp>
#include <iostream>

bool haveCollision(const Segment& A, const Segment& B) {
	float time1 = getTimeCollisionBetweenTwoSegment(A, B);
	float time2 = getTimeCollisionBetweenTwoSegment(B, A);
	return time1 > 0.f && time1 < 1.f && time2 > 0.f && time2 < 1.f;
}

float getTimeCollisionBetweenTwoSegment(const Segment& A, const Segment& B) {

	const float& D1x = A.begin.x;
	const float& D1y = A.begin.y;
	const float& D2x = A.end.x;
	const float& D2y = A.end.y;

	const float& D3x = B.begin.x;
	const float& D3y = B.begin.y;
	const float& D4x = B.end.x;
	const float& D4y = B.end.y;

	float numerator   = (D1x - D3x) * (D3y - D4y) - (D1y - D3y) * (D3x - D4x);
	float denumerator = (D1x - D2x) * (D3y - D4y) - (D1y - D2y) * (D3x - D4x);

	return numerator / denumerator;
}

glm::vec2 lerp(const Segment& s, const float& time) {
	return s.begin + time * (s.end - s.begin);
}

glm::vec2 lerp(const glm::vec2& A, const glm::vec2& B, const float& time) {
	return A + time * (B - A);
}

float cross2d(glm::vec2 a, glm::vec2 b) {
	return a.x * b.y - a.y * b.x;
}

float angleBetweenVectors(const glm::vec2& dir1, const glm::vec2& dir2) {
	return std::acosf(glm::dot(dir1, dir2) / (glm::length(dir1) * glm::length(dir2)));
}


float atan2(const glm::vec2& dir) {
	return std::atan2f(dir.y,dir.x);
}

glm::vec2 getDirection(const glm::vec2& A, const glm::vec2& B) {
	return B - A;
}

float getAreaParallelogram(const glm::vec2& A, const glm::vec2& B, const glm::vec2& C) {
	return cross2d(getDirection(A, B), getDirection(A, C));
}
float getAreaTriangle(const glm::vec2& A, const glm::vec2& B, const glm::vec2& C) {
	return getAreaParallelogram(A, B, C) / 2.f;
}

glm::vec2 getPointOnCircle(const float& angle) {
	return glm::vec2{ std::cosf(angle), std::sinf(angle) };
}

bool Collision_Point_and_Circle(const glm::vec2& point, const glm::vec2& posCircle, const float& radius) {
	return glm::distance(point, posCircle) < radius;
}

bool isIntersectPointPolygon(const glm::vec2& point, const std::vector<glm::vec2>& polygon) {
	if (polygon.size() < 3) return false;

	bool inside = false;
	size_t n = polygon.size();
	float x = point.x;
	float y = point.y;

	for (size_t i = 0, j = n - 1; i < n; j = i++) {
		const glm::vec2& a = polygon[i];
		const glm::vec2& b = polygon[j];

		if ((a.y > y) != (b.y > y) && 
			(x < a.x + (b.x - a.x) * (y - a.y) / (b.y - a.y))) {
			inside = !inside;
		}
	}
	return inside;
}


glm::vec2 closestPointOnSegment(const glm::vec2& A, const glm::vec2& B, const glm::vec2& P) {
	glm::vec2 AB = B - A;
	float lenSquared = glm::dot(AB, AB);
	if (lenSquared < 1e-6f) {
		return A;
	}
	glm::vec2 AP = P - A;
	float t = glm::dot(AP, AB) / lenSquared;
	t = glm::clamp(t, 0.0f, 1.0f);
	return A + t * AB;
}

glm::vec2 findClosestPointOnPolygon(const std::vector<glm::vec2>& polygon, const glm::vec2& P, glm::vec2& OutNormal) {
	if (polygon.size() < 3) {
		return glm::vec2(0.0f);
	}

	glm::vec2 closestPoint = P;
	float minDistance = std::numeric_limits<float>::max();

	for (size_t i = 0; i < polygon.size(); ++i) {
		size_t j = (i + 1) % polygon.size();
		glm::vec2 candidate = closestPointOnSegment(polygon[i], polygon[j], P);
		float distance = glm::length(candidate - P);

		if (distance < minDistance) {
			minDistance = distance;
			closestPoint = candidate;
			OutNormal = perp_normalized(polygon[j] - polygon[i]);
		}
	}

	return closestPoint;
}

std::optional<glm::vec2> findClosestIntersection_Segment_and_Polygon(const std::vector<glm::vec2>& polygon, const Segment& segment, glm::vec2& OutNormal, glm::vec2& OutDirection) {

	std::vector<std::pair<size_t, glm::vec2>> collisions;

	for (size_t i = 0; i < polygon.size(); i++) {
		Segment border{ polygon[i], polygon[(i + 1) % polygon.size()] };

		float time = getTimeCollisionBetweenTwoSegment(border, segment);
		float time2 = getTimeCollisionBetweenTwoSegment(segment, border);

		const float EPSILON = 1e-5f;

		if (time > 1.f + EPSILON || time < 0.f - EPSILON ||
			time2 > 1.f + EPSILON || time2 < 0.f - EPSILON)
			continue;

		glm::vec2 point = lerp(border, time);


		bool duplicate = false;
		for (const auto& existing : collisions) {
			if (glm::length(point - existing.second) < EPSILON) {
				duplicate = true;
				break;
			}
		}
		if (!duplicate) {
			collisions.push_back({ i, point });
		}
	}

	if (collisions.empty())
		return std::nullopt;

	std::sort(collisions.begin(), collisions.end(), [&](const auto& a, const auto& b) {
		return glm::length(a.second - segment.begin) > glm::length(b.second - segment.begin);
		});

	size_t& index_segment = collisions.back().first;

	OutDirection = glm::normalize(getDirection(polygon[index_segment], polygon[(index_segment + 1) % polygon.size()]));
	OutNormal = perp_normalized(OutDirection);

	return collisions.back().second;
}


glm::vec2 rotate(const glm::vec2& dir1, const float& angle) {
	const glm::mat2 rot_mat = {
		{cosf(angle),-sinf(angle)},
		{sinf(angle), cosf(angle)}
	};
	return rot_mat * dir1;
}

glm::vec2 perp_normalized(const glm::vec2& p) {
	return glm::normalize(glm::vec2{ -p.y,p.x });
}

bool isClockwise(const std::vector<glm::vec2>& polygon, bool yAxisUp ) {
	if (polygon.size() < 3) return false;

	float sum = 0.0f;
	size_t n = polygon.size();
	size_t vertexCount = polygon[0] == polygon[n - 1] ? n - 1 : n;

	for (size_t i = 0; i < vertexCount; i++) {
		size_t j = (i + 1) % vertexCount;
		sum += (polygon[j].x - polygon[i].x) *
			(polygon[j].y + polygon[i].y);
	}

	if (yAxisUp) {
		return sum > 0.0f;
	}
	else {
		return sum < 0.0f;
	}
}


std::vector<glm::vec2> GenerateRadiusBorder(const std::vector<glm::vec2>& original_border, const float& radius, const bool& outer_border) {

	if (original_border.size() <= 1)
		return {};

	std::vector<glm::vec2> output;

	const float MaxSampleRateCircle = 10;

	const std::vector<glm::vec2>& vertices = original_border;

	for (size_t i = 0; i < vertices.size(); i++) {

		Segment A, B;

		size_t prev_index = i == 0 ? vertices.size() - 1 : i - 1;
		size_t curr_index = i;
		size_t next_index = (i + 1) % vertices.size();

		A = { vertices[prev_index], vertices[curr_index] };
		B = { vertices[curr_index], vertices[next_index] };

		glm::vec2 dirA = A.begin - A.end;
		glm::vec2 dirB = B.begin - B.end;

		glm::vec2 perpA = perp_normalized(dirA) * radius;
		glm::vec2 perpB = perp_normalized(dirB) * radius;

		float sign_radius = outer_border == true ? -1.f : 1.f;

		perpA *= sign_radius;
		perpB *= sign_radius;

		Segment A_radius{ A.begin + perpA, A.end + perpA };
		Segment B_radius{ B.begin + perpB, B.end + perpB };

		glm::vec2 dirA_radius = A_radius.getDirection();
		glm::vec2 dirB_radius = B_radius.getDirection();


		float cross_dir_radius = cross2d(dirA_radius, dirB_radius) * (-1.f) * sign_radius;

		bool circle_moment = cross_dir_radius > 0 ? false : true;

		if (circle_moment) {
			float sign = cross2d(dirA, dirB) > 0.f ? 1.f : 3.f;

			const float& PI = glm::pi<float>();

			float alphaA = atan2(dirA);
			float alphaB = atan2(dirB);

			float angleA = alphaA + PI / 2.f * sign;
			float angleB = alphaB + PI / 2.f * sign;

			float diff = angleB - angleA;

			if (abs(diff) <= 0.05f) {
				output.push_back(A_radius.end);

				continue;
			}


			diff = atan2({ cosf(diff), sinf(diff) });

			int count_samples = (std::max)(2.f, std::floor((std::abs(diff) / PI) * MaxSampleRateCircle));

			for (size_t sample = 0; sample <= count_samples; sample++) {

				float time = (float)sample / (float)count_samples;
				float angle_interpolated = angleA + time * diff;

				glm::vec2 point_circle = A.end + getPointOnCircle(angle_interpolated) * radius;

				output.push_back(point_circle);
			}


		}
		else {
			float t = getTimeCollisionBetweenTwoSegment(A_radius, B_radius);

			glm::vec2 point = lerp(A_radius, t);

			output.push_back(point);

		}

	}

	return output;
}


