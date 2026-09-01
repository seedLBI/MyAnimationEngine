#ifndef KEYFRAME_RESOLVER_H
#define KEYFRAME_RESOLVER_H

#include <vector>
#include <cstdint>
#include <cmath>
#include <algorithm>
#include <cassert>
#include "Keyframe.h"

template<typename T, uint8_t N>
struct RangeValueKeyframe {
	std::array<T, N> Min;
	std::array<T, N> Max;
};

template<typename T, uint8_t N = 1>
class KeyframeResolver {
public:

	KeyframeResolver(
		std::vector<TypeKeyframe> type_restrictions,
		uint32_t default_sample_rate = 24
	);
	KeyframeResolver(
		const std::array<T, N>& min,
		const std::array<T, N>& max,
		std::vector<TypeKeyframe> type_restrictions,
		uint32_t default_sample_rate = 24
	);
	KeyframeResolver(
		const std::array<T, N>& min, 
		const std::array<T, N>& max, 
		uint32_t default_sample_rate = 24
	);
	KeyframeResolver(
		uint32_t default_sample_rate = 24
	);

	~KeyframeResolver();

	void precompute();

	std::array<T, N>  evaluate(float time);
	const std::array<T, N>& getDefaultValue();
	const std::optional<RangeValueKeyframe<T,N>>& getRange();
	const std::vector<Keyframe<T, N>>& getKeyframes();
	const std::vector<TypeKeyframe>& getTypeRestrictions();

	void setDefaultValue(std::array<T, N>& value);

	void  setDeltaTime(float deltaTime);
	void  applyDeltaTime();
	float getDeltaTime();

	void addFromDefaultValue(float time);
	void add(const Keyframe<T, N>& keyframe);
	void add(float time, std::array<T,N>& value, TypeKeyframe type = TypeKeyframe::LINEAR);
	void changeKeyframe(uint64_t index, const Keyframe<T, N>& keyframe);
	void changeType(uint64_t index, TypeKeyframe type);
	void erase(uint64_t index);
	void clear();
	void changeDefaultSampleRate(uint32_t default_sample_rate);

	Keyframe<T, N>* getKeyframeByID(uint64_t id);
	void setDeltaTimeForKeyframeByID(float deltaTime, uint64_t id);

private:
	static uint64_t keyframe_id_counter;

	std::array<T,N> default_value;
	std::vector<Keyframe<T, N>> keyframes;
	std::vector<LerpedSample<T, N>> samples;
	std::vector<TypeKeyframe> type_restrictions;

	std::optional<RangeValueKeyframe<T, N>> range;

	float deltaTime = 0.f;

	uint32_t default_sample_rate = 24;
	bool is_dirty = true;

	LerpedSample<T, N> lerpCubicBezier(
		const Keyframe<T, N>& a,
		const Keyframe<T, N>& b,
		float t
	);

	uint64_t getCountForReserve();
	void sortKeyframes();
};

template<typename T, uint8_t N>
uint64_t KeyframeResolver<T, N>::keyframe_id_counter = 1;


template<typename T, uint8_t N>
Keyframe<T, N>* KeyframeResolver<T, N>::getKeyframeByID(uint64_t id) {
	auto it = std::find_if(keyframes.begin(), keyframes.end(),
		[id](const auto& k) { return k.id == id; });
	return it != keyframes.end() ? &(*it) : nullptr;
}

template<typename T, uint8_t N>
void KeyframeResolver<T, N>::setDeltaTimeForKeyframeByID(float deltaTime, uint64_t id) {
	auto* keyframe = getKeyframeByID(id);
	if (!keyframe) return;
	is_dirty = true;

	keyframe->sample.time += deltaTime;

	sortKeyframes();
}


template<typename T, uint8_t N>
void KeyframeResolver<T, N>::sortKeyframes() {
	is_dirty = true;
	std::sort(keyframes.begin(), keyframes.end(),
		[](const Keyframe<T, N>& a, const Keyframe<T, N>& b) {
			return a.sample.time < b.sample.time;
	});
}


template<typename T, uint8_t N>
KeyframeResolver<T, N>::KeyframeResolver(
	std::vector<TypeKeyframe> type_restrictions,
	uint32_t default_sample_rate
) 
	:
	default_sample_rate(std::max(uint32_t(2), default_sample_rate)),
	range(std::nullopt),
	type_restrictions(type_restrictions),
	default_value({})
{
	this->is_dirty = true;
	this->keyframe_id_counter = 0;
}



template<typename T, uint8_t N>
KeyframeResolver<T, N>::KeyframeResolver(
	const std::array<T, N>& min,
	const std::array<T, N>& max,
	std::vector<TypeKeyframe> type_restrictions,
	uint32_t default_sample_rate
) 
	:
	default_sample_rate(std::max(uint32_t(2), default_sample_rate)),
	range(RangeValueKeyframe<T, N>{ min, max }),
	type_restrictions(type_restrictions),
	default_value({})
{
	this->is_dirty = true;
	this->keyframe_id_counter = 0;

}



template<typename T, uint8_t N>
KeyframeResolver<T, N>::KeyframeResolver(
	const std::array<T, N>& min, 
	const std::array<T, N>& max, 
	uint32_t default_sample_rate
)
	:
	default_sample_rate(std::max(uint32_t(2), default_sample_rate)),
	range(RangeValueKeyframe<T,N>{ min, max }),
	default_value({})
{
	this->is_dirty = true;
	this->keyframe_id_counter = 0;

}

template<typename T, uint8_t N>
KeyframeResolver<T, N>::KeyframeResolver(uint32_t default_sample_rate)
	: 
	default_sample_rate(std::max(uint32_t(2), default_sample_rate)),
	range(std::nullopt),
	default_value({})
{
	this->is_dirty = true;
	this->keyframe_id_counter = 0;

}

template<typename T, uint8_t N>
KeyframeResolver<T, N>::~KeyframeResolver()
{

}

template<typename T, uint8_t N>
void KeyframeResolver<T, N>::setDeltaTime(float deltaTime) {
	this->deltaTime = deltaTime;
}
template<typename T, uint8_t N>
void KeyframeResolver<T, N>::applyDeltaTime() {
	if (this->deltaTime == 0.f) return;
	is_dirty = true;

	for (auto& keyframe : keyframes) {
		keyframe.sample.time += this->deltaTime;
		for (size_t i = 0; i < N; i++) {
			if (keyframe.anchors.in.has_value())
				keyframe.anchors.in.value().time[i] += this->deltaTime;
			if (keyframe.anchors.out.has_value())
				keyframe.anchors.out.value().time[i] += this->deltaTime;
		}
	}

	this->deltaTime = 0.f;
}
template<typename T, uint8_t N>
float KeyframeResolver<T, N>::getDeltaTime() {
	return this->deltaTime;
}



template<typename T, uint8_t N>
const std::optional<RangeValueKeyframe<T,N>>& KeyframeResolver<T, N>::getRange() {
	return range;
}

template<typename T, uint8_t N>
void KeyframeResolver<T, N>::setDefaultValue(std::array<T, N>& value) {
	if (range.has_value()) {
		for (size_t i = 0; i < N; i++) {
			const auto& r = range.value();
			this->default_value[i] = std::clamp(value[i], r.Min[i], r.Max[i]);
		}
	}
	else
		this->default_value = value;

	this->is_dirty = true;
}

template<typename T, uint8_t N>
const std::array<T, N>& KeyframeResolver<T, N>::getDefaultValue() {
	return default_value;
}

template<typename T, uint8_t N>
const std::vector<TypeKeyframe>& KeyframeResolver<T, N>::getTypeRestrictions() {
	return type_restrictions;
}

template<typename T, uint8_t N>
const std::vector<Keyframe<T, N>>& KeyframeResolver<T, N>::getKeyframes() {
	return keyframes;
}

template<typename T, uint8_t N>
void KeyframeResolver<T, N>::changeDefaultSampleRate(uint32_t default_sample_rate) {
	this->default_sample_rate = default_sample_rate;
}

template<typename T, uint8_t N>
void KeyframeResolver<T, N>::changeKeyframe(uint64_t index, const Keyframe<T, N>& keyframe) {
	if (index > keyframes.size() - 1)
		return;

	keyframes[index] = keyframe;
	this->is_dirty = true;
}

template<typename T, uint8_t N>
void KeyframeResolver<T, N>::changeType(uint64_t index, TypeKeyframe type) {
	if (index > keyframes.size() - 1)
		return;

	keyframes[index].type = type;
	this->is_dirty = true;
}

template<typename T, uint8_t N>
void KeyframeResolver<T, N>::erase(uint64_t index) {
	if (keyframes.empty() || index >= keyframes.size())
		return;

	keyframes.erase(keyframes.begin() + index);
	this->is_dirty = true;
}

template<typename T, uint8_t N>
void KeyframeResolver<T, N>::clear() {
	keyframes.clear();
	this->is_dirty = true;
}

template<typename T, uint8_t N>
void KeyframeResolver<T, N>::addFromDefaultValue(float time) {
	Keyframe<T, N> keyframe;
	keyframe.sample = {time, getDefaultValue() };
	keyframe.type = TypeKeyframe::LINEAR;
	add(keyframe);
}

template<typename T, uint8_t N>
void KeyframeResolver<T, N>::add(const Keyframe<T, N>& keyframe) {
	this->is_dirty = true;

	auto keyframe_add = keyframe;


	if (range.has_value()) {
		for (size_t i = 0; i < N; i++) {
			const auto& r = range.value();
			keyframe_add.sample.value[i] = std::clamp(keyframe.sample.value[i], r.Min[i], r.Max[i]);
		}
	}

	if (keyframes.empty()) {
		keyframe_add.id = this->keyframe_id_counter++;
		keyframes.push_back(keyframe_add);
		return;
	}

	for (size_t i = 0; i < keyframes.size(); i++) {

		float eps = 0.00001f;
		float diff = std::abs(keyframe.sample.time - keyframes[i].sample.time);

		if (keyframe.sample.time < keyframes[i].sample.time) {
			if (keyframe_add.id == 0)
				keyframe_add.id = this->keyframe_id_counter++;
			keyframes.insert(keyframes.begin() + i, keyframe_add);
			return;
		}
		else if (keyframe.sample.time == keyframes[i].sample.time || diff < eps) {
			keyframe_add.id = keyframes[i].id;
			keyframes[i] = keyframe_add;

			return;
		}
	}


	keyframe_add.id = this->keyframe_id_counter++;
	keyframes.push_back(keyframe_add);
}


template<typename T, uint8_t N>
void KeyframeResolver<T, N>::add(float time, std::array<T, N>& value, TypeKeyframe type) {

	Keyframe<T, N> keyframe;
	keyframe.sample.time = time;
	keyframe.sample.value = value;
	keyframe.type = type;

	add(keyframe);
}


template<typename T, uint8_t N>
LerpedSample<T, N> KeyframeResolver<T, N>::lerpCubicBezier(
	const Keyframe<T, N>& a,
	const Keyframe<T, N>& b,
	float t)
{
	LerpedSample<T, N> result;

	const float t1 = (1.f - t) * (1.f - t) * (1.f - t);
	const float t2 = 3.f * (1.f - t) * (1.f - t) * t;
	const float t3 = 3.f * (1.f - t) * t * t;
	const float t4 = t * t * t;

	for (uint8_t i = 0; i < N; i++) {

		BezierAnchor<T, N> anchor_key2{};
		if (b.anchors.in.has_value())
			anchor_key2 = b.anchors.in.value();
		else {
			anchor_key2.time.fill(b.sample.time);
			anchor_key2.value = b.sample.value;
		}

		result.time[i] =
			t1 * a.sample.time +
			t2 * a.anchors.out->time[i] +
			t3 * anchor_key2.time[i] +
			t4 * b.sample.time;

		result.value[i] =
			t1 * a.sample.value[i] +
			t2 * a.anchors.out->value[i] +
			t3 * anchor_key2.value[i] +
			t4 * b.sample.value[i];
	}

	return result;
}

template<typename T, uint8_t N>
uint64_t KeyframeResolver<T, N>::getCountForReserve() {
	uint64_t result = 0;

	for (size_t i = 0; i < keyframes.size() - 1; i++) {
		switch (keyframes[i].type) {
		case TypeKeyframe::BINARY: {
			result += 2 + (i == 0);
			break;
		}
		case TypeKeyframe::LINEAR: {
			result += 1 + (i == 0);
			break;
		}
		case TypeKeyframe::BEZIER: {
			if (keyframes[i].need_sample.has_value())
				result += keyframes[i].need_sample.value() - 1 + (i == 0);
			else
				result += default_sample_rate - 1 + (i == 0);
			break;
		}
		default:
			break;
		}
	}

	return result;
}

template<typename T, uint8_t N>
void KeyframeResolver<T, N>::precompute()
{
	this->is_dirty = false;
	samples.clear();
	if (keyframes.empty()) return;

	samples.reserve(getCountForReserve());

	if (keyframes.size() == 1) {
		samples.push_back(keyframes.front().sample);
		return;
	}


	for (size_t i = 0; i < keyframes.size() - 1; i++) {

		const auto& key1 = keyframes[i];
		const auto& key2 = keyframes[i + 1];

		switch (key1.type) {
		case TypeKeyframe::BINARY: {
			if (i == 0)
				samples.push_back(LerpedSample<T,N>(key1.sample));
			KeyframeSample<T, N> temp;
			temp.time = key2.sample.time;
			temp.value = key1.sample.value;
			samples.push_back(LerpedSample<T, N>(temp));
			samples.push_back(LerpedSample<T, N>(key2.sample));
			break;
		}
		case TypeKeyframe::LINEAR: {
			if (i == 0)
				samples.push_back(key1.sample);
			samples.push_back(key2.sample);
			break;
		}
		case TypeKeyframe::BEZIER: {
			uint32_t rate = default_sample_rate;
			if (key1.need_sample.has_value())
				rate = key1.need_sample.value();

			float step = 1.f / float(rate - 1);

			for (size_t j = (i == 0 ? 0 : 1); j < rate; j++)
				samples.push_back(lerpCubicBezier(key1, key2, step * float(j)));

			break;
		}
		default:
			break;
		}

	}
}

template<typename T, uint8_t N>
std::array<T, N> KeyframeResolver<T, N>::evaluate(float time)
{
	time += this->deltaTime;

	if (is_dirty)
		precompute();

	if (samples.empty()) 
		return default_value;

	if (keyframes.size() == 1)
		return keyframes.front().sample.value;

	if (time <= samples.front().time.front())
		return samples.front().value;

	if (time >= samples.back().time.back())
		return samples.back().value;


	std::array<T, N> result;

	for (size_t i = 0; i < N; i++) {
		int64_t left = 0;
		int64_t right = samples.size() - 1;

		while (left < right) {
			int64_t middle = left + (right - left + 1) / 2;
			if (time < samples[middle].time[i])
				right = middle - 1;
			else
				left = middle;
		}

		if (time == samples[left].time[i]) {
			result[i] = samples[left].value[i];
			continue;
		}

		float x0 = samples[left].time[i];
		T y0 = samples[left].value[i];
		float x1 = samples[left + 1].time[i];
		T y1 = samples[left + 1].value[i];

		float t = (time - x0) / (x1 - x0);

		result[i] = y0 + t * (y1 - y0);
	}

	return result;
}


#endif