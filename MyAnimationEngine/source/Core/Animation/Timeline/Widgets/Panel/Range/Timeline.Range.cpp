#include "Timeline.Range.h"

Timeline_Range::Timeline_Range(
	TimelineProviderData* timelineData,
	TimelineTransformHorizontal* horizontalTransform)
	: timelineData(timelineData),
	horizontalTransform(horizontalTransform)
{

}

Timeline_Range::~Timeline_Range() {

}

void Timeline_Range::Draw() {
	auto drawlist = ImGui::GetWindowDrawList();
	ImVec2 cursor_pos = ImGui::GetCursorScreenPos();
	ImVec2 region_max = ImGui::GetContentRegionAvail();

	ImGui::InvisibleButton("Timeline_Range_canvas_button", region_max, ImGuiButtonFlags_MouseButtonLeft);


	float left_range  = horizontalTransform->getPostionScreen_lerped(timelineData->GetData().left_range_repeat);
	float right_range = horizontalTransform->getPostionScreen_lerped(timelineData->GetData().right_range_repeat);


	ImVec2 range_box_min = { cursor_pos.x + left_range, cursor_pos.y };
	ImVec2 range_box_max = { cursor_pos.x + right_range, cursor_pos.y + region_max.y };

	ImVec2 left_button_min = { range_box_min.x - button_width, range_box_min.y };
	ImVec2 left_button_max = { range_box_min.x + button_width, range_box_max.y };

	ImVec2 right_button_min = { range_box_max.x - button_width, range_box_min.y };
	ImVec2 right_button_max = { range_box_max.x + button_width, range_box_max.y };

	ImVec2 mouse = ImGui::GetMousePos();

	ImVec2 mouseScreen = { mouse.x - cursor_pos.x, mouse.y - cursor_pos.y };


	float left_hitbox_min_x = range_box_min.x - button_width / 2.f - hitbox_width / 2.f;
	float left_hitbox_max_x = range_box_min.x - button_width / 2.f + hitbox_width / 2.f;


	float right_hitbox_min_x = range_box_max.x + button_width / 2.f - hitbox_width / 2.f;
	float right_hitbox_max_x = range_box_max.x + button_width / 2.f + hitbox_width / 2.f;


	ImColor color_left_button = color_button;
	ImColor color_right_button = color_button;


	if (mouse.y >= range_box_min.y && mouse.y <= range_box_max.y) {

		if (mouse.x >= left_hitbox_min_x && mouse.x <= left_hitbox_max_x) {
			color_left_button = color_button_hovered;


			if (ImGui::IsMouseClicked(ImGuiMouseButton_Left, false)) {
				left_range_dragging = true;
			}

		}
		else if (mouse.x >= right_hitbox_min_x && mouse.x <= right_hitbox_max_x) {
			color_right_button = color_button_hovered;

			if (ImGui::IsMouseClicked(ImGuiMouseButton_Left, false)) {
				right_range_dragging = true;
			}


		}

	}

	if (left_range_dragging) {
		if (ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
			color_left_button = color_button_down;
			timelineData->GetData().SetLeftRange(horizontalTransform->getPostionGlobal_lerped(mouseScreen.x));
		}
		else left_range_dragging = false;
	}
	else if (right_range_dragging) {
		if (ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
			color_right_button = color_button_down;
			timelineData->GetData().SetRightRange(horizontalTransform->getPostionGlobal_lerped(mouseScreen.x));
		}
		else right_range_dragging = false;
	}




	drawlist->AddRectFilled(left_button_min, left_button_max, color_left_button, button_width / 2.f);
	drawlist->AddRectFilled(right_button_min, right_button_max, color_right_button, button_width / 2.f);

	drawlist->AddRectFilled(range_box_min, range_box_max, color_range, 0.f);

}

void Timeline_Range::Update() {





}