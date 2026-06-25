#include "Timeline.Ruler.h"
#include "CursorManager/CursorManger.h"

#include "Utils/Text/Text.h"

#include "Core/DebugOverlay/DebugOverlay.h"
#include "Utils/Math/Math.Lerp.h"

Timeline_Ruler::Timeline_Ruler(
	Timeline_TimeCursor* timeCursor,
	TimelineController* timelineController,
	TimelineProviderData* timelineData, 
	TimelineTransformHorizontal* timelineTransformHorizontal,
	HorizontalTickRenderer* horizontalTickRenderer
)
	: timeCursor(timeCursor),
	  timelineController(timelineController),
	  timelineData(timelineData),
	  timelineTransformHorizontal(timelineTransformHorizontal),
	  horizontalTickRenderer(horizontalTickRenderer)
{

}

Timeline_Ruler::~Timeline_Ruler() {

}

bool Timeline_Ruler::OutOfView(const ImVec2& pos) {
	const ImVec2& min = cursor_pos;
	const ImVec2& max = { cursor_pos.x + region_max.x,  cursor_pos.y + region_max.y };

	float len_x = max.x - min.x;
	float len_y = max.y - min.y;

	float len_pos_x = pos.x - min.x;
	float len_pos_y = pos.y - min.y;

	return (len_pos_x < 0.f || len_pos_x > len_x) || 
		   (len_pos_y < 0.f || len_pos_y > len_y) ;
}

bool Timeline_Ruler::OutOfView(const ImVec2& pos_min, const ImVec2& pos_max) {
	return OutOfView(pos_min) && OutOfView(pos_max);
}



void Timeline_Ruler::Draw() {
	//ImGui::BeginChild(u8"Timeline_Ruler", ImVec2(0, ImGui::GetTextLineHeight() * 2.f));


	auto drawlist = ImGui::GetWindowDrawList();
	cursor_pos = ImGui::GetCursorScreenPos();
	region_max = ImGui::GetContentRegionAvail();

	ImGui::InvisibleButton("Timeline_Ruler_canvas_button", region_max, ImGuiButtonFlags_MouseButtonLeft);

	isHovered = ImGui::IsItemHovered();
	scrol_zoom = ImGui::GetIO().MouseWheel;

	ImGui::SetCursorScreenPos(cursor_pos);

	ImVec2 p_min = cursor_pos;
	ImVec2 p_max = { cursor_pos.x + region_max.x, cursor_pos.y + region_max.y };


	drawlist->AddRectFilled(
		p_min,
		p_max,
		color_background,
		0.f,
		0.f
	);


	float padding_x = 0.f;

	p_min.x += padding_x;
	p_max.x -= padding_x;




	ImVec2 block_min = cursor_pos;
	ImVec2 block_max = { cursor_pos.x + timelineTransformHorizontal->getPostionScreen_lerped(0.f), cursor_pos.y + region_max.y };

	ImColor color_left_bound_outOfBounds  = color_bounds;
	ImColor color_right_bound_outOfBounds = color_bounds;

	Lerp(color_left_bound_outOfBounds, color_bounds_offsetMax,  1.f - time_left_bound.GetLerped());
	Lerp(color_right_bound_outOfBounds, color_bounds_offsetMax, 1.f - time_right_bound.GetLerped());

	if (OutOfView(block_max) == false || block_max.x > p_max.x) {


		drawlist->AddRectFilledMultiColor(
			block_min,
			{ block_max.x - width_gradient_bound, block_max.y},
			color_left_bound_outOfBounds, color_bounds, color_bounds, color_left_bound_outOfBounds
		);


		drawlist->AddRectFilledMultiColor(
			{ block_max.x - width_gradient_bound,  block_min.y},
			block_max,
			color_bounds, color_background, color_background, color_bounds
		);

	}

	block_min = { cursor_pos.x + timelineTransformHorizontal->getPostionScreen_lerped(timelineData->GetData().totalTime), cursor_pos.y };
	block_max = { cursor_pos.x + region_max.x, cursor_pos.y + region_max.y };

	if (OutOfView(block_min) == false || block_min.x < p_max.x) {

		drawlist->AddRectFilledMultiColor(
			block_min,
			{ block_min.x + width_gradient_bound, block_max.y },
			color_background, color_bounds, color_bounds, color_background
		);

		drawlist->AddRectFilledMultiColor(
			{ block_min.x + width_gradient_bound, block_min.y },
			block_max,
			color_bounds, color_right_bound_outOfBounds, color_right_bound_outOfBounds, color_bounds
		);

	}



	horizontalTickRenderer->DrawTicks(
		p_min,
		p_max,
		color_mainTick, color_subTick,
		width_main_tick, width_sub_tick,
		ImGui::GetTextLineHeight(), ImGui::GetTextLineHeight() / 3.f, ImGui::GetTextLineHeight() / 1.5f,
		true
	);

	/*
	int count_tick_rendered = 0;

	float height_main_tick		= ImGui::GetTextLineHeight();
	float height_sub_minor_tick = ImGui::GetTextLineHeight() / 3.f;
	float height_sub_major_tick = ImGui::GetTextLineHeight() / 1.5f;


	float step_Main = 1.f;
	float distance_between_main_ticks = timelineTransformHorizontal->getPostionScreen_lerped(step_Main) - timelineTransformHorizontal->getPostionScreen_lerped(0.f);
	int count_main_tick = timelineData->GetData().totalTime;
	float size_main_tick_text = 52.f;


	if (distance_between_main_ticks < size_main_tick_text) {
		float count = std::round(size_main_tick_text / distance_between_main_ticks);
		float countTicks = std::floor(count_main_tick / count);
		float stepTick = timelineData->GetData().totalTime / countTicks;
		step_Main = stepTick;
		count_main_tick = countTicks;
		distance_between_main_ticks = timelineTransformHorizontal->getPostionScreen_lerped(stepTick) - timelineTransformHorizontal->getPostionScreen_lerped(0.f);
	}

	int begin_index_main_tick = timelineTransformHorizontal->getLeftBoundView_global_lerped() / step_Main;
	if (begin_index_main_tick < 0) begin_index_main_tick = 0;




	float step_Sub = 1.f / (float)timelineData->GetData().framerate;
	float distance_between_sub_ticks = timelineTransformHorizontal->getPostionScreen_lerped(step_Sub) - timelineTransformHorizontal->getPostionScreen_lerped(0.f);
	int count_sub_tick = timelineData->GetData().framerate;
	float size_sub_tick_text = 16.f;
	bool draw_sub_ticks = true;

	if (distance_between_sub_ticks < size_sub_tick_text) {
		float count = (std::max)(1.0f, std::round(size_sub_tick_text / distance_between_sub_ticks));
		float countTicks = std::round((float)timelineData->GetData().framerate / count);

		if (countTicks < 1.0f) {
			draw_sub_ticks = false;
		}
		else {
			float stepTick = 1.f / countTicks;
			step_Sub = stepTick;
			count_sub_tick = countTicks;
			distance_between_sub_ticks = timelineTransformHorizontal->getPostionScreen_lerped(stepTick) - timelineTransformHorizontal->getPostionScreen_lerped(0.f);
		}
	}


	int sub_text_step = 1;
	if (draw_sub_ticks && distance_between_sub_ticks > 0.001f && distance_between_sub_ticks < 52.f) {
		sub_text_step = std::ceil(52.f / distance_between_sub_ticks);
	}

	
	for (size_t i = begin_index_main_tick; i <= count_main_tick; i++) {

		double tick_main_value = std::round(double(i)*double(step_Main));

		if (long double(tick_main_value) > timelineData->GetData().totalTime + 0.0001l)
			break;

		float pos_main_tick = p_min.x + timelineTransformHorizontal->getPostionScreen_lerped(tick_main_value);

		ImVec2 min_pos_main_tick = { pos_main_tick - width_main_tick / 2.f, p_max.y - height_main_tick };
		ImVec2 max_pos_main_tick = { pos_main_tick + width_main_tick / 2.f, p_max.y };

		char buf[32];
		if (i == 0)
			snprintf(buf, sizeof(buf), "0s");
		else
			snprintf(buf, sizeof(buf), "%.0fs", tick_main_value);

		ImVec2 text_size = ImGui::CalcTextSize(buf, nullptr, true);

		if (max_pos_main_tick.x + distance_between_main_ticks < p_min.x) continue;
		if (min_pos_main_tick.x - text_size.x * 0.5f > p_max.x) break;


		float text_x = pos_main_tick - text_size.x * 0.5f;
		float text_y = min_pos_main_tick.y - text_size.y - 1.0f;


		drawlist->AddText(ImGui::GetFont(), ImGui::GetFontSize(), ImVec2(text_x, text_y), color_text_mainTick, buf);
		drawlist->AddRectFilled(min_pos_main_tick, max_pos_main_tick, color_mainTick, 2.f, 0.f);
		count_tick_rendered++;

		if (draw_sub_ticks) {

			int subs_per_main_step = std::round(step_Main / step_Sub);

			for (int j = 1; j <= subs_per_main_step; j++) {

				double tick_sub_value = tick_main_value + (double(j) * double(step_Sub));

				if (abs(tick_sub_value - tick_main_value) < 0.000001f || abs(tick_sub_value - tick_main_value - step_Main) < 0.000001f) continue;


				if (long double(tick_sub_value) > timelineData->GetData().totalTime + 0.0001l)
					break;

				float pos_sub_tick = p_min.x + timelineTransformHorizontal->getPostionScreen_lerped(tick_sub_value);


				bool isMajorTick = (j % sub_text_step == 0) && (subs_per_main_step / sub_text_step > 1);

				ImVec2 min_pos_sub_tick = { pos_sub_tick - width_sub_tick / 2.f, p_max.y - (isMajorTick ? height_sub_major_tick : height_sub_minor_tick) };
				ImVec2 max_pos_sub_tick = { pos_sub_tick + width_sub_tick / 2.f, p_max.y };

				if (max_pos_sub_tick.x < p_min.x || min_pos_sub_tick.x > p_max.x) continue;

				if (isMajorTick) {
					int actual_frame = std::round((tick_sub_value - tick_main_value) * timelineData->GetData().framerate);

					char buf[32];
					snprintf(buf, sizeof(buf), "%if", actual_frame);
					ImVec2 sub_text_size = ImGui::CalcTextSize(buf, nullptr, true); 
					float text_x = pos_sub_tick - sub_text_size.x * 0.5f; 
					float text_y = min_pos_sub_tick.y - sub_text_size.y - 1.0f; 
					drawlist->AddText(ImGui::GetFont(), ImGui::GetFontSize(), ImVec2(text_x, text_y), color_text_subTick, buf);
				}


				drawlist->AddRectFilled(min_pos_sub_tick, max_pos_sub_tick, color_subTick, 2.f, 0.f);
				count_tick_rendered++;
			}
		}

	}
	*/






	ImColor lerpedColor_reaction = color_reaction_overScale;
	lerpedColor_reaction.Value.w = timelineTransformHorizontal->getReactionToOverScale() * 0.5f;

	drawlist->AddRectFilled(
		p_min,
		p_max,
		lerpedColor_reaction,
		0.f,
		0.f
	);


	//DebugOverlay::AddText("tick rendered: " + std::to_string(count_tick_rendered), ImColor(1.f, 0.3f, 0.3f, 1.f));

	timeCursor->Draw(p_min, p_max);

}

void Timeline_Ruler::UpdateVars(const float& dt) {
	timelineTransformHorizontal->updateAvailableWidth(region_max.x);

	float mouse_real_screen = ImGui::GetMousePos().x;
	float mouse_fixed = mouse_real_screen - cursor_pos.x;
	mouse_global = timelineTransformHorizontal->getPostionGlobal_real(mouse_fixed);
	mouse_screen = timelineTransformHorizontal->getPostionScreen_real(mouse_global);


	float offsetOutOfBound = timelineTransformHorizontal->getOffset_OutOfBounds();

	const float valueForOutBounds = 250.f;

	if (offsetOutOfBound > valueForOutBounds)
		offsetOutOfBound = valueForOutBounds;

	if (offsetOutOfBound < -valueForOutBounds)
		offsetOutOfBound = -valueForOutBounds;

	offsetOutOfBound /= valueForOutBounds;

	time_left_bound.Set(offsetOutOfBound >= 0.f ?   offsetOutOfBound : 0.f);
	time_right_bound.Set(offsetOutOfBound <= 0.f ? -offsetOutOfBound : 0.f);


	time_left_bound.Update(dt * 8.f);
	time_right_bound.Update(dt * 8.f);

}
void Timeline_Ruler::UpdateZoom() {
	if (!isHovered) return;
	
	if (scrol_zoom > 0.f) {
		timelineTransformHorizontal->zoomToPointByValue(mouse_global, 1.5f, false);
	}
	else if (scrol_zoom < 0.f) {
		timelineTransformHorizontal->zoomToPointByValue(mouse_global, 0.75f, false);
	}
}

void Timeline_Ruler::FitAllTimeline() {
	timelineTransformHorizontal->fitToRanges(-0.1, timelineData->GetData().totalTime + 0.1);
}


const float& Timeline_Ruler::getGlobalMouse() {
	return mouse_global;
}
const float& Timeline_Ruler::getScreenMouse() {
	return mouse_screen;
}


void Timeline_Ruler::Update(const float& dt) {
	UpdateVars(dt);
	UpdateZoom();


	if (isHovered)
		CursorManager::getInstance().SetCursor(TypeCursor::HAND_OPEN);
	if (isDraging)
		CursorManager::getInstance().SetCursor(TypeCursor::HAND_CLOSED);


	static bool isOfsetDeltaApplied = false;

	if (isDraging && ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
		isOfsetDeltaApplied = true;
		float delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left).x;
		timelineTransformHorizontal->setOffsetDelta(delta);
	}
	else {
		isDraging = false;
		if (isOfsetDeltaApplied)
			timelineTransformHorizontal->applyOffsetDelta();
		isOfsetDeltaApplied = false;
	}

	if (!isHovered) return;

	if (ImGui::IsMouseClicked(ImGuiMouseButton_Left, false)) {
		isDraging = true;
	}


	if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left, false)) {
		timelineTransformHorizontal->fitToRanges(-0.1, timelineData->GetData().totalTime + 0.1);
	}

	if (ImGui::IsMouseDown(ImGuiMouseButton_Right, false)) {

		float step_frame = 1.f / (float)timelineData->GetData().framerate;
		float count = std::floor(mouse_global / step_frame);
		float time = count * step_frame + 0.000001f;

		if (ImGui::IsKeyDown(ImGuiKey_LeftShift))
			time = mouse_global;

		timelineData->GetData().SetTime(
			long double(time),
			timelineController->GetState() == TimelineRunState::RUNNING_CYCLE
		);

		CursorManager::getInstance().SetCursor(TypeCursor::POINT);
	}

}