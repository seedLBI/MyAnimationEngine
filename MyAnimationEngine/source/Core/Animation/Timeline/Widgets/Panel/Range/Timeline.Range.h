#ifndef TIMELINE_RANGE_H
#define TIMELINE_RANGE_H

#include "Core/Animation/Timeline/Transform/Horizontal/Timeline.Transform.Horizontal.h"
#include "Core/Animation/Timeline/ProviderData/Timeline.ProviderData.h"

#include "ThirdParty/ImGui/imgui.h"
#include "ThirdParty/ImGui/imgui_internal.h"

class Timeline_Range {
public:
	Timeline_Range(
		TimelineProviderData* timelineData, 
		TimelineTransformHorizontal* horizontalTransform
	);
	~Timeline_Range();

	void Draw();
	void Update();

private:
	TimelineProviderData* timelineData;
	TimelineTransformHorizontal* horizontalTransform;

	const float button_width = 10.f;
	const float hitbox_width = button_width * 3.f;


	bool left_range_dragging = false;
	bool right_range_dragging = false;

	ImColor color_range = { 0.4f,0.4f,1.f,1.f };
	ImColor color_button = { 1.f,0.f,0.f,1.f };
	ImColor color_button_hovered = { 0.8,0.3f,0.3f,1.f };
	ImColor color_button_down = { 0.3,1.f,1.f,1.f };


};



#endif