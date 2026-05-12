#pragma once

#include <glm/glm.hpp>
#include <vector>


struct Keyframe {
	glm::vec2 in;

	glm::vec2 pos;

	glm::vec2 out;
};

struct Keyframes {
	std::vector<Keyframe> keys;
	std::vector<glm::vec2> samples;

	int min_sampleRate = 2;
	int max_sampleRate = 30;

	void discretize();
	void discretize_optimized();
	float evaluate(const float& time);

	void addKeyframe(const Keyframe& k);

	void addPoint(const glm::vec2& p);

	float getBeginTime();
	float getEndTime();

private:
	glm::vec2 lerp(const Keyframe& k1, const Keyframe& k2, const float& t);

	float calculateCurvature(const Keyframe& k1, const Keyframe& k2, float t);
	int estimateSampleCount(const Keyframe& k1, const Keyframe& k2, const float& maxCurvatureThreshold);
};