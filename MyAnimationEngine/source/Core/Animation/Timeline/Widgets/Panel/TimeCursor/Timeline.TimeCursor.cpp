#include "Timeline.TimeCursor.h"

Timeline_TimeCursor::Timeline_TimeCursor(
	TimelineProviderData* timelineData,
	TimelineTransformHorizontal* horizontalTransform
) : timelineData(timelineData),
	horizontalTransform(horizontalTransform)
{

}

Timeline_TimeCursor::~Timeline_TimeCursor() {

}

void Timeline_TimeCursor::Draw(const ImVec2& p_min, const ImVec2& p_max) {
	auto drawlist = ImGui::GetWindowDrawList();


	float pos_timeline = p_min.x + horizontalTransform->getPostionScreen_lerped(timelineData->GetData().currentTime);

	float height_timeline = ImGui::GetTextLineHeight() * 2.f;

	ImVec2 min_pos_main_tick = { pos_timeline - width_cursor / 2.f, p_min.y };
	ImVec2 max_pos_main_tick = { pos_timeline + width_cursor / 2.f, p_max.y };


	ImColor color_main_tick_timeline = ImColor(1.f, 0.3f, 0.3f, 1.f);
	ImColor color_main_tick_timeline_transparent = ImColor(1.f, 0.3f, 0.3f, 0.f);


	drawlist->AddRectFilled(min_pos_main_tick, max_pos_main_tick, color_main_tick_timeline, 2.f, 0.f);

}

void Timeline_TimeCursor::Update() {

	delta_time = timelineData->GetData().currentTime - prev_value;
	prev_value = timelineData->GetData().currentTime;


}