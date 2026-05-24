#include "Timeline.Layers.Viewer.h"
#include "CursorManager/CursorManger.h"
#include "Core/DebugOverlay/DebugOverlay.h"

Timeline_Layers_Viewer::Timeline_Layers_Viewer(
	Timeline_TimeCursor* timeCursor,
	LayersDataProvider* layersData,
	TimelineProviderData* timelineData,
	TimelineTransformVertical* verticalTransform,
	TimelineTransformHorizontal* horizontalTransform)
	: timeCursor(timeCursor),
	layersData(layersData),
	timelineData(timelineData),
	verticalTransform(verticalTransform),
	horizontalTransform(horizontalTransform)
{

}

Timeline_Layers_Viewer::~Timeline_Layers_Viewer() {

}


bool Timeline_Layers_Viewer::IsHovered_LeftResize(const ImRect& rect) {
	return ImGui::IsMouseHoveringRect(
		rect.Min, 
		{ rect.Min.x + width_hover, rect.Max.y }
	);
}
bool Timeline_Layers_Viewer::IsHovered_RightResize(const ImRect& rect) {
	return ImGui::IsMouseHoveringRect(
		{ rect.Max.x - width_hover, rect.Min.y }, 
		{ rect.Max.x + width_hover, rect.Max.y }
	);
}
bool Timeline_Layers_Viewer::IsHovered_BodyMove(const ImRect& rect) {

	ImVec2 min = { rect.Min.x + width_hover, rect.Min.y };
	ImVec2 max = { rect.Max.x - width_hover, rect.Max.y };

	return ImGui::IsMouseHoveringRect( min,max );
}

void Timeline_Layers_Viewer::DrawLayerBox(const ImRect& rect, const LayerInfo& info, const bool& hovered) {
	ImDrawList* draw_list = ImGui::GetWindowDrawList();

	colorRGB outline;
	colorRGB body = info.color;

	if (getLuminance(info.color) > 0.6f)
		outline = lerp(info.color, colorRGB{ 0,0,0 }, 0.55f);
	else
		outline = lerp(info.color, colorRGB{ 1,1,1 }, 0.45f);

	if (hovered) {
		outline = lerp(outline, colorRGB{ 1,1,1 }, 0.35f);
		body = lerp(body, colorRGB{ 1,1,1 }, 0.25f);
	}

	draw_list->AddRectFilled(rect.Min, rect.Max, ImColor(body.r, body.g, body.b, 1.f), 0.f, 0);
	draw_list->AddRect(rect.Min, rect.Max, ImColor(outline.r, outline.g, outline.b, 1.f), 0.f, 0, 2.f);

}


void Timeline_Layers_Viewer::Draw() {
	ImVec2 ASize = ImGui::GetWindowSize();

	ImVec2 SizeBody{ ASize.x, ASize.y - 1 };



	auto& layers = layersData->GetData().layers;

	ImVec2 region_max = ImGui::GetContentRegionAvail();
	ImVec2 cursor_pos = ImGui::GetCursorScreenPos();

	ImVec2 mouse_pos = { ImGui::GetMousePos().x - cursor_pos.x,ImGui::GetMousePos().y - cursor_pos.y };



	ImVec2 p_min = cursor_pos;
	ImVec2 p_max = { cursor_pos.x + region_max.x, cursor_pos.y + region_max.y };


	float itemHeight = verticalTransform->getScaledHeightOneObject();
	float width_window = ImGui::GetWindowWidth();


	float left_range  = horizontalTransform->getPostionScreen_lerped(timelineData->GetData().left_range_repeat);
	float right_range = horizontalTransform->getPostionScreen_lerped(timelineData->GetData().right_range_repeat);

	ImDrawList* draw_list = ImGui::GetWindowDrawList();


	ImVec2 p = p_min;

	for (size_t i = 0; i < layers.size(); i++) {

		auto layer = layers[i].get();
		auto& layerInfo = layer->GetLayerInfo();

		float time_begin	= layerInfo.time.Begin;
		float time_end		= layerInfo.time.End;

		float screen_time_begin = horizontalTransform->getPostionScreen_lerped(time_begin);
		float screen_time_end	= horizontalTransform->getPostionScreen_lerped(time_end);

		ImVec2 rect_min = { p.x + screen_time_begin, p.y };
		ImVec2 rect_max = { p.x + screen_time_end,	 p.y + itemHeight };

		ImRect rect = ImRect{ rect_min, rect_max };


		ImGui::SetCursorScreenPos({ rect_min.x - width_hover,rect_min.y});
		ImGui::InvisibleButton(("Rect_LAYER##" + std::to_string(i)).c_str(), { rect.GetWidth() + width_hover * 2.f, rect.GetHeight()});

		bool hovered = ImGui::IsItemHovered();
		bool active = ImGui::IsItemActive();

		DrawLayerBox(rect, layerInfo, hovered || active);



		ObjectStatus isHovered = ObjectStatus::NOTHING;
		if (IsHovered_LeftResize(rect))
			isHovered = ObjectStatus::LEFT_RESIZE;
		else if (IsHovered_RightResize(rect))
			isHovered = ObjectStatus::RIGHT_RESIZE;
		else if (IsHovered_BodyMove(rect))
			isHovered = ObjectStatus::BODY_MOVE;


		if (ImGui::IsMouseDown(ImGuiMouseButton_Left) == false) {
			switch (isHovered) {
			case ObjectStatus::LEFT_RESIZE:
			case ObjectStatus::RIGHT_RESIZE:
				CursorManager::getInstance().SetCursor(TypeCursor::RESIZE_HORIZONTAL);
				break;
			case ObjectStatus::BODY_MOVE:
				CursorManager::getInstance().SetCursor(TypeCursor::HAND_OPEN);
				break;
			}
		}


		if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left) && isHovered == ObjectStatus::BODY_MOVE)
			horizontalTransform->fitToRanges(time_begin - 0.1f, time_end + 0.1f);
		else if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
			pressed_status = isHovered;

			if (pressed_status != ObjectStatus::NOTHING) {
				index_drag = i;
				drag_start_time_begin = time_begin;
				length_drag = time_end - time_begin;
				drag_start_mouse_global = horizontalTransform->getPostionGlobal_lerped(ImGui::GetMousePos().x - cursor_pos.x);
			}
		}

		if (ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
			if (pressed_status != ObjectStatus::NOTHING){
				draging_status = pressed_status;
			}

		}
		else {
			pressed_status = ObjectStatus::NOTHING;
			draging_status = ObjectStatus::NOTHING;
			index_drag = UINT64_MAX;
		}
		


		draw_list->AddLine({ p_min.x,rect_max.y }, { p_max.x,rect_max.y }, ImColor(0.3f, 0.3f, 0.3f), 1.f);
		p.y += itemHeight + 1.f;
	}


	draw_list->AddRectFilled({ p_min.x + left_range, p_min.y}, { p_min.x + right_range, p_max.y }, ImColor(1.f, 1.f, 1.f, 0.1f), 0.f, 0);

	timeCursor->Draw(p_min, p_max);


	if (index_drag != UINT64_MAX) {
		const auto& index = index_drag;
		const auto& status = draging_status;

		float delta = horizontalTransform->getPostionGlobal_lerped(ImGui::GetMousePos().x - cursor_pos.x) - drag_start_mouse_global;
		LayerInfo& info = layers[index].get()->GetLayerInfo();

		switch (status) {
		case ObjectStatus::LEFT_RESIZE:
		case ObjectStatus::RIGHT_RESIZE:
			CursorManager::getInstance().SetCursor(TypeCursor::RESIZE_HORIZONTAL);

			if (status == ObjectStatus::RIGHT_RESIZE) {
				info.time.End = drag_start_time_begin + length_drag + delta;

				if (info.time.End <= info.time.Begin) {
					info.time.End = info.time.Begin + 0.1f;
				}

				if (info.time.End <= 0.f) {
					info.time.End = 0.f;
				}

			}
			else {
				info.time.Begin = drag_start_time_begin + delta;

				if (info.time.Begin >= info.time.End) {
					info.time.Begin = info.time.End - 0.1f;
				}

				if (info.time.Begin >= timelineData->GetData().totalTime) {
					info.time.Begin = timelineData->GetData().totalTime;
				}
			}


			break;

		case ObjectStatus::BODY_MOVE:
			CursorManager::getInstance().SetCursor(TypeCursor::HAND_OPEN);

			

			info.time.Begin = drag_start_time_begin + delta;
			info.time.End = drag_start_time_begin + length_drag + delta;

			if (info.time.End <= 0.f) {
				info.time.End = 0.f;
				info.time.Begin = -length_drag;
			}

			if (info.time.Begin > timelineData->GetData().totalTime) {
				info.time.Begin = timelineData->GetData().totalTime;
				info.time.End	= info.time.Begin + length_drag;
			}

			break;
		}

	}
		
	


}

void Timeline_Layers_Viewer::Update() {

	


}