#pragma once

#include <glm/glm.hpp>

#include "Utils/OpenGL/Texture/Texture.h"


struct Keyframe {
	double time;
	double value;
	glm::vec2 in;
	glm::vec2 out;
};

template <typename T>
struct Keyframes {
	std::vector<Keyframe> keys;
	std::vector<glm::vec2> samples;

	void discretize() {

	}

private:

};


struct Component_2DTransform {
	glm::vec2 position;
	glm::vec2 size;
	glm::vec2 anchor;
	float rotation;
};

struct Component_2DTransform_Animation {
	Keyframes<glm::vec2> position;
	Keyframes<glm::vec2> size;
	Keyframes<glm::vec2> anchor;
	Keyframes<float> angle;
};

struct Component_MSDFQuad {
	Texture texture;
	float thiknessBase;
	float thiknessOutline;
	glm::vec4 colorBase;
	glm::vec4 colorOutline;
};

struct Component_Layer {
	double timeBegin;
	double timeEnd;
	bool Visible;
	uint64_t Index;
};


