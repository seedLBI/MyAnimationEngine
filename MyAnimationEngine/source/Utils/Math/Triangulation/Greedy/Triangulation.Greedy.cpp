#include "Triangulation.Greedy.h"


#include "Utils/Math/Math.h"
#include "Utils/Math/Segment/Segment.h"

#include <set>
#include <iostream>
#include <algorithm>

std::vector<Triangle> MakeTriangulationGreedy(std::vector<glm::vec2>& polygon) {
	std::vector<std::pair<size_t, size_t>> indexes_connected;

	for (size_t i = 0; i < polygon.size(); i++) {

		const glm::vec2& i_p = polygon[i];

		std::vector<std::pair<size_t, float>> length_segment;
		length_segment.reserve(polygon.size() - 1);

		// calc all length
		for (size_t j = 0; j < polygon.size(); j++) {
			if (i == j) 
				continue;
			if (j == ((i + 1) % polygon.size()) || j == ((i - 1) % polygon.size()))
				continue;

			bool correctIndexes = true;
			for (size_t check = 0; check < indexes_connected.size(); check++) {
				size_t& i_check = indexes_connected[check].first;
				size_t& j_check = indexes_connected[check].second;

				if ((i_check == i && j_check == j) ||
					(i_check == j && j_check == i)) {
					correctIndexes = false;
					break;
				}
			}
			if (correctIndexes == false)
				continue;

			const int count_check = 50;

			for (size_t k = 1; k < count_check - 1; k++) {
				float step = 1.f / (float)count_check;

				float time = step * k;

				glm::vec2 point_check = lerp(polygon[i], polygon[j], time);

				if (!isIntersectPointPolygon(point_check,polygon))
				{
					correctIndexes = false;
					break;

				}


			}
			if (correctIndexes == false)
				continue;

			const glm::vec2& j_p = polygon[j];
			float len = glm::length(i_p - j_p);
			length_segment.push_back({ j, len });
		}
		std::sort(
			length_segment.begin(), 
			length_segment.end(), 
			[](const auto& a, const auto& b) {
				return a.second < b.second;
			}
		);

		// check candidate
		for (size_t j = 0; j < length_segment.size(); j++) {
			auto& candidate = length_segment[j];

			if (indexes_connected.empty()) {
				indexes_connected.push_back({ i, candidate.first });
				break;
			}
			else {

				bool inserted = true;

				for (size_t k = 0; k < indexes_connected.size(); k++) {

					const glm::vec2& connected_begin = polygon[indexes_connected[k].first];
					const glm::vec2& connected_end = polygon[indexes_connected[k].second];

					const glm::vec2& candidate_begin = polygon[i];
					const glm::vec2& candidate_end = polygon[candidate.first];

					Segment s_connected{ connected_begin, connected_end };
					Segment s_candidate{ candidate_begin, candidate_end };

					if (haveCollision(s_connected, s_candidate)) {
						inserted = false;
						break;
					}
				}
				if (inserted){
					indexes_connected.push_back({ i, candidate.first });
					
				}
	
			}

		}

	}



	for (size_t i = 0; i < polygon.size(); i++) {
		indexes_connected.push_back({ i % polygon.size(),(i + 1)% polygon.size() });
	}

	std::vector<std::set<size_t>> setsIndexedTrigs;
	for (size_t i = 0; i < polygon.size(); i++) {


		for (size_t j = 0; j < polygon.size(); j++) {
			if (i == j) continue;

			bool have_path_to_first = false;
			for (size_t check = 0; check < indexes_connected.size(); check++) {
				const size_t& i_check = indexes_connected[check].first;
				const size_t& j_check = indexes_connected[check].second;
				if (
					(i_check == i && j_check == j) || 
					(i_check == j && j_check == i)) {
					have_path_to_first = true;
					break;
				}
			}
			if (!have_path_to_first) {
				continue;
			}



			for (size_t k = 0; k < polygon.size(); k++) {
				if (k == i || k == j) continue;

				have_path_to_first = false;
				for (size_t check = 0; check < indexes_connected.size(); check++) {
					const size_t& i_check = indexes_connected[check].first;
					const size_t& j_check = indexes_connected[check].second;
					if ((i_check == i && j_check == k) || 
						(i_check == k && j_check == i)) {
						have_path_to_first = true;
						break;
					}
				}
				if (!have_path_to_first) {
					continue;
				}

				have_path_to_first = false;
				for (size_t check = 0; check < indexes_connected.size(); check++) {
					const size_t& i_check = indexes_connected[check].first;
					const size_t& j_check = indexes_connected[check].second;
					if ((i_check == j && j_check == k) || 
						(i_check == k && j_check == j)) {
						have_path_to_first = true;
						break;
					}
				}
				if (!have_path_to_first) {
					continue;
				}


				std::set<size_t> temp = { i,j,k };

				for (size_t q = 0; q < setsIndexedTrigs.size(); q++) {
					if (temp == setsIndexedTrigs[q]) {
						have_path_to_first = false;
						break;
					}
				}

				if (have_path_to_first) {
					setsIndexedTrigs.emplace_back(temp);
					break;
				}
			}
		}
	}

	std::vector<Triangle> trigs(setsIndexedTrigs.size());
	
	for (size_t i = 0; i < setsIndexedTrigs.size(); i++) {
		glm::vec2 vertexs[3] = {glm::vec2(0,0), glm::vec2(0,0), glm::vec2(0,0)};
		int index = 0;
		for (const size_t& indexVertexPolugon : setsIndexedTrigs[i]) {
			vertexs[index] = polygon[indexVertexPolugon];
			index++;
		}
		trigs[i] = { vertexs[0],vertexs[1],vertexs[2] };
	}

	return trigs;
}
