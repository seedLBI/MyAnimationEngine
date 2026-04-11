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

	float scale_upper = 1.1f;
	float scale_bottom = 0.8f;
	ImColor color_upperTimerText  = { 58, 150, 255, 255 };
	ImColor color_bottomTimerText = { 58, 150, 255, 255 };

	ImVec2 DrawTimer(const ImVec2& p);
};