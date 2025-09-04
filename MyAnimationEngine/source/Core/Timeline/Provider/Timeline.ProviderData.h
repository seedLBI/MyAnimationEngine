#pragma once

#include "Core/Timeline/Data/Timeline.Data.h"

class TimelineProviderData {
public:
	TimelineProviderData() = default;
	~TimelineProviderData() = default;

	Timeline& GetData();

private:
	Timeline data;
};
