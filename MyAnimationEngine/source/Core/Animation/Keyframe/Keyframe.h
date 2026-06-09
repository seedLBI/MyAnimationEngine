#ifndef KEYFRAME_H
#define KEYFRAME_H

#include <cstdint>
#include <array>
#include <optional>

enum class TypeKeyframe {
	LINEAR,
	BINARY,
	BEZIER
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
	std::array<float, N> time;
	std::array<T, N> value;
	KeyframeSample() = default;
	KeyframeSample(float time, const std::array<T, N>& value) {
		for (uint8_t i = 0; i < N; i++)
			this->time[i] = time;
		this->value = value;
	}
};

template<typename T, uint8_t N>
struct Keyframe {
	KeyframeSample<T, N> sample;
	BezierAnchors<T, N>	anchors;
	TypeKeyframe type;
	std::optional<uint32_t> need_sample;
};

#endif