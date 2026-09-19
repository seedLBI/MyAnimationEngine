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

	void setNeedPrecompute();
	void precompute();

	std::array<T, N>  evaluate(float time);
	
	const std::vector<LerpedSample<T, N>>& getSamples();
	const std::optional<RangeValueKeyframe<T,N>>& getRange();
	const std::vector<Keyframe<T, N>>& getKeyframes();
	const std::vector<TypeKeyframe>& getTypeRestrictions();

	void setDefaultValue(std::array<T, N>& value);
	const std::array<T, N>& getDefaultValue();

	void  setDeltaTime(float deltaTime);
	void  applyDeltaTime();
	float getDeltaTime();

	uint64_t addFromDefaultValue(float time);
	uint64_t add(const Keyframe<T, N>& keyframe);
	uint64_t add(float time, std::array<T,N>& value, TypeKeyframe type = TypeKeyframe::AUTOBEZIER);
	void changeKeyframe(uint64_t index, const Keyframe<T, N>& keyframe);
	void changeType(uint64_t index, TypeKeyframe type);
	void erase(uint64_t index);
	void clear();
	void changeDefaultSampleRate(uint32_t default_sample_rate);

	Keyframe<T, N>* getKeyframeByID(uint64_t id);
	void setDeltaTimeForKeyframeByID(float deltaTime, uint64_t id);

	uint64_t GetIndexByID(uint64_t id);
	void EraseByID(uint64_t id);
	
	void changeTypeByID(uint64_t id, TypeKeyframe new_type);

	bool hasAnyOverlaps() const;
	void resolveOverlaps(uint64_t priority_id);

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

	LerpedSample<T, N> lerpCatmullRom(
		const Keyframe<T, N>& p0,
		const Keyframe<T, N>& p1,
		const Keyframe<T, N>& p2,
		const Keyframe<T, N>& p3,
		float t,
		float alpha = 0.5
	);

	std::array<LerpedSample<T, N>, 2> getTangentCatmullRomForCubicBezier(
		const Keyframe<T, N>& p0,
		const Keyframe<T, N>& p1,
		const Keyframe<T, N>& p2,
		const Keyframe<T, N>& p3,
		float alpha = 0.5
	);


	uint64_t getCountForReserve();
	void sortKeyframes();
};

template<typename T, uint8_t N>
uint64_t KeyframeResolver<T, N>::keyframe_id_counter = 1;




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
const std::vector<LerpedSample<T, N>>& KeyframeResolver<T, N>::getSamples() {
	return samples;
}

template<typename T, uint8_t N>
void KeyframeResolver<T, N>::setNeedPrecompute() {
	this->is_dirty = true;
}


template<typename T, uint8_t N>
bool KeyframeResolver<T, N>::hasAnyOverlaps() const {
	if (keyframes.size() < 2) return false;
	float eps = 0.0001f;

	for (size_t i = 0; i < keyframes.size() - 1; ++i) {
		if (std::abs(keyframes[i].sample.time - keyframes[i + 1].sample.time) < eps) {
			return true;
		}
	}
	return false;
}

template<typename T, uint8_t N>
uint64_t KeyframeResolver<T, N>::GetIndexByID(uint64_t id) {
	for (uint64_t i = 0; i < keyframes.size(); i++)
		if (keyframes[i].id == id)
			return i;

	return UINT64_MAX;
}

template<typename T, uint8_t N>
void KeyframeResolver<T, N>::EraseByID(uint64_t id) {

	auto index = GetIndexByID(id);

	if (index != UINT64_MAX) {
		keyframes.erase(keyframes.begin() + index);
		is_dirty = true;
	}

}

template<typename T, uint8_t N>
void KeyframeResolver<T, N>::changeTypeByID(uint64_t id, TypeKeyframe new_type) {
	auto index = GetIndexByID(id);
	if (index == UINT64_MAX)
		return;

	changeType(index, new_type);
}

template<typename T, uint8_t N>
void KeyframeResolver<T, N>::resolveOverlaps(uint64_t priority_id) {
	auto* keyframe = getKeyframeByID(priority_id);
	if (!keyframe)
		return;

	float target_time = keyframe->sample.time;

	float eps = 0.0001f;

	auto it = std::remove_if(keyframes.begin(), keyframes.end(),
		[priority_id, target_time, eps](const Keyframe<T, N>& kf) {
			return kf.id != priority_id && std::abs(kf.sample.time - target_time) < eps;
		});

	if (it != keyframes.end()) {
		keyframes.erase(it, keyframes.end());
		is_dirty = true;
	}
}

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
void KeyframeResolver<T, N>::setDeltaTime(float deltaTime) {
	this->deltaTime = deltaTime;
}
template<typename T, uint8_t N>
void KeyframeResolver<T, N>::applyDeltaTime() {
	if (this->deltaTime == 0.f) return;
	is_dirty = true;

	for (auto& keyframe : keyframes) {
		keyframe.sample.time += this->deltaTime;
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

	if (keyframes[index].type == type)
		return;


	keyframes[index].type = type;
	switch (type) {
	case TypeKeyframe::LINEAR:
	case TypeKeyframe::BINARY: {
		keyframes[index].anchors.in.reset();
		keyframes[index].anchors.out.reset();
		break;
	}
	case TypeKeyframe::BEZIER: {
		BezierAnchor<T, N> in, out;

		for (size_t i = 0; i < N; i++) {
			in.time[i] = 1.f / 3.f;
			in.value[i] = keyframes[index].sample.value[i];
		}
		out = in;

		keyframes[index].anchors.in = in;
		keyframes[index].anchors.out = out;

		break;
	}
	case TypeKeyframe::AUTOBEZIER:
		break;
	default:
		break;
	}


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
uint64_t KeyframeResolver<T, N>::addFromDefaultValue(float time) {
	Keyframe<T, N> keyframe;
	keyframe.sample = {time, getDefaultValue() };
	keyframe.type = TypeKeyframe::AUTOBEZIER;
	return add(keyframe);
}

template<typename T, uint8_t N>
uint64_t KeyframeResolver<T, N>::add(const Keyframe<T, N>& keyframe) {
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
		return keyframe_add.id;
	}

	for (size_t i = 0; i < keyframes.size(); i++) {

		float eps = 0.0001f;
		float diff = std::abs(keyframe.sample.time - keyframes[i].sample.time);

		if (keyframe.sample.time < keyframes[i].sample.time) {
			if (keyframe_add.id == 0)
				keyframe_add.id = this->keyframe_id_counter++;
			keyframes.insert(keyframes.begin() + i, keyframe_add);
			return keyframe_add.id;
		}
		else if (keyframe.sample.time == keyframes[i].sample.time || diff < eps) {
			keyframe_add.id = keyframes[i].id;
			keyframes[i] = keyframe_add;

			return keyframe_add.id;
		}
	}


	keyframe_add.id = this->keyframe_id_counter++;
	keyframes.push_back(keyframe_add);
	return keyframe_add.id;
}


template<typename T, uint8_t N>
uint64_t KeyframeResolver<T, N>::add(float time, std::array<T, N>& value, TypeKeyframe type) {

	Keyframe<T, N> keyframe;
	keyframe.sample.time = time;
	keyframe.sample.value = value;
	keyframe.type = type;

	return add(keyframe);
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

	float dt = b.sample.time - a.sample.time;

	for (uint8_t i = 0; i < N; i++) {

		float out_time = a.sample.time;
		T out_val = a.sample.value[i];
		if (a.anchors.out.has_value()) {
			out_time = a.sample.time + (a.anchors.out->time[i] * dt);
			out_val = a.anchors.out->value[i];
		}

		float in_time = b.sample.time;
		T in_val = b.sample.value[i];
		if (b.anchors.in.has_value()) {
			in_time = b.sample.time - (b.anchors.in->time[i] * dt);
			in_val = b.anchors.in->value[i];
		}


		result.time[i] =
			t1 * a.sample.time +
			t2 * out_time +
			t3 * in_time +
			t4 * b.sample.time;

		result.value[i] =
			t1 * a.sample.value[i] +
			t2 * out_val +
			t3 * in_val +
			t4 * b.sample.value[i];
	}

	return result;
}

template<typename T, uint8_t N>
LerpedSample<T, N> KeyframeResolver<T, N>::lerpCatmullRom(
	const Keyframe<T, N>& p0,
	const Keyframe<T, N>& p1,
	const Keyframe<T, N>& p2,
	const Keyframe<T, N>& p3,
	float t,
	float alpha
) {

	auto RemapT = [](std::array<T, N> t, const Keyframe<T, N>& p0, const Keyframe<T, N>& p1, float alpha) -> std::array<T, N> {
		std::array<T, N> remapped_t;

		for (size_t i = 0; i < N; i++) {
			float  timeDelta = p1.sample.time - p0.sample.time;
			float valueDelta = p1.sample.value[i] - p0.sample.value[i];

			float distance = std::sqrt(timeDelta * timeDelta + valueDelta * valueDelta);

			remapped_t[i] = t[i] + powf(distance, alpha);
		}

		return remapped_t;
	};

	auto CalcPyramidNode = [](
		const std::array<T, N>& t_start, 
		const std::array<T, N>& t_end, 
		const std::array<T, N>& t_current,
		const LerpedSample<T, N>& p_start, 
		const LerpedSample<T, N>& p_end ) -> LerpedSample<T, N> {

		LerpedSample<T, N> result;

		for (size_t i = 0; i < N; i++) {

			float denom = t_end[i] - t_start[i];

			if (std::abs(denom) < 1e-6f) {
				result.time[i] = p_start.time[i];
				result.value[i] = p_start.value[i];
			}
			else {
				T w1 = (t_end[i] - t_current[i]) / denom;
				T w2 = (t_current[i] - t_start[i]) / denom;

				result.time[i]  = p_start.time[i]  * w1 + p_end.time[i] * w2;
				result.value[i] = p_start.value[i] * w1 + p_end.value[i] * w2;
			}

		}

		return result;

	};

	auto mix = [](const std::array<T, N>& t_start, const std::array<T, N>& t_end, float t) -> std::array<T, N> {
		std::array<T, N> result;

		for (size_t i = 0; i < N; i++){
			result[i] = t_start[i] * (1.f - t) + t_end[i] * t;
		}

		return result;
	};

	std::array<T, N> t0; t0.fill(T(0.f));
	std::array<T, N> t1 = RemapT(t0, p0, p1, alpha);
	std::array<T, N> t2 = RemapT(t1, p1, p2, alpha);
	std::array<T, N> t3 = RemapT(t2, p2, p3, alpha);

	std::array<T, N> t_current = mix(t1, t2, t);

	LerpedSample<T, N> l0(p0.sample);
	LerpedSample<T, N> l1(p1.sample);
	LerpedSample<T, N> l2(p2.sample);
	LerpedSample<T, N> l3(p3.sample);

	LerpedSample<T, N> a1 = CalcPyramidNode(t0, t1, t_current, l0, l1);
	LerpedSample<T, N> a2 = CalcPyramidNode(t1, t2, t_current, l1, l2);
	LerpedSample<T, N> a3 = CalcPyramidNode(t2, t3, t_current, l2, l3);

	LerpedSample<T, N> b1 = CalcPyramidNode(t0, t2, t_current, a1, a2);
	LerpedSample<T, N> b2 = CalcPyramidNode(t1, t3, t_current, a2, a3);

	LerpedSample<T, N> c = CalcPyramidNode(t1, t2, t_current, b1, b2);

	return c;
}

template<typename T, uint8_t N>
std::array<LerpedSample<T, N>, 2> KeyframeResolver<T, N>::getTangentCatmullRomForCubicBezier(
	const Keyframe<T, N>& p0,
	const Keyframe<T, N>& p1,
	const Keyframe<T, N>& p2,
	const Keyframe<T, N>& p3,
	float alpha
) {
	std::array<LerpedSample<T, N>, 2> anchors;

	auto RemapT = [](std::array<T, N> t, const Keyframe<T, N>& p0, const Keyframe<T, N>& p1, float alpha) -> std::array<T, N> {
		std::array<T, N> remapped_t;

		for (size_t i = 0; i < N; i++) {
			float  timeDelta = p1.sample.time - p0.sample.time;
			float valueDelta = p1.sample.value[i] - p0.sample.value[i];

			float distance = std::sqrt(timeDelta * timeDelta + valueDelta * valueDelta);

			remapped_t[i] = t[i] + powf(distance, alpha);
		}

		return remapped_t;
	};

	std::array<T, N> t0; t0.fill(T(0.f));
	std::array<T, N> t1 = RemapT(t0, p0, p1, alpha);
	std::array<T, N> t2 = RemapT(t1, p1, p2, alpha);
	std::array<T, N> t3 = RemapT(t2, p2, p3, alpha);

	auto safe_div = [](float num, float den) -> float {
		return (std::abs(den) > 1e-6f) ? (num / den) : 0.0f;
	};

	// anchors cubic bezier for p1 and p2 from CatmullRom
	LerpedSample<T, N> V1, V2;

	for (size_t i = 0; i < N; i++) {
		float p1p0_v = p1.sample.value[i] - p0.sample.value[i];
		float p1p0_t = p1.sample.time - p0.sample.time;

		float p2p1_v = p2.sample.value[i] - p1.sample.value[i];
		float p2p1_t = p2.sample.time - p1.sample.time;

		float p3p2_v = p3.sample.value[i] - p2.sample.value[i];
		float p3p2_t = p3.sample.time - p2.sample.time;

		float t2t1 = t2[i] - t1[i];
		float t3t2 = t3[i] - t2[i];
		float t3t1 = t3[i] - t1[i];

		float s = t2t1 / 3.f;

		V1.time[i]  = p1.sample.time     + s * (safe_div(t2t1, t2[i]) * safe_div(p1p0_t, t1[i]) + safe_div(t1[i], t2[i]) * safe_div(p2p1_t,t2t1));
		V1.value[i] = p1.sample.value[i] + s * (safe_div(t2t1, t2[i]) * safe_div(p1p0_v, t1[i]) + safe_div(t1[i], t2[i]) * safe_div(p2p1_v,t2t1));

		V2.time[i]  = p2.sample.time     - s * (safe_div(t3t2, t3t1) * safe_div(p2p1_t, t2t1) + safe_div(t2t1, t3t1) * safe_div(p3p2_t, t3t2));
		V2.value[i] = p2.sample.value[i] - s * (safe_div(t3t2, t3t1) * safe_div(p2p1_v, t2t1) + safe_div(t2t1, t3t1) * safe_div(p3p2_v, t3t2));
	}

	anchors[0] = V1;
	anchors[1] = V2;

	return anchors;
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
		case TypeKeyframe::BEZIER:
		case TypeKeyframe::AUTOBEZIER:
		{
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
				samples.push_back(LerpedSample<T, N>(key1.sample));
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
		case TypeKeyframe::AUTOBEZIER: {
			uint32_t rate = default_sample_rate;
			if (key1.need_sample.has_value())
				rate = key1.need_sample.value();

			float step = 1.f / float(rate - 1);

			Keyframe<T, N> k0, k1, k2, k3;

			k1 = keyframes[i];
			k2 = keyframes[i + 1];

			if (i == 0) {
				k0.sample.time = (2.f * k1.sample.time - k2.sample.time);
				k0.sample.value = k1.sample.value;
			}
			else {
				k0 = keyframes[i - 1];
			}

			if (i == keyframes.size() - 2) {
				k3.sample.time = (2.f * k2.sample.time - k1.sample.time);
				k3.sample.value = k2.sample.value;
			}
			else {
				k3 = keyframes[i + 2];
			}

			//for (size_t j = (i == 0 ? 0 : 1); j < rate; j++)
			//	samples.push_back(lerpCatmullRom(k0, k1, k2, k3, step * float(j), 0.5f));

			std::array<LerpedSample<T, N>, 2> anchors = getTangentCatmullRomForCubicBezier(k0, k1, k2, k3, 0.5f);

			if (i == 0) {
				anchors[0].value = k1.sample.value;
			}

			if (i == keyframes.size() - 2) {
				anchors[1].value = k2.sample.value;
			}

			if ((i > 0 && k0.type == TypeKeyframe::BINARY)) {
				for (size_t d = 0; d < N; d++) {
					float dt_orig = anchors[0].time[d] - k1.sample.time;
					float dv_orig = float(anchors[0].value[d] - k1.sample.value[d]);
					float orig_length = std::sqrt(dt_orig * dt_orig + dv_orig * dv_orig);
					float sign = (keyframes[i].sample.value[d] > keyframes[i-1].sample.value[d]) ? 1.0f : -1.0f;

					anchors[0].time[d] = k1.sample.time;
					anchors[0].value[d] = k1.sample.value[d] + T(sign * orig_length * 2.f);
				}
			}
			if (k2.type == TypeKeyframe::BINARY) {
				anchors[1].value = k2.sample.value;
			}

			if (i > 0 && k0.type == TypeKeyframe::LINEAR) {
				float dt_lin = k1.sample.time - k0.sample.time;
				if (dt_lin > 0.0001f) {
					for (size_t d = 0; d < N; d++) {
						float dv_lin = float(k1.sample.value[d] - k0.sample.value[d]);

						float dt_orig = anchors[0].time[d] - k1.sample.time;
						float dv_orig = float(anchors[0].value[d] - k1.sample.value[d]);
						float orig_length = std::sqrt(dt_orig * dt_orig + dv_orig * dv_orig);

						float lin_length = std::sqrt(dt_lin * dt_lin + dv_lin * dv_lin);
						if (lin_length > 0.0001f && orig_length > 0.0001f) {
							float dir_t = dt_lin / lin_length;
							float dir_v = dv_lin / lin_length;

							anchors[0].time[d] = k1.sample.time + dir_t * orig_length * 2.f;
							anchors[0].value[d] = k1.sample.value[d] + T(dir_v * orig_length * 2.f);
						}
					}
				}
			}

			if (i < keyframes.size() - 2 && k2.type == TypeKeyframe::LINEAR) {
				float dt_lin = k3.sample.time - k2.sample.time;
				if (dt_lin > 0.0001f) {
					for (size_t d = 0; d < N; d++) {
						float dv_lin = float(k3.sample.value[d] - k2.sample.value[d]);

						float dt_orig = k2.sample.time - anchors[1].time[d];
						float dv_orig = float(k2.sample.value[d] - anchors[1].value[d]);
						float orig_length = std::sqrt(dt_orig * dt_orig + dv_orig * dv_orig);

						float lin_length = std::sqrt(dt_lin * dt_lin + dv_lin * dv_lin);
						if (lin_length > 0.0001f && orig_length > 0.0001f) {
							float dir_t = dt_lin / lin_length;
							float dir_v = dv_lin / lin_length;

							anchors[1].time[d] = k2.sample.time - dir_t * orig_length * 2.f;
							anchors[1].value[d] = k2.sample.value[d] - T(dir_v * orig_length * 2.f);
						}
					}
				}
			}

			float t1 = k1.sample.time;
			float t2 = k2.sample.time;

			float dt_width = std::max(t2 - t1, 0.00001f);

			for (size_t d = 0; d < N; d++) {
				float t_out = anchors[0].time[d];
				if (t_out > t2) {
					float scale = (t2 - t1) / (t_out - t1);
					anchors[0].time[d] = t1 + (t_out - t1) * scale;
					anchors[0].value[d] = k1.sample.value[d] + (anchors[0].value[d] - k1.sample.value[d]) * scale;
				}
				else if (t_out < t1) {
					anchors[0].time[d] = t1;
					anchors[0].value[d] = k1.sample.value[d];
				}

				float t_in = anchors[1].time[d];
				if (t_in < t1) {
					float scale = (t2 - t1) / (t2 - t_in);
					anchors[1].time[d] = t2 - (t2 - t_in) * scale;
					anchors[1].value[d] = k2.sample.value[d] - (k2.sample.value[d] - anchors[1].value[d]) * scale;
				}
				else if (t_in > t2) {
					anchors[1].time[d] = t2;
					anchors[1].value[d] = k2.sample.value[d];
				}

				anchors[0].time[d] = (anchors[0].time[d] - t1) / dt_width;
				anchors[1].time[d] = (t2 - anchors[1].time[d]) / dt_width;
			}


			k1.anchors.out = BezierAnchor<T, N>{ anchors[0].time, anchors[0].value };
			k2.anchors.in  = BezierAnchor<T, N>{ anchors[1].time, anchors[1].value };

			keyframes[i] = k1;
			keyframes[i + 1] = k2;

			for (size_t j = (i == 0 ? 0 : 1); j < rate; j++)
				samples.push_back(lerpCubicBezier(k1, k2, step * float(j)));

			break;
		}
		default:
			break;
		}


		if (range.has_value()) {
			const auto& r = range.value();
			for (size_t i = 0; i < samples.size(); i++) {
				for (size_t j = 0; j < N; j++) {
					samples[i].value[j] = std::clamp(samples[i].value[j], r.Min[j], r.Max[j]);
				}
			}
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