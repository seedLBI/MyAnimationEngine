#pragma once

#include <glm/glm.hpp>

struct Timeline{
	int framerate;
	int currentFrame;
	int totalFrames;

	Timeline() : framerate(24), currentFrame(0), totalFrames(240) {}
};



