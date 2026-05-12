#include "Timeline.ProviderData.h"

Timeline& TimelineProviderData::GetData() {
	return data;
}

void TimelineProviderData::SetTimeline(Timeline newTimeline) {
	this->data = newTimeline;
}