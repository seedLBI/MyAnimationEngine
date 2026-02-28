#include "Timeline.Ruler.h"


#include "Core/DebugOverlay/DebugOverlay.h"

Timeline_Ruler::Timeline_Ruler(
	TimelineController* timelineController,
	TimelineProviderData* timelineData, 
	TimelineTransformHorizontal* timelineTransformHorizontal) 
	: timelineController(timelineController),
	  timelineData(timelineData),
	  timelineTransformHorizontal(timelineTransformHorizontal)
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

	float distance_between_Main_tick = timelineTransformHorizontal->getLengthTick_lerped();
	int countMainTick_for_Render = region_max.x / distance_between_Main_tick + 1.f;

	float height_main_tick = ImGui::GetTextLineHeight();
	float height_sub_minor_tick = ImGui::GetTextLineHeight() / 3.f;
	float height_sub_major_tick = ImGui::GetTextLineHeight() / 2.f;

	float width_main_tick = 5.f;


	ImVec2 block_min = cursor_pos;
	ImVec2 block_max = { cursor_pos.x + timelineTransformHorizontal->getPostionScreen_lerped(0.f), cursor_pos.y + region_max.y };

	if (OutOfView(block_max) == false || block_max.x > p_max.x) {

		drawlist->AddRectFilled(
			block_min,
			block_max,
			color_bounds,
			0.f,
			0.f
		);
	}




	block_min = { cursor_pos.x + timelineTransformHorizontal->getPostionScreen_lerped(timelineData->GetData().totalTime), cursor_pos.y };
	block_max = { cursor_pos.x + region_max.x, cursor_pos.y + region_max.y };

	if (OutOfView(block_min) == false || block_min.x < p_max.x) {
		drawlist->AddRectFilled(
			block_min,
			block_max,
			color_bounds,
			0.f,
			0.f
		);
	}


	int count_tick_rendered = 0;


	const float distance_between_ticks = timelineTransformHorizontal->getPostionScreen_lerped(1.f) - timelineTransformHorizontal->getPostionScreen_lerped(0.f);



	int count_main_tick = timelineData->GetData().totalTime;

	int begin_index_tick = timelineTransformHorizontal->getLeftBoundView_global();
	if (begin_index_tick < 0) begin_index_tick = 0;



	for (size_t i = begin_index_tick; i <= count_main_tick; i++) {


		double tick_value = double(i);

		if (long double(tick_value) > timelineData->GetData().totalTime + 0.0001l)
			break;

		float pos_main_tick = p_min.x + timelineTransformHorizontal->getPostionScreen_lerped(tick_value);



		ImVec2 min_pos_main_tick = { pos_main_tick - width_main_tick / 2.f, p_max.y - height_main_tick };
		ImVec2 max_pos_main_tick = { pos_main_tick + width_main_tick / 2.f, p_max.y };



		char buf[32];
		if (i == 0)
			snprintf(buf, sizeof(buf), "0");
		else
			snprintf(buf, sizeof(buf), "%.1f", tick_value);

		ImVec2 text_size = ImGui::CalcTextSize(buf, nullptr, true);
		float text_x = pos_main_tick - text_size.x * 0.5f;
		float text_y = min_pos_main_tick.y - text_size.y - 1.0f;

		ImU32 text_col = ImGui::GetColorU32(ImGuiCol_Text);
		drawlist->AddText(ImGui::GetFont(), ImGui::GetFontSize(), ImVec2(text_x, text_y), text_col, buf);




		if (max_pos_main_tick.x + distance_between_ticks < p_min.x) continue;
		if (min_pos_main_tick.x - text_size.x * 0.5f > p_max.x) break;


		drawlist->AddRectFilled(min_pos_main_tick, max_pos_main_tick, color_mainTick, 2.f, 0.f);
		count_tick_rendered++;


		for (size_t j = 1; j < timelineData->GetData().framerate; j++) {

			double sub_tick_value = double(j) / double(timelineData->GetData().framerate);
			double sum_tick_value = tick_value + sub_tick_value;

			if (long double(sum_tick_value) > timelineData->GetData().totalTime + 0.0001l)
				break;

			float pos_sub_tick = p_min.x + timelineTransformHorizontal->getPostionScreen_lerped(sum_tick_value);

			ImVec2 min_pos_sub_tick = { pos_sub_tick - width_main_tick / 3.f, p_max.y };
			ImVec2 max_pos_sub_tick = { pos_sub_tick + width_main_tick / 3.f, p_max.y };

			min_pos_sub_tick.y -= j % 2 == 0 ? height_sub_minor_tick : height_sub_major_tick;



			if (max_pos_sub_tick.x < p_min.x) continue;
			if (min_pos_sub_tick.x > p_max.x) break;


			drawlist->AddRectFilled(min_pos_sub_tick, max_pos_sub_tick, color_subTick, 2.f, 0.f);
			count_tick_rendered++;

		}


	}

	DebugOverlay::AddText("tick rendered: " + std::to_string(count_tick_rendered), ImColor(1.f, 0.3f, 0.3f, 1.f));


	float pos_timeline = p_min.x + timelineTransformHorizontal->getPostionScreen_lerped(timelineData->GetData().currentTime);

	float height_timeline = ImGui::GetTextLineHeight() * 2.f;

	ImVec2 min_pos_main_tick = { pos_timeline - width_main_tick / 2.5f, p_max.y - height_timeline };
	ImVec2 max_pos_main_tick = { pos_timeline + width_main_tick / 2.5f, p_max.y };


	ImColor color_main_tick_timeline = ImColor(1.f, 0.3f, 0.3f, 1.f);
	ImColor color_main_tick_timeline_transparent = ImColor(1.f, 0.3f, 0.3f, 0.f);

	drawlist->AddRectFilled(min_pos_main_tick, max_pos_main_tick, color_main_tick_timeline, 2.f, 0.f);

	/*
	
	min_pos_main_tick = { pos_timeline - width_main_tick * 6.f, p_max.y - height_timeline };
	max_pos_main_tick = { pos_timeline, p_max.y };

	drawlist->AddRectFilledMultiColor(
		min_pos_main_tick, max_pos_main_tick,
		color_main_tick_timeline_transparent, color_main_tick_timeline, color_main_tick_timeline, color_main_tick_timeline_transparent);

	*/



	//ImGui::EndChild();
}

#include <iostream>


void Timeline_Ruler::UpdateVars() {
	timelineTransformHorizontal->updateAvailableWidth(region_max.x);

	float mouse_real_screen = ImGui::GetMousePos().x;
	float mouse_fixed = mouse_real_screen - cursor_pos.x;
	mouse_global = timelineTransformHorizontal->getPostionGlobal_real(mouse_fixed);
	mouse_screen = timelineTransformHorizontal->getPostionScreen_real(mouse_global);

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

}


const float& Timeline_Ruler::getGlobalMouse() {
	return mouse_global;
}
const float& Timeline_Ruler::getScreenMouse() {
	return mouse_screen;
}


void Timeline_Ruler::Update() {
	UpdateVars();
	UpdateZoom();


	if (isDraging && ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
		float delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left).x;
		timelineTransformHorizontal->setOffsetDelta(delta);
	}
	else {
		isDraging = false;
		timelineTransformHorizontal->applyOffsetDelta();
	}

	if (!isHovered) return;

	if (ImGui::IsMouseClicked(ImGuiMouseButton_Left, false)) {
		isDraging = true;
	}


	if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left, false)) {
		timelineTransformHorizontal->fitToRanges(0.0, timelineData->GetData().totalTime);
	}

	if (ImGui::IsMouseDown(ImGuiMouseButton_Right, false)) {
		timelineData->GetData().SetTime(
			long double(mouse_global),
			timelineController->GetState() == TimelineRunState::RUNNING_CYCLE
		);
	}

	if (ImGui::IsKeyPressed(ImGuiKey_A)) {
		timelineTransformHorizontal->centerize(timelineData->GetData().totalTime/2.f);
	}
	


}