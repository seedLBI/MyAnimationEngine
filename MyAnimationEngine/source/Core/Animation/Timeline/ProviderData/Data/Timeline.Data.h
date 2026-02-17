#pragma once

#include <glm/glm.hpp>

struct Timeline{
	uint64_t framerate;
	uint64_t currentFrame;
	uint64_t totalFrames;

	long double totalTime;   // Seconds
	long double currentTime; // Seconds

	Timeline() : framerate(24), currentFrame(0), totalFrames(240), totalTime(10), currentTime(0) {}

	Timeline(const uint64_t& framerate, const uint64_t& currentFrame, const uint64_t& totalFrames) {
		this->framerate = framerate;
		this->currentFrame = currentFrame;
		this->totalFrames = totalFrames;
		totalTime = (long double)totalFrames / (long double)framerate;
		UpdateCurrentTime();
	}

	void SetTime(const uint64_t& frame) {
		currentFrame = frame > totalFrames ? totalFrames : frame;
		UpdateCurrentTime();
	}

	void SetTime(const long double& time) {
		if (time <= 0)
			currentTime = 0;
		else
			currentTime = time > totalTime ? totalTime : time;
		UpdateCurrentFrame();
	}

	inline void UpdateCurrentTime() {
		currentTime = (long double)currentFrame / (long double)framerate;
	}

	inline void UpdateCurrentFrame() {
		currentFrame = (uint64_t)(currentTime * (long double)framerate);
	}
};



