#ifndef LAYER_INFO_H
#define LAYER_INFO_H

#include <string>
#include <cstdint>
#include "Utils/Color/Utils.Color.h"

struct TimeSector {
	long double Begin;
	long double End;
	void Shift(const long double& time) { Begin += time; End += time; }

	struct ForDrag{ // Удалить при OnlyRender сборке
		long double Prev_Length;
		long double Prev_Begin;
		long double OffsetFromBegin;
	} DragInfo;

};

struct LayerInfo {
	uint64_t id;
	std::string name;
	colorRGB color;
	TimeSector time;
	bool isVisible;
	bool isExpanded;
	bool isLocked;
	bool isOnly;
};

#endif