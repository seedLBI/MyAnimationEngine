#include "Timeline.Scroll.Horizontal.h"

#include "ThirdParty/ImGui/imgui.h"
#include "ThirdParty/ImGui/imgui_internal.h"
#include "CursorManager/CursorManger.h"
#include <iostream>
#include <algorithm>

#include "Core/DebugOverlay/DebugOverlay.h"

Timeline_Scroll_Horizontal::Timeline_Scroll_Horizontal(
	TimelineProviderData* timelineData,
	TimelineTransformHorizontal* horizontalTransform)
	: timelineData(timelineData),
	horizontalTransform(horizontalTransform)
{

}

Timeline_Scroll_Horizontal::~Timeline_Scroll_Horizontal()
{
}


void Timeline_Scroll_Horizontal::Draw() {

	ImDrawList* draw_list = ImGui::GetWindowDrawList();

	ImVec2 region_max = ImGui::GetContentRegionAvail();
	ImVec2 cursor_pos = ImGui::GetCursorScreenPos();

	ImVec2 p_min = cursor_pos;
	ImVec2 p_max = { cursor_pos.x + region_max.x, cursor_pos.y + region_max.y };


	ImVec2 p = p_min;

	float offset = 10.f;

	float padding = 8.f;

	p.x += offset;

	float min_radius = region_max.y / 7.f;
	float max_radius = region_max.y / 3.5f;

	p.x += min_radius;
	draw_list->AddCircleFilled({ p.x, p.y + region_max.y / 2.f }, min_radius, ImColor(0.69f, 0.69f, 0.69f, 1.f));
	p.x += min_radius ;


	p.x += padding;

	float length_line = 100.f;



	float currentScale	= horizontalTransform->getScale().GetLerped();
	float minScale		= horizontalTransform->getMinScale();
	float maxScale		= horizontalTransform->getMaxScale();

	float t = (currentScale - minScale)/(maxScale - minScale);


	// linearize
	const double a = 0.000670541;
	const double r = 1.5; 
	t = (std::log((t * (r - 1.0) / a) + 1.0) / std::log(r)) * (1.f)/(16.32f);


	float pos_x_current_scale = p.x + 5.f + t * (length_line-10.f);

	float min_pos_x_scale = p.x + 5.f;
	float max_pos_x_scale = p.x + 5.f + length_line - 10.f;

	float radius_drag = region_max.y / 3.6f;


	draw_list->AddLine({ p.x, p.y + region_max.y / 2.f }, { p.x + length_line,  p.y + region_max.y / 2.f }, ImColor(.29f, .29f, .29f, 1.f),2.5f);


	ImRect bb_slider_zoom({ pos_x_current_scale - radius_drag * 1.7f, p.y }, { pos_x_current_scale + radius_drag * 1.7f,  p.y + region_max.y });
	ImGuiID id_slider_zoom = ImGui::GetID("GRAB_SLIDER_ZOOM");
	bool hovered_slider_zoom, held_slider_zoom;
	ImGui::ItemAdd(bb_slider_zoom, id_slider_zoom);
	ImGui::ButtonBehavior(bb_slider_zoom, id_slider_zoom, &hovered_slider_zoom, &held_slider_zoom);

	float value_thinkness = 1.6f;

	ImColor hovered_color = ImColor(0.89f, 0.89f, 0.89f, 1.f);
	ImColor default_color = ImColor(0.69f, 0.69f, 0.69f, 1.f);

	ImColor current_color =  default_color;

	if (hovered_slider_zoom || held_slider_zoom) {
		CursorManager::getInstance().SetCursor(TypeCursor::HAND_OPEN);
		value_thinkness = 2.f;
		current_color = hovered_color;
	}

	if (held_slider_zoom) {
		CursorManager::getInstance().SetCursor(TypeCursor::HAND_CLOSED);

		float x_pos_mouse = ImGui::GetIO().MousePos.x;

		float t_l = (x_pos_mouse - min_pos_x_scale) / (max_pos_x_scale - min_pos_x_scale);
		t_l = std::clamp(t_l, 0.f, 1.f);

		float t_linear = static_cast<float>((a / (r - 1.0)) * (std::pow(r, t_l * 16.32f) - 1.0));
		t_linear = std::clamp(t_linear, 0.f, 1.f);

		float new_scale = minScale + t_linear * (maxScale - minScale);



		float center = 0.5f * (horizontalTransform->getLeftBoundView_global_real() + horizontalTransform->getRightBoundView_global_real());

		horizontalTransform->scaleZoomBy( new_scale / horizontalTransform->getScale().GetReal());
		horizontalTransform->centerize(center);

	}

	draw_list->AddCircleFilled({ pos_x_current_scale, p.y + region_max.y / 2.f }, radius_drag * 1.7f, ImColor(ImGui::GetStyleColorVec4(ImGuiCol_WindowBg)));
	draw_list->AddCircleFilled({ pos_x_current_scale, p.y + region_max.y / 2.f }, radius_drag, current_color);
	draw_list->AddCircleFilled({ pos_x_current_scale, p.y + region_max.y / 2.f }, radius_drag/ value_thinkness, ImColor(ImGui::GetStyleColorVec4(ImGuiCol_WindowBg)));
	p.x += length_line;
	p.x += padding;

	p.x += max_radius ;
	draw_list->AddCircleFilled({ p.x, p.y + region_max.y / 2.f }, max_radius, ImColor(.69f, .69f, .69f, 1.f));
	p.x += max_radius;

	p.x += padding * 2.f;


	float full_length_slider = region_max.x - (p.x - p_min.x) - padding;

	float min_offset = horizontalTransform->getMinOffset();
	float max_offset = horizontalTransform->getMaxOffset();

	float offset_lerped = horizontalTransform->getOffset().GetLerped();

	float full_length_offset = abs(max_offset - min_offset)*3.f;
	float visible_length_offset = horizontalTransform->getAvailableWidth();

	if (visible_length_offset > full_length_offset)
		visible_length_offset = full_length_offset;

	float t_width = sqrt((visible_length_offset / full_length_offset));
	float width_slider = t_width * full_length_slider;

	width_slider = (std::min)((std::max)(width_slider, full_length_slider / 25.f), full_length_slider);

	float curr_pos = (offset_lerped - min_offset)/(max_offset - min_offset);
	float offset_slider_x = curr_pos * (full_length_slider - width_slider);

	ImRect bb_slider_offset({ p.x + offset_slider_x, p.y + region_max.y / 2.f - max_radius }, { p.x + width_slider + offset_slider_x, p.y + region_max.y / 2.f + max_radius });
	ImGuiID id_slider_offset = ImGui::GetID("GRAB_SLIDER_OFFSET");
	bool hovered_slider_offset, held_slider_offset;
	ImGui::ItemAdd(bb_slider_offset, id_slider_offset);
	ImGui::ButtonBehavior(bb_slider_offset, id_slider_offset, &hovered_slider_offset, &held_slider_offset);

	ImColor slider_offset_current_color = default_color;

	ImVec2 min_FullSlider = { p.x, p.y + region_max.y / 2.f - max_radius };
	ImVec2 max_FullSlider = { p.x + full_length_slider, p.y + region_max.y / 2.f + max_radius };

	ImVec2 min_UserSlider = { p.x + offset_slider_x, p.y + region_max.y / 2.f - max_radius };
	ImVec2 max_UserSlider = { p.x + width_slider + offset_slider_x, p.y + region_max.y / 2.f + max_radius };

	if (hovered_slider_offset || held_slider_offset) {
		CursorManager::getInstance().SetCursor(TypeCursor::HAND_OPEN);
		slider_offset_current_color = hovered_color;
	}

	if (held_slider_offset || ImGui::IsMouseHoveringRect({ p.x, p.y + region_max.y / 2.f - max_radius }, { p.x + full_length_slider, p.y + region_max.y / 2.f + max_radius })) {
		float scrol_zoom = ImGui::GetIO().MouseWheel;
		if (scrol_zoom != 0.f) {
			float center = 0.5f * (horizontalTransform->getLeftBoundView_global_real() + horizontalTransform->getRightBoundView_global_real());

			float mult = scrol_zoom > 0.f ? 1.5f : 0.75f;
			horizontalTransform->zoomToPointByValue(center, mult, false);
		}
	}

	if (held_slider_offset) {
		CursorManager::getInstance().SetCursor(TypeCursor::HAND_CLOSED);

		float mouse_delta_x = ImGui::GetIO().MouseDelta.x;

		float pixels_for_full_range = full_length_slider * (1.f - t_width);
		float delta_offset = (mouse_delta_x / pixels_for_full_range) * (max_offset - min_offset);

		float actualOffset = horizontalTransform->getOffset().GetReal();
		float newOffset = actualOffset + delta_offset;

		horizontalTransform->setOffsetWithClamp(newOffset);
	}

	draw_list->AddRectFilled(min_FullSlider,max_FullSlider,ImColor(.19f, .19f, .19f, 1.f), max_radius);
	draw_list->AddRectFilled(min_UserSlider,max_UserSlider,slider_offset_current_color,max_radius);

}

void Timeline_Scroll_Horizontal::Update() {

}