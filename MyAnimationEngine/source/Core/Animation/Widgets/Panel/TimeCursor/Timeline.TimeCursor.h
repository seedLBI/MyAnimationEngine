#ifndef TIMELINE_TIME_CURSOR_H
#define TIMELINE_TIME_CURSOR_H

#include "Core/Animation/Widgets/Panel/Context/Timeline.Context.h"

#include "Utils/LerpedValue/LerpedValue.h"

#include "ThirdParty/ImGui/imgui.h"

class Timeline_TimeCursor {
public:
	Timeline_TimeCursor(TimelineContext* ctx);
	~Timeline_TimeCursor();

	void Draw(const ImVec2& p_min, const ImVec2& p_max);
	void Update(const float& dt);

private:
	TimelineContext* ctx;

	LerpedValue<float> width_tail_lerped;
	const float max_width_tail = 50.f;

	const float width_cursor = 2.f;

	float prev_value = 0.f;
	float delta_time = 0.f;

};





#endif