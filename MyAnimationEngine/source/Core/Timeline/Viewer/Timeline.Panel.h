#pragma once
#include "Core/Timeline/Provider/Timeline.ProviderData.h"


class TimelinePanel {
public:
	TimelinePanel(TimelineProviderData* provider);
	~TimelinePanel();

	void Draw();
	void Update();

private:
	TimelineProviderData* provider = nullptr;
};



