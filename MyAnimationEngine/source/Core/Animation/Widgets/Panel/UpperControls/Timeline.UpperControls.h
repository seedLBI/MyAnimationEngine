#pragma once

#include "Core/Animation/Timeline/ProviderData/Timeline.ProviderData.h"

#include "ThirdParty/ImGui/imgui.h"
#include <string>

#include "Utils/Color/Utils.Color.h"

class Timeline_UpperControls {
public:
	Timeline_UpperControls(TimelineProviderData* timelineData);
	~Timeline_UpperControls();

	void Draw();
	void Update();

	const bool& isGraphMode();


private:
	TimelineProviderData* timelineData;

	bool GraphMode = false;

	void DrawTimer();
	void DrawFinder();
	void DrawButtons();

	std::string getTimer();
	std::string getFrame();
};