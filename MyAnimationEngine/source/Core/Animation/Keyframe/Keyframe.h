#ifndef KEYFRAME_H
#define KEYFRAME_H

#include <cstdint>
#include <array>
#include <algorithm>
#include <optional>

enum class TypeKeyframe {
	LINEAR,
	BINARY,
	BEZIER,
	AUTOBEZIER
};

template<typename T, uint8_t N>
struct BezierAnchor {
	std::array<float, N> time;
	std::array<T, N>	 value;
};

template<typename T, uint8_t N>
struct BezierAnchors {
	std::optional<BezierAnchor<T, N>> in;
	std::optional<BezierAnchor<T, N>> out;
};

template<typename T, uint8_t N>
struct KeyframeSample {
	float time;
	std::array<T, N> value;
	T MinValue() const { return *std::min_element(value.begin(), value.end()); }
	T MaxValue() const { return *std::max_element(value.begin(), value.end()); }
};

template<typename T, uint8_t N>
struct LerpedSample {
	std::array<float, N> time;
	std::array<T, N>	 value;
	LerpedSample() = default;
	LerpedSample(float timee, const std::array<T, N>& value) {
		time.fill(timee);
		this->value = value;
	}
	LerpedSample(const KeyframeSample<T,N>& keyframeSample) {
		time.fill(keyframeSample.time);
		this->value = keyframeSample.value;
	}
};

template<typename T, uint8_t N>
struct Keyframe {
	uint64_t id = 0;
	uint8_t type_color = 1;
	KeyframeSample<T, N> sample;
	BezierAnchors<T, N>	anchors;
	TypeKeyframe type;
	std::optional<uint32_t> need_sample;
};

#endif