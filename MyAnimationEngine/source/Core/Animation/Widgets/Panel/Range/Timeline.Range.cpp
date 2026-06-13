#include "Timeline.Range.h"
#include "CursorManager/CursorManger.h"
#include <algorithm>

Timeline_Range::Timeline_Range(
	Timeline_TimeCursor* timeCursor,
	TimelineProviderData* timelineData,
	TimelineTransformHorizontal* horizontalTransform)
	: timeCursor(timeCursor),
	timelineData(timelineData),
	horizontalTransform(horizontalTransform)
{

}

Timeline_Range::~Timeline_Range() {

}

void Timeline_Range::Draw() {
	auto drawlist = ImGui::GetWindowDrawList();
	ImVec2 cursor_pos = ImGui::GetCursorScreenPos();
	ImVec2 region_max = ImGui::GetContentRegionAvail();

	
	ImVec2 p_min = cursor_pos;
	ImVec2 p_max = { cursor_pos.x + region_max.x, cursor_pos.y + region_max.y };


	ImGui::InvisibleButton("Timeline_Range_canvas_button", region_max, ImGuiButtonFlags_MouseButtonLeft);


	float left_range  = horizontalTransform->getPostionScreen_lerped(timelineData->GetData().left_range_repeat);
	float right_range = horizontalTransform->getPostionScreen_lerped(timelineData->GetData().right_range_repeat);


	ImVec2 range_box_min = { cursor_pos.x + left_range, cursor_pos.y };
	ImVec2 range_box_max = { cursor_pos.x + right_range, cursor_pos.y + region_max.y };

	ImVec2 left_button_min = { range_box_min.x - button_width, range_box_min.y };
	ImVec2 left_button_max = { range_box_min.x, range_box_max.y };

	ImVec2 right_button_min = { range_box_max.x, range_box_min.y };
	ImVec2 right_button_max = { range_box_max.x + button_width, range_box_max.y };

	ImVec2 mouse = ImGui::GetMousePos();

	ImVec2 mouseScreen = { mouse.x - cursor_pos.x, mouse.y - cursor_pos.y };

	float global_pos_mouse_x = horizontalTransform->getPostionGlobal_lerped(mouseScreen.x);


	float left_hitbox_min_x = range_box_min.x - button_width / 2.f - hitbox_width / 2.f;
	float left_hitbox_max_x = range_box_min.x - button_width / 2.f + hitbox_width / 2.f;

	float right_hitbox_min_x = range_box_max.x + button_width / 2.f - hitbox_width / 2.f;
	float right_hitbox_max_x = range_box_max.x + button_width / 2.f + hitbox_width / 2.f;

	float range_hitbox_min_x = range_box_min.x + hitbox_width / 2.f;
	float range_hitbox_max_x = range_box_max.x - hitbox_width / 2.f;


	ImColor color_left_button = color_button;
	ImColor color_right_button = color_button;
	ImColor color_range_button = color_range;



	bool need_check_hover = !(left_range_dragging || right_range_dragging || range_dragging);
	bool mouse_y_in_box = (mouse.y >= range_box_min.y && mouse.y <= range_box_max.y);

	bool mouse_x_in_left_button = mouse.x >= left_hitbox_min_x && mouse.x <= left_hitbox_max_x;
	bool mouse_x_in_right_button = mouse.x >= right_hitbox_min_x && mouse.x <= right_hitbox_max_x;
	bool mouse_x_in_range_button = mouse.x > range_hitbox_min_x && mouse.x < range_hitbox_max_x;

	if (need_check_hover && mouse_y_in_box) {



		if (mouse_x_in_left_button) {
			color_left_button = color_button_hovered;

			if (ImGui::IsMouseClicked(ImGuiMouseButton_Left, false)) {
				left_range_dragging = true;
			}

		}
		else if (mouse_x_in_right_button) {
			color_right_button = color_button_hovered;

			if (ImGui::IsMouseClicked(ImGuiMouseButton_Left, false)) {
				right_range_dragging = true;
			}
		}
		else if (mouse_x_in_range_button) {
			color_range_button = color_range_hovered;

			if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
				timelineData->GetData().left_range_repeat = 0.f;
				timelineData->GetData().right_range_repeat = timelineData->GetData().totalTime;
			}
			else if (ImGui::IsMouseClicked(ImGuiMouseButton_Left, false)) {
				range_dragging = true;


				 left_range_before_dragging =  timelineData->GetData().left_range_repeat;
				right_range_before_dragging = timelineData->GetData().right_range_repeat;

				length_range_dragging = timelineData->GetData().right_range_repeat - timelineData->GetData().left_range_repeat;
				global_pos_begin_draging = global_pos_mouse_x;

			}
		}
	}





	if (left_range_dragging) {
		if (ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
			color_left_button = color_button_down;
			timelineData->GetData().SetLeftRange(global_pos_mouse_x);
		}
		else left_range_dragging = false;
	}
	else if (right_range_dragging) {
		if (ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
			color_right_button = color_button_down;
			timelineData->GetData().SetRightRange(global_pos_mouse_x);
		}
		else right_range_dragging = false;
	}
	else if (range_dragging) {
		if (ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
			color_range_button = color_range_down;

			float delta_mouse = global_pos_mouse_x - global_pos_begin_draging;

			float condidate_left = left_range_before_dragging + delta_mouse;
			float condidate_right = right_range_before_dragging + delta_mouse;


			if (condidate_left < 0.f) {
				condidate_left = 0.f;
				condidate_right = length_range_dragging;
			}

			if (condidate_right > timelineData->GetData().totalTime) {
				condidate_left = timelineData->GetData().totalTime - length_range_dragging;
				condidate_right = timelineData->GetData().totalTime;
			}

			timelineData->GetData().SetLeftRange(condidate_left);
			timelineData->GetData().SetRightRange(condidate_right);



		}
		else range_dragging = false;
	}

	bool isHovered = (mouse_x_in_left_button || mouse_x_in_right_button || mouse_x_in_range_button) && mouse_y_in_box;
	bool isResizing = left_range_dragging || right_range_dragging;
	bool isDraging  = range_dragging;

	if (isHovered)
		CursorManager::getInstance().SetCursor(TypeCursor::HAND_OPEN);
	if (isResizing)
		CursorManager::getInstance().SetCursor(TypeCursor::RESIZE_HORIZONTAL);
	if(isDraging)
		CursorManager::getInstance().SetCursor(TypeCursor::HAND_CLOSED);





	drawlist->AddRectFilled(left_button_min, left_button_max, color_left_button, button_width / 2.f, ImDrawFlags_RoundCornersLeft);
	drawlist->AddRectFilled(right_button_min, right_button_max, color_right_button, button_width / 2.f, ImDrawFlags_RoundCornersRight);

	drawlist->AddRectFilled(range_box_min, range_box_max, color_range_button, 0.f);


	if (range_dragging) {

		float delta_end = timelineData->GetData().left_range_repeat + (global_pos_begin_draging - left_range_before_dragging);


		float screen_begin_pos_drag = horizontalTransform->getPostionScreen_lerped(global_pos_begin_draging);
		float screen_end_pos_drag   = horizontalTransform->getPostionScreen_lerped(delta_end);


		float begin = cursor_pos.x + (std::min)(screen_begin_pos_drag, screen_end_pos_drag);
		float end   = cursor_pos.x + (std::max)(screen_begin_pos_drag, screen_end_pos_drag);


		float height = range_box_max.y - range_box_min.y;


		ImVec2 pos_debug_drag_line_min = { begin, cursor_pos.y + height / 2.f - 3.f };
		ImVec2 pos_debug_drag_line_max = { end,   cursor_pos.y + height / 2.f + 3.f };


		drawlist->AddRectFilled(pos_debug_drag_line_min, pos_debug_drag_line_max, ImColor(1.f, 1.f, 0.f, 0.7f), 8.f);

	}



	timeCursor->Draw(p_min, p_max);


}

void Timeline_Range::Update() {





}