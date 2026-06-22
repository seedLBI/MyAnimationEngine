#include "Timeline.Scroll.Vertical.h"

#include "Utils/ImGui/Utils.ImGui.h"
#include "Core/Animation/Widgets/Panel/Colors/Panel.Colors.h"
#include "CursorManager/CursorManger.h"

Timeline_Scroll_Vertical::Timeline_Scroll_Vertical(
	LayersDataProvider* layersData,
	TimelineTransformVertical* verticalTransform)
	: layersData(layersData),
	verticalTransform(verticalTransform)
{

}

Timeline_Scroll_Vertical::~Timeline_Scroll_Vertical() {

}

void Timeline_Scroll_Vertical::Draw() {
	ImDrawList* draw_list = ImGui::GetWindowDrawList();

	ImVec2 region_max = ImGui::GetContentRegionAvail();
	ImVec2 cursor_pos = ImGui::GetCursorScreenPos();

	ImVec2 p_min = cursor_pos;
	ImVec2 p_max = { cursor_pos.x + region_max.x, cursor_pos.y + region_max.y };

	float max_radius = region_max.x / 3.5f;

	ImVec2 min_FullSlider = { p_min.x + region_max.x / 2.f - max_radius, p_min.y + 5.f };
	ImVec2 max_FullSlider = { p_min.x + region_max.x / 2.f + max_radius, p_max.y - 5.f };

	float height_slider = max_FullSlider.y - min_FullSlider.y;

	float koef_height = verticalTransform->getAvailableHeight() / verticalTransform->getContentHeight();
	koef_height = std::clamp(koef_height, 0.f, 1.f);

	float height_user_slider = height_slider * koef_height;

	float koef_offset = -verticalTransform->getOffset_lerped() / verticalTransform->getContentHeight();
	koef_offset = std::clamp(koef_offset,0.f,1.f);

	float offset = height_slider * koef_offset;

	ImVec2 min_UserSlider = { p_min.x + region_max.x / 2.f - max_radius,	p_min.y + 5.f + offset };
	ImVec2 max_UserSlider = { p_min.x + region_max.x / 2.f + max_radius,	p_min.y + 5.f + offset + height_user_slider };

	bool hovered_user_slider, held_user_slider;
	bool pressed_user_slider = MyButtonBehavior(
		min_UserSlider,
		max_UserSlider,
		"GRAB_VERTICAL_SLIDER_OFFSET",
		hovered_user_slider, held_user_slider
	);

	ImColor color_slider = COLOR_DEFAULT;
	if (hovered_user_slider)
		CursorManager::getInstance().SetCursor(TypeCursor::HAND_OPEN);
	if (held_user_slider)
		CursorManager::getInstance().SetCursor(TypeCursor::HAND_CLOSED);

	if (hovered_user_slider || held_user_slider)
		color_slider = COLOR_DEFAULT_HOVERED;


	if (ImGui::IsMouseHoveringRect(p_min, p_max)) {
		float scrol_zoom = ImGui::GetIO().MouseWheel;
		verticalTransform->addOffset(scrol_zoom * verticalTransform->getScaledHeightOneObject());
	}

	if (held_user_slider) {
		float delta = ImGui::GetIO().MouseDelta.y;
		float delta_to_add = delta / koef_height;
		verticalTransform->addOffset(-delta_to_add);
	}




	draw_list->AddRectFilled(min_FullSlider, max_FullSlider, COLOR_BG_BUTTONS_HOVERED, max_radius);
	draw_list->AddRectFilled(min_UserSlider, max_UserSlider, color_slider, max_radius);
}

void Timeline_Scroll_Vertical::Update() {

}
