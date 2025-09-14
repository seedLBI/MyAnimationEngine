#include "Keyframe.h"



void Keyframes::discretize() {
	samples.clear();

	for (size_t i = 1; i < keys.size(); i++) {

		size_t first_step = i == 1 ? 0 : 1;
		const Keyframe k1 = keys[i - 1];
		const Keyframe k2 = keys[i];

		for (size_t j = first_step; j <= max_sampleRate; j++) {

			const float time = float(j) / (float)max_sampleRate;


			const glm::vec2 sample = lerp(k1, k2, time);

			samples.emplace_back(sample);

		}
	}
}

void Keyframes::discretize_optimized() {

	samples.clear();

	// Шаг 1: Вычисляем среднюю нормализованную кривизну по всей кривой
	float totalCurvature = 0.0f;
	int totalPoints = 0;
	for (size_t i = 1; i < keys.size(); ++i) {
		const int testPoints = 5;
		for (int j = 0; j < testPoints; ++j) {
			float t = j / (float)(testPoints - 1);
			float curvature = calculateCurvature(keys[i - 1], keys[i], t);
			totalCurvature += curvature;
			totalPoints++;
		}
	}
	float avgCurvature = totalPoints > 0 ? totalCurvature / totalPoints : 0.0f;

	float maxCurvatureThreshold = std::max(avgCurvature * 4.0f, 0.0001f);

	for (size_t i = 1; i < keys.size(); i++) {

		size_t first_step = i == 1 ? 0 : 1;
		const Keyframe k1 = keys[i - 1];
		const Keyframe k2 = keys[i];

		size_t optimal_sampleRate = estimateSampleCount(k1, k2, maxCurvatureThreshold);

		for (size_t j = first_step; j <= optimal_sampleRate; j++) {

			const float time = float(j) / (float)optimal_sampleRate;

			const glm::vec2 sample = lerp(k1, k2, time);

			samples.emplace_back(sample);

		}
	}
}

float Keyframes::evaluate(const float& time) {

	if (time <= samples.front().x)
		return samples.front().y;

	if (time >= samples.back().x)
		return samples.back().y;

	if (keys.size() == 1)
		return keys[0].pos.y;

	// binary search
	int left = 0;
	int right = samples.size() - 1;

	while (left < right) {
		int middle = left + (right - left + 1) / 2;

		if (time < samples[middle].x)
			right = middle - 1;
		else
			left = middle;
	}

	if (time == samples[left].x) {
		return samples[left].y;
	}

	float x0 = samples[left].x;
	float y0 = samples[left].y;
	float x1 = samples[left + 1].x;
	float y1 = samples[left + 1].y;

	float t = (time - x0) / (x1 - x0);
	return y0 + t * (y1 - y0);


	return 0.f;
}


float Keyframes::getBeginTime() {
	return keys.front().pos.x;
}
float Keyframes::getEndTime() {
	return keys.back().pos.x;
}


void Keyframes::addKeyframe(const Keyframe& k) {

}

void Keyframes::addPoint(const glm::vec2& p) {

}


glm::vec2 Keyframes::lerp(const Keyframe& k1, const Keyframe& k2, const float& t) {
	const glm::vec2 p0 = k1.pos;
	const glm::vec2 p1 = k1.out;
	const glm::vec2 p2 = k2.in;
	const glm::vec2 p3 = k2.pos;

	auto lerp_func = [](const glm::vec2& a, const glm::vec2& b, const float& time) {
		return a + time * (b - a);
		};

	glm::vec2 a = lerp_func(p0, p1, t);
	glm::vec2 b = lerp_func(p1, p2, t);
	glm::vec2 c = lerp_func(p2, p3, t);

	glm::vec2 d = lerp_func(a, b, t);
	glm::vec2 e = lerp_func(b, c, t);

	return lerp_func(d, e, t);
}

float Keyframes::calculateCurvature(const Keyframe& k1, const Keyframe& k2, float t) {
	glm::vec2 p0 = k1.pos;
	glm::vec2 p1 = k1.out;
	glm::vec2 p2 = k2.in;
	glm::vec2 p3 = k2.pos;

	// Первая производная
	glm::vec2 d1 = 3.0f * (1.0f - t) * (1.0f - t) * (p1 - p0) +
		6.0f * (1.0f - t) * t * (p2 - p1) +
		3.0f * t * t * (p3 - p2);

	// Вторая производная
	glm::vec2 d2 = 6.0f * (1.0f - t) * (p2 - 2.0f * p1 + p0) +
		6.0f * t * (p3 - 2.0f * p2 + p1);

	float dx_dt = d1.x;
	float dy_dt = d1.y;
	float d2x_dt2 = d2.x;
	float d2y_dt2 = d2.y;

	float numerator = std::abs(dx_dt * d2y_dt2 - dy_dt * d2x_dt2);
	float denominator = std::pow(dx_dt * dx_dt + dy_dt * dy_dt, 1.5f);
	float curvature = denominator > 0.0001f ? numerator / denominator : 0.0f;

	// Нормализация на длину хорды
	float chordLength = glm::length(k2.pos - k1.pos);
	return curvature * chordLength; // Теперь кривизна безразмерная
}

int Keyframes::estimateSampleCount(const Keyframe& k1, const Keyframe& k2, const float& maxCurvatureThreshold) {
	const int testPoints = 10;
	float maxCurvature = 0.0f;

	// Проверяем кривизну в нескольких точках
	for (int i = 0; i < testPoints; ++i) {
		float t = i / (float)(testPoints - 1);
		float curvature = calculateCurvature(k1, k2, t);
		maxCurvature = std::max(maxCurvature, curvature);
	}

	float t = std::min(maxCurvature / maxCurvatureThreshold, 1.0f);
	int sampleCount = min_sampleRate + (int)(t * (max_sampleRate - min_sampleRate));
	return std::max(min_sampleRate, std::min(max_sampleRate, sampleCount));
}