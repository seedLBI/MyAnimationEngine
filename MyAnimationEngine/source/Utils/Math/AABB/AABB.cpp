#include "AABB.h"

AABB_Region GetAABB(const std::vector<glm::vec2>& vertices) {
	AABB_Region output;

	output.min =  glm::vec2(100000.f, 100000.f);
	output.max = -glm::vec2(100000.f, 100000.f);

	for (size_t i = 0; i < vertices.size(); i++) {
		const glm::vec2& point_check = vertices[i];

		output.min.x = (std::min)(point_check.x, output.min.x);
		output.max.x = (std::max)(point_check.x, output.max.x);

		output.min.y = (std::min)(point_check.y, output.min.y);
		output.max.y = (std::max)(point_check.y, output.max.y);

	}

	return output;
}
