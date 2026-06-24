#pragma once

#include "Core/Animation/Timeline/ProviderData/Timeline.ProviderData.h"
#include "Core/Animation/Layer/ProviderData/Layers.Data.Provider.h"

#include "ThirdParty/ImGui/imgui.h"
#include <string>

#include "Utils/Color/Utils.Color.h"

class Timeline_UpperControls {
public:
	Timeline_UpperControls(LayersDataProvider* layersData, TimelineProviderData* timelineData);
	~Timeline_UpperControls();

	void Draw();
	void Update();

	const bool& isGraphMode();
	const bool& isMotionBlurMode();

private:
	LayersDataProvider* layersData;
	TimelineProviderData* timelineData;

	bool GraphMode		= false;
	bool MotionBlueMode = false;

	float DrawTimer();
	float DrawFinder(float availableWidth);
	float DrawButtons();

	std::string getTimer();
	std::string getFrame();
};