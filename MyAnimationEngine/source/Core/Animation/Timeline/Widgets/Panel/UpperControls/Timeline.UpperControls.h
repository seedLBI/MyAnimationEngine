#pragma once

#include "Core/Animation/Timeline/ProviderData/Timeline.ProviderData.h"

#include "ThirdParty/ImGui/imgui.h"

class Timeline_UpperControls {
public:
	Timeline_UpperControls(TimelineProviderData* timelineData);
	~Timeline_UpperControls();

	void Draw();
	void Update();

private:
	TimelineProviderData* timelineData;

	ImColor color_timerText = { 38, 110, 235, 255 };

	ImVec2 DrawTimer(const ImVec2& p);
};