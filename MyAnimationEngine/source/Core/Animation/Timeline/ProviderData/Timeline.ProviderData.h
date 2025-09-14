#pragma once

#include "Core/Animation/Timeline/ProviderData/Data/Timeline.Data.h"

class TimelineProviderData {
public:
	TimelineProviderData() = default;
	~TimelineProviderData() = default;

	Timeline& GetData();

private:
	Timeline data;
};
