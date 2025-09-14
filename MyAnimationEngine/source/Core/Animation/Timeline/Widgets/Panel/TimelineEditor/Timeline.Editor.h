#pragma once

#include "Core/Animation/Timeline/ProviderData/Timeline.ProviderData.h"
#include "Core/Animation/Layer/ProviderData/Layers.Data.Provider.h"

#include "Core/Animation/Timeline/Widgets/Panel/ListScrollController/Timeline.ListScrollController.h"

class Timeline_Editor
{
public:
	Timeline_Editor(TimelineProviderData* timelineData, LayersDataProvider* layersData, Timeline_ListScrollController* listScrollController);
	~Timeline_Editor();

	void Draw();
	void Update();

private:
	TimelineProviderData* timelineData;
	LayersDataProvider* layersData;
	Timeline_ListScrollController* listScrollController;
};