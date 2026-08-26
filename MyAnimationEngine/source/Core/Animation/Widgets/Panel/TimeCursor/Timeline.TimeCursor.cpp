#include "Timeline.TimeCursor.h"

Timeline_TimeCursor::Timeline_TimeCursor(TimelineContext* ctx) : ctx(ctx)
{

}

Timeline_TimeCursor::~Timeline_TimeCursor() {

}

void Timeline_TimeCursor::Draw(const ImVec2& p_min, const ImVec2& p_max) {
	auto drawlist = ImGui::GetWindowDrawList();


	float pos_timeline = p_min.x + ctx->horizontalTransform->getPostionScreen_lerped(ctx->timelineData->GetData().currentTime);

	float height_timeline = ImGui::GetTextLineHeight() * 2.f;


	float width_frame_time = (float)ctx->timelineData->GetData().getLengthTimeFrame();

	float pos_integer_current = width_frame_time * std::floorf(ctx->timelineData->GetData().currentTime / width_frame_time);


	float screen_int_cursor_pos_1 = p_min.x + ctx->horizontalTransform->getPostionScreen_lerped(pos_integer_current);
	float screen_int_cursor_pos_2 = p_min.x + ctx->horizontalTransform->getPostionScreen_lerped(pos_integer_current + width_frame_time);




	ImVec2 min_pos = { pos_timeline - width_cursor / 2.f, p_min.y };
	ImVec2 max_pos = { pos_timeline + width_cursor / 2.f, p_max.y };


	ImColor color = ImColor(1.f, 0.3f, 0.3f, 1.f);
	ImColor color_alpha = ImColor(1.f, 0.0f, 0.0f, 0.f);


	drawlist->AddRectFilled({ screen_int_cursor_pos_1 , p_min.y }, { screen_int_cursor_pos_2, p_max.y }, ImColor(1.f,1.f,1.f,0.1f), 0.f, 0.f);


	drawlist->AddRectFilled(min_pos, max_pos, color, 2.f, 0.f);

	drawlist->AddRectFilledMultiColor(
		{ min_pos.x - width_tail_lerped.GetLerped(), min_pos.y },
		max_pos,
		color_alpha, color, color, color_alpha);

}

void Timeline_TimeCursor::Update(const float& dt) {

	delta_time = ctx->timelineData->GetData().currentTime - prev_value;
	prev_value = ctx->timelineData->GetData().currentTime;

	float value_set_delta = delta_time * 300.f;

	if (std::abs(value_set_delta) > max_width_tail){
		value_set_delta = max_width_tail * (std::abs(value_set_delta) / value_set_delta);
	}

	width_tail_lerped.Set(value_set_delta);
	width_tail_lerped.Update(dt * 10.f);
}