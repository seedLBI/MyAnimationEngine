#pragma once

#include "Core/Animation/Timeline/ProviderData/Data/Timeline.Data.h"

class TimelineProviderData {
public:
	TimelineProviderData() = default;
	~TimelineProviderData() = default;

	Timeline& GetData();
	void SetTimeline(Timeline newTimeline);


private:
	Timeline data;
};
