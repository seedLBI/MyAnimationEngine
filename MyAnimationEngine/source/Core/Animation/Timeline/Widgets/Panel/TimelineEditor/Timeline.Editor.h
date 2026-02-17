#pragma once

#include "Core/Animation/Timeline/ProviderData/Timeline.ProviderData.h"
#include "Core/Animation/Layer/ProviderData/Layers.Data.Provider.h"

#include "Core/Animation/Timeline/Widgets/Panel/ListScrollController/Timeline.ListScrollController.h"

#include "Core/Animation/Timeline/Transform/Horizontal/Timeline.Transform.Horizontal.h"
#include "Utils/Timer/Timer.Framerate.h"



class Timeline_Editor
{
public:
	Timeline_Editor(
		FPS_Timer* deltaTime, 
		TimelineTransformHorizontal* timelineTransformHorizontal, 
		TimelineProviderData* timelineData, 
		LayersDataProvider* layersData, 
		Timeline_ListScrollController* listScrollController);

	~Timeline_Editor();

	void Draw();
	void Update();

private:
	FPS_Timer* deltaTime = nullptr;
	TimelineTransformHorizontal* timelineTransformHorizontal = nullptr;

	TimelineProviderData* timelineData;
	LayersDataProvider* layersData;
	Timeline_ListScrollController* listScrollController;
};