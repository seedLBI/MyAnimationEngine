#ifndef TIMELINE_RANGE_H
#define TIMELINE_RANGE_H

#include "Core/Animation/Widgets/Panel/Context/Timeline.Context.h"
#include "Core/Animation/Widgets/Panel/TimeCursor/Timeline.TimeCursor.h"

#include "ThirdParty/ImGui/imgui.h"
#include "ThirdParty/ImGui/imgui_internal.h"

class Timeline_Range {
public:
	Timeline_Range(
		TimelineContext* ctx,
		Timeline_TimeCursor* timeCursor
	);
	~Timeline_Range();

	void Draw();
	void Update();

private:
	TimelineContext* ctx;
	Timeline_TimeCursor* timeCursor;

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