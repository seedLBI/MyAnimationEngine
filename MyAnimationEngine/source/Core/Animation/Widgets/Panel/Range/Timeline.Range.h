#ifndef TIMELINE_RANGE_H
#define TIMELINE_RANGE_H

#include "Core/Animation/Widgets/Panel/Transform/Horizontal/Timeline.Transform.Horizontal.h"
#include "Core/Animation/Timeline/ProviderData/Timeline.ProviderData.h"

#include "Core/Animation/Widgets/Panel/TimeCursor/Timeline.TimeCursor.h"

#include "ThirdParty/ImGui/imgui.h"
#include "ThirdParty/ImGui/imgui_internal.h"

class Timeline_Range {
public:
	Timeline_Range(
		Timeline_TimeCursor* timeCursor,
		TimelineProviderData* timelineData, 
		TimelineTransformHorizontal* horizontalTransform
	);
	~Timeline_Range();

	void Draw();
	void Update();

private:
	Timeline_TimeCursor* timeCursor;
	TimelineProviderData* timelineData;
	TimelineTransformHorizontal* horizontalTransform;

	const float button_width = 10.f;
	const float hitbox_width = button_width * 3.f;


	bool left_range_dragging = false;
	bool right_range_dragging = false;

	bool range_dragging = false;

	float global_pos_begin_draging = 0.f;
	float length_range_dragging = 0.f;
	float left_range_before_dragging = 0.f;
	float right_range_before_dragging = 0.f;

};



#endif