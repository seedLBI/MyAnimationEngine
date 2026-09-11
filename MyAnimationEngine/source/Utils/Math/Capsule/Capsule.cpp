#include "Capsule.h"

#include <glm/gtc/constants.hpp>

float Capsule::getArea() const {
	return this->segment.getLength() * this->radius;
}

AABB_Region Capsule::getAABB(bool crop) const {
	if (crop) {
		float angle = segment.getAngle();
		static const float pi_half = glm::pi<float>() / 2.f;

		glm::vec2 cl = glm::vec2{ cosf(angle - pi_half), sin(angle - pi_half) } * radius;
		glm::vec2 cr = glm::vec2{ cosf(angle + pi_half), sin(angle + pi_half) } * radius;

		std::vector<glm::vec2> points_check(4);

		points_check[0] = segment.begin + cl;
		points_check[1] = segment.begin + cr;

		points_check[2] = segment.end + cl;
		points_check[3] = segment.end + cr;


		return GetAABB(points_check);
	}
	else {
		std::vector<glm::vec2> points_check(8);

		const glm::vec2 left_check = { -1.f * radius ,0.f };
		const glm::vec2 right_check = { 1.f * radius ,0.f };
		const glm::vec2 up_check = { 0.f , 1.f * radius };
		const glm::vec2 down_check = { 0.f ,-1.f * radius };

		points_check[0] = segment.begin + left_check;
		points_check[1] = segment.begin + right_check;
		points_check[2] = segment.begin + up_check;
		points_check[3] = segment.begin + down_check;

		points_check[4] = segment.end + left_check;
		points_check[5] = segment.end + right_check;
		points_check[6] = segment.end + up_check;
		points_check[7] = segment.end + down_check;

		return GetAABB(points_check);
	}
}