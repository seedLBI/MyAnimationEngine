#pragma once

#include <glm/glm.hpp>

struct Timeline{
private:
	long double lengthTimeFrame;
public:
	uint64_t framerate;
	uint64_t currentFrame;
	uint64_t totalFrames;

	long double totalTime;   // Seconds
	long double currentTime; // Seconds

	long double left_range_repeat;
	long double right_range_repeat;


	Timeline() : 
		framerate(24), 
		currentFrame(0), 
		totalTime(50), 
		currentTime(0),
		left_range_repeat(2.5l),
		right_range_repeat(7.5l)
	{
		lengthTimeFrame = 1.0 / (double)framerate;
		totalFrames = framerate * totalTime;
	}

	Timeline(const uint64_t& framerate, const uint64_t& currentFrame, const uint64_t& totalFrames) {
		this->framerate = framerate;
		this->currentFrame = currentFrame;
		this->totalFrames = totalFrames;
		totalTime = (long double)totalFrames / (long double)framerate;
		UpdateCurrentTime();

		lengthTimeFrame = 1.0 / (double)framerate;
	}

	const long double &getLengthTimeFrame() {
		return lengthTimeFrame;
	}


	void SetTime(const uint64_t& frame) {
		currentFrame = frame > totalFrames ? totalFrames : frame;
		UpdateCurrentTime();
	}


	void SetLeftRange(const long double& time) {

		left_range_repeat = time;

		if (left_range_repeat < 0.f)
			left_range_repeat = 0.f;

		if (left_range_repeat > right_range_repeat)
			left_range_repeat = right_range_repeat - 0.1f;

		if (left_range_repeat > totalTime)
			left_range_repeat = totalTime;

	}

	void SetRightRange(const long double& time) {

		right_range_repeat = time;

		if (right_range_repeat > totalTime)
			right_range_repeat = totalTime;

		if (right_range_repeat < left_range_repeat)
			right_range_repeat = left_range_repeat + 0.1f;

		if (right_range_repeat < 0.f)
			right_range_repeat = 0.f;

	}



	void SetTime(const long double& time, bool withRepeatRanges = false ) {
		if (withRepeatRanges) {

			currentTime = time;

			if (currentTime < left_range_repeat)
				currentTime = left_range_repeat;
			if (currentTime > right_range_repeat)
				currentTime = right_range_repeat - 0.0001l;

		}
		else {
			if (time <= 0) currentTime = 0;
			else currentTime = time > totalTime ? totalTime : time;
		}
		UpdateCurrentFrame();
	}

	inline void UpdateCurrentTime() {
		currentTime = (long double)currentFrame / (long double)framerate;
	}

	inline void UpdateCurrentFrame() {
		currentFrame = (uint64_t)(currentTime * (long double)framerate);
	}
};



