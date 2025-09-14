#pragma once

#include <glm/glm.hpp>
#include <string>

#include "Utils/OpenGL/Texture/Texture.h"
#include "Core/Animation/Keyframe/Keyframe.h"


struct Component {};

struct Component_2DTransform : Component {
	glm::vec2 position;
	glm::vec2 size;
	glm::vec2 anchor;
	float	  angle;

	std::pair<Keyframes, Keyframes> anim_position;
	std::pair<Keyframes, Keyframes> anim_size;
	std::pair<Keyframes, Keyframes> anim_anchor;
	Keyframes						anim_angle;
};

struct Component_MSDFQuad : Component {
	Texture texture;
};


// Colors
struct Component_HUE : Component {
	float value;
	Keyframes anim_value;
};

struct Component_Color : Component {
	glm::vec4 color;

	Keyframes anim_r;
	Keyframes anim_g;
	Keyframes anim_b;
	Keyframes anim_a;
};

struct Component_ColorOutline : Component {
	glm::vec4 color;

	Keyframes anim_r;
	Keyframes anim_g;
	Keyframes anim_b;
	Keyframes anim_a;
};


struct Component_Thikness : Component {
	float value;
	Keyframes anim_value;
};

struct Component_ThiknessOutline : Component {
	float value;
	Keyframes anim_value;
};

struct TimeSector {
	long double Begin;
	long double End;
	void Shift(const long double& time) { Begin += time; End += time; }
};

struct LayerInfo {
	std::string name;
	TimeSector time;
	bool isVisible;
};


