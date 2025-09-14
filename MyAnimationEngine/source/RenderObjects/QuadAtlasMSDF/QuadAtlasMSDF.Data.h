#pragma once

#include <glm/glm.hpp>

struct QuadAtlasMSDF_Data {
	glm::vec4 atlasUV;
	float thiknessText;
	float thiknessOutline;
	glm::vec4 colorText;
	glm::vec4 colorOutline;
	glm::mat4 transform;
	glm::mat4 prev_transform;
};