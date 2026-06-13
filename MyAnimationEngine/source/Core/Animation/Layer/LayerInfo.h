#ifndef LAYER_INFO_H
#define LAYER_INFO_H

#include <string>
#include <cstdint>
#include "Utils/Color/Utils.Color.h"

struct TimeSector {
	long double Begin;
	long double End;
	void Shift(const long double& time) { Begin += time; End += time; }
};

struct LayerInfo {
	std::string name;
	uint64_t index;
	colorRGB color;
	TimeSector time;
	bool isVisible;
	bool isExpanded;
};

#endif