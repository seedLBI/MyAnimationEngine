#include "Triangulation.EarClipping.h"
#include "Utils/Math/Math.Collision.h"
#include <numeric>

std::vector<Triangle> MakeTriangulationEarClipping(
	const std::vector<glm::vec2>& polygon) {

	std::vector<Triangle> triangles;

	bool flag_isCloakwise = isClockwise(polygon, true);
	float correction_for_cross = std::powf(-1, 1 - flag_isCloakwise);

	std::vector<size_t> indices(polygon.size());
	std::iota(indices.begin(), indices.end(), 0);
	
	auto isEar = [&](const glm::vec2& prev, const glm::vec2& current, const glm::vec2& next) -> bool {

		glm::vec2 p1 = getDirection(prev, current);
		glm::vec2 p2 = getDirection(current, next);

		float cross = cross2d(p1, p2) * correction_for_cross;
		bool isConvex = cross > 0 ? false: true;
		if (isConvex == false) return false;


		for (const auto& vertex : polygon) {

			if (prev == vertex || current == vertex || next == vertex)
				continue;

			if (Collision::Check(vertex, Triangle{ prev, current, next }))
				return false;

		}

		return true;
	};


	while (indices.size() > 3) {

		bool earFound = false;

		for (size_t i = 0; i < indices.size(); i++) {
			size_t prevIndex = (i == 0) ? indices[indices.size() - 1]  : indices[i-1];
			size_t currIndex = indices[i];
			size_t nextIndex = indices[(i + 1) % indices.size()];

			const glm::vec2& prev    = polygon[prevIndex];
			const glm::vec2& current = polygon[currIndex];
			const glm::vec2& next    = polygon[nextIndex];

			if (isEar(prev,current,next)) {

				triangles.push_back({ prev,current,next });

				indices.erase(indices.begin() + i);

				earFound = true;

				break;

			}

		}
		if (!earFound) {
			return {};
		}

	}


	if (indices.size() == 3) {
		triangles.push_back({
			polygon[indices[0]],
			polygon[indices[1]],
			polygon[indices[2]]
			});
	}



	return triangles;
}