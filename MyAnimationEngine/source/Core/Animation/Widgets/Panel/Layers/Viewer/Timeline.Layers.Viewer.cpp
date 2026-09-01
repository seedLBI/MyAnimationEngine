#include "Timeline.Layers.Viewer.h"
#include "CursorManager/CursorManger.h"
#include "Core/DebugOverlay/DebugOverlay.h"
#include "Core/Animation/Widgets/Panel/Textures/Noise/Texture.Noise.h"
#include "Core/Animation/Widgets/Panel/Colors/Panel.Colors.h"

Timeline_Layers_Viewer::Timeline_Layers_Viewer(
	TimelineContext* ctx,
	Timeline_TimeCursor* timeCursor)
	: 
	ctx(ctx),
	timeCursor(timeCursor)
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

void Timeline_Layers_Viewer::DrawLayerBox(const ImRect& rect, const LayerInfo& info, const bool& hovered, bool isSelected) {
	ImDrawList* draw_list = ImGui::GetWindowDrawList();

	ImVec2 cursor_pos = ImGui::GetCursorScreenPos();

	ImVec2 mouse_pos = { ImGui::GetMousePos().x - cursor_pos.x,ImGui::GetMousePos().y - cursor_pos.y };
	
	colorRGB outline;
	colorRGB body = lerp(info.color, colorRGB{ 0,0,0 }, 0.45f);
	
	if (isSelected)
		body = info.color;



	bool isLight = getLuminance(info.color) > 0.6f;

	if (isLight)
		outline = lerp(info.color, colorRGB{ 0,0,0 }, 0.55f);
	else
		outline = lerp(info.color, colorRGB{ 1,1,1 }, 0.45f);

	if (hovered) {
		outline = lerp(outline, colorRGB{ 1,1,1 }, 0.35f);
		body = lerp(body, colorRGB{ 1,1,1 }, 0.25f);
	}

	colorRGB underline = lerp(body, colorRGB{ 0,0,0 }, 0.55f);




	const float SIZE_TEX = 512.f;

	// вот именно с таким width классно выглядит
	float width		= ctx->horizontalTransform->getPostionScreen_real(info.time.End) - ctx->horizontalTransform->getPostionScreen_real(info.time.Begin);
	float height	= rect.Max.y - rect.Min.y;

	float uv_width = width / SIZE_TEX;
	float uv_height = height / SIZE_TEX;
	
	ImVec2 uv_min = { 0.f, 0.f };
	ImVec2 uv_max = { uv_width, uv_height };


	draw_list->AddRectFilled({ rect.Min.x,  rect.Min.y + 2.f }, { rect.Max.x, round(rect.Max.y) }, ImColor(body.r, body.g, body.b, 1.f), 0.f, 0);

	if (isSelected)
		draw_list->AddImage(
			NoiseTexture::getInstance().getID(),
			{ rect.Min.x,	rect.Min.y + 2.f },
			{ rect.Max.x,	round(rect.Max.y) },
			uv_min,
			uv_max,
			ImColor(1.f, 1.f, 1.f, 0.45f));
	


	draw_list->AddRect({ rect.Min.x - 0.5f,rect.Min.y + 2.f }, { rect.Max.x + 0.5f, round(rect.Max.y) }, ImColor(outline.r, outline.g, outline.b, 1.f), 0.f, 0, 1.f);
	if (!hovered)
		draw_list->AddRect({ rect.Min.x-1.2f, rect.Max.y-2.f }, { rect.Max.x + 1.2f, round(rect.Max.y) }, ImColor(underline.r, underline.g, underline.b, 1.f), 1.f);

}


void Timeline_Layers_Viewer::DrawRomb(const ImVec2& pos_center, float size, ImColor color, bool isSelected, ImColor colorSelected) {
	ImDrawList* draw_list = ImGui::GetWindowDrawList();


	ImVec2 vertexs_main[] = {
		{pos_center.x, pos_center.y - size},
		{pos_center.x + size, pos_center.y},
		{pos_center.x, pos_center.y + size},
		{pos_center.x - size, pos_center.y},
		{pos_center.x, pos_center.y - size},
	};

	if (isSelected)	{
		float upsize = size / 3.f;
		float roundness_offset = size / 4.f;
		ImVec2 vertexs_outline[] = {
			{pos_center.x - roundness_offset, pos_center.y - size - upsize},
			{pos_center.x + roundness_offset, pos_center.y - size - upsize},
			{pos_center.x + size + upsize, pos_center.y - roundness_offset},
			{pos_center.x + size + upsize, pos_center.y + roundness_offset},
			{pos_center.x + roundness_offset, pos_center.y + size + upsize},
			{pos_center.x - roundness_offset, pos_center.y + size + upsize},
			{pos_center.x - size - upsize, pos_center.y + roundness_offset},
			{pos_center.x - size - upsize, pos_center.y - roundness_offset},
			{pos_center.x - roundness_offset, pos_center.y - size - upsize},
		};
		draw_list->AddConvexPolyFilled(vertexs_outline, 8, colorSelected);
	}

	draw_list->AddConvexPolyFilled(vertexs_main, 4, color);

}


void Timeline_Layers_Viewer::Draw() {

	ImDrawList* draw_list = ImGui::GetWindowDrawList();

	ImVec2 region_max = ImGui::GetContentRegionAvail();
	ImVec2 cursor_pos = ImGui::GetCursorScreenPos();
	ImVec2 mouse_pos = { ImGui::GetMousePos().x - cursor_pos.x,ImGui::GetMousePos().y - cursor_pos.y };
	float mouse_global_pos_x = ctx->horizontalTransform->getPostionGlobal_real(mouse_pos.x);

	ImVec2 p_min = cursor_pos;
	ImVec2 p_max = { cursor_pos.x + region_max.x, cursor_pos.y + region_max.y };


	draw_list->AddRectFilled(p_min, p_max, ImColor(.15f, .15f, .15f, 1.f));

	float itemHeight = ctx->verticalTransform->getScaledHeightOneObject();
	float width_window = ImGui::GetWindowWidth();

	auto& layers = ctx->layersData->GetData().layers;

	ImVec2 p = p_min;
	p.y += ctx->verticalTransform->getOffset_lerped();
	ctx->layersData->forEach([&](LayerObject* layer, uint64_t index) {

		auto& layerInfo = layer->GetLayerInfo();

		float time_begin = layerInfo.time.Begin;
		float time_end = layerInfo.time.End;

		float screen_time_begin = ctx->horizontalTransform->getPostionScreen_lerped(time_begin);
		float screen_time_end = ctx->horizontalTransform->getPostionScreen_lerped(time_end);

		ImVec2 rect_min = { p.x + screen_time_begin, p.y };
		ImVec2 rect_max = { p.x + screen_time_end,	 p.y + itemHeight };

		ImRect rect = ImRect{ rect_min, rect_max };

		bool isLayerSelected = ctx->selector->ContainsLayer({ index });

		ImColor bg_layer = COLOR_BG_LAYER;
		if (isLayerSelected)
			bg_layer = COLOR_BG_LAYER_CHOOSED;

		draw_list->AddRectFilled({ p_min.x, round(p.y + 1.f) }, { p_max.x, round(p.y + itemHeight) }, bg_layer);

		ImGui::SetCursorScreenPos({ rect_min.x - width_hover,rect_min.y });

		ImGui::InvisibleButton(("Rect_LAYER##" + std::to_string(index)).c_str(), { rect.GetWidth() + width_hover * 2.f, rect.GetHeight() });

		bool hovered	= ImGui::IsItemHovered();
		bool active		= ImGui::IsItemActive();

		DrawLayerBox(rect, layerInfo, hovered || active, isLayerSelected);



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
			ctx->horizontalTransform->fitToRanges(time_begin - 0.1f, time_end + 0.1f);
		else if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
			pressed_status = isHovered;

			if (pressed_status != ObjectStatus::NOTHING) {

				if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl))
					ctx->selector->AddMoreSelectedLayer_Toggle({ index });
				else if (!ctx->selector->ContainsLayer({ index }))
					ctx->selector->SelectLayer({ index });

				ImGui::IsMouseDragging(ImGuiMouseButton_Left);

				index_drag = index;

				drag_start_mouse_global = ctx->horizontalTransform->getPostionGlobal_real(ImGui::GetMousePos().x - cursor_pos.x);

				for (const auto& layer : ctx->selector->getLayers()) {
					LayerInfo& info = layers[layer.index].get()->GetLayerInfo();

					info.time.DragInfo.OffsetFromBegin = drag_start_mouse_global - info.time.Begin;

					info.time.DragInfo.Prev_Length = info.time.End - info.time.Begin;
					info.time.DragInfo.Prev_Begin = info.time.Begin;
				}



			}
		}

		if (ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
			if (pressed_status != ObjectStatus::NOTHING)
				draging_status = pressed_status;
		}
		else {
			pressed_status = ObjectStatus::NOTHING;
			draging_status = ObjectStatus::NOTHING;
			index_drag = UINT64_MAX;
		}


		if (draging_status == ObjectStatus::BODY_MOVE && index_drag == index ) {
			float screen_pos_x = cursor_pos.x + ctx->horizontalTransform->getPostionScreen_lerped(time_begin + layerInfo.time.DragInfo.OffsetFromBegin);
			const int count = 4;
			const float gap = 3.f;
			const float offset = -gap * float(count) / 2.f;
			for (size_t index_line = 0; index_line < count; index_line++) {
				float g = gap * (float)index_line;
				draw_list->AddLine(
					{ std::round(screen_pos_x + offset + g), rect_min.y + 4.f},
					{ std::round(screen_pos_x + offset + g), rect_max.y - 4.f },
					ImColor(1.f, 1.f, 1.f, 0.5f)
				);
				draw_list->AddLine(
					{ std::round(screen_pos_x + 1.f + offset + g), rect_min.y + 4.f },
					{ std::round(screen_pos_x + 1.f + offset + g), rect_max.y - 4.f },
					ImColor(0.f, 0.f, 0.f, 0.35f)
				);
			}
		}


		if (layerInfo.isVisible == false) {
			draw_list->AddRectFilled({ p_min.x,rect_min.y }, { p_max.x,rect_max.y }, ImColor(0.f, 0.f, 0.f, 0.7f), 0.f, 0);
		}


		draw_list->AddLine({ p_min.x, round(rect_max.y) }, { p_max.x, round(rect_max.y) }, ImColor(ImGui::GetStyleColorVec4(ImGuiCol_Separator)), 1.0f);
		//ImGui::Separator();


		p.y += itemHeight;





		if (layerInfo.isExpanded) {

			auto& properties = layer->GetProperties();

			properties.forEachUnwrapped([&](const std::string& name, auto& value) {

				float textHeight = ImGui::GetTextLineHeight();
				float offsetY = (itemHeight - textHeight) * 0.5f;

				if (value.kr.getKeyframes().empty() == false) {
					auto& keyframes = value.kr.getKeyframes();
					for (size_t i = 0; i < keyframes.size(); i++) {
						auto& keyframe = keyframes[i];
						auto& sample = keyframe.sample;

						float pos_screen = ctx->horizontalTransform->getPostionScreen_lerped(layerInfo.time.Begin + sample.time + value.kr.getDeltaTime());

						ImVec2 min_hitbox = { p.x + pos_screen - textHeight / 2.f, p.y };
						ImVec2 max_hitbox = { p.x + pos_screen + textHeight / 2.f, p.y + textHeight};

						
						SelectedKeyframe current = { {index, name}, keyframe.id };


						if (
							ctx->areaSelection->GetTypeAreaStart() == SelectionArea_Type::SELECTION_VIEWER && 
							ctx->areaSelection->IsActive()) 
						{

							if (ctx->areaSelection->CheckAABB({ min_hitbox, max_hitbox })) {
								ctx->selector->AddMoreSelectedKeyframe(current);
								ctx->selector->AddMoreSelectedProperty({ index, name });
								ctx->selector->AddMoreSelectedLayer({ index });
							}
							else {
								ctx->selector->EraseKeyframe(current);
								if (!ctx->selector->HaveSelectedKeyframesInProperty({ index, name })) {
									ctx->selector->ErasePropertie({ index, name });
								}
								if (!ctx->selector->HaveSelectedKeyframesInLayer({index})) {
									ctx->selector->ErasePropertie({ index, name });
									ctx->selector->EraseLayer({ index });
								}
							}
						}

						bool isHovered  = ImGui::IsMouseHoveringRect(min_hitbox, max_hitbox);
						bool isSelected = ctx->selector->ContainsKeyframe(current);
						
						if (isHovered && ctx->selector->ContainsKeyframe(current)) {
							CursorManager::getInstance().SetCursor(TypeCursor::HAND_OPEN);
						}

						if (isHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
							DragKeyframe.drag_started = true;
							DragKeyframe.startedDrag_global_x = mouse_global_pos_x;
							DragKeyframe.original_times.clear();

							if (ctx->selector->ContainsKeyframe(current)) {
								for (const auto& sel_kf : ctx->selector->getKeyframes()) {
									auto& properties = layers[sel_kf.property.layer.index].get()->GetProperties();
									properties.applyToKr(
										sel_kf.property.name,
										[&](auto& kr) {
											auto* kf = kr.getKeyframeByID(sel_kf.index);
											if (kf) {
												std::string uid = std::to_string(sel_kf.property.layer.index) + "_" +
													sel_kf.property.name + "_" +
													std::to_string(sel_kf.index);
												DragKeyframe.original_times[uid] = kf->sample.time;
											}
										}
									);
								}
							}
							else {
								ctx->selector->UnsetAll();
								ctx->selector->AddMoreSelectedKeyframe(current);
								ctx->selector->AddMoreSelectedProperty({ index, name });
								ctx->selector->AddMoreSelectedLayer({ index });

								auto& properties = layers[index].get()->GetProperties();
								properties.applyToKr(
									name,
									[&](auto& kr) {
										auto* kf = kr.getKeyframeByID(current.index);
										if (kf) {
											std::string uid = std::to_string(index) + "_" +
												name + "_" +
												std::to_string(current.index);
											DragKeyframe.original_times[uid] = kf->sample.time;
										}
									}
								);


							}


						}


						if (isHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !DragKeyframe.drag_started) {
							ctx->selector->UnsetAll();
							ctx->selector->AddMoreSelectedKeyframe(current);
						}

						


						DrawRomb(
							{
								p.x + pos_screen,
								p.y + textHeight / 2.f
							},
							textHeight / 4.f,
							COLOR_KEYFRAME,
							isSelected,
							COLOR_KEYFRAME_OUTLINE
						);

						/*
						draw_list->AddText(
							{
								p.x + pos_screen,
								p.y + textHeight / 2.f
							},
							ImColor(1.f, 0.f, 0.f, 1.f),
							std::to_string(keyframe.id).c_str()
						);
						*/

					}
				}

				p.y += itemHeight;
				draw_list->AddLine({ p_min.x, round(p.y) }, { p_max.x, round(p.y) }, ImColor(ImGui::GetStyleColorVec4(ImGuiCol_Separator)), 1.0f);

				ImGui::SetCursorScreenPos(p);
			});
		}


	});


	DebugOverlay::AddText(std::string(std::string("AreaSelection : ") + std::string(ctx->areaSelection->IsActive() == true ? "TRUE" : "FALSE")).c_str(), ImColor(1.f, 1.f, 1.f, 1.f));


	if (DragKeyframe.drag_started) {
		CursorManager::getInstance().SetCursor(TypeCursor::HAND_CLOSED);

		float total_delta = mouse_global_pos_x - DragKeyframe.startedDrag_global_x;
		float step_frame = 1.f / (float)ctx->timelineData->GetData().framerate;

		auto snapToGrid = [step_frame](float value) -> float {
			return ImGui::IsKeyDown(ImGuiKey_LeftShift) ? value : std::round(value / step_frame) * step_frame;
		};

		for (auto& keyframe : ctx->selector->getKeyframes()) {

			std::string uid = std::to_string(keyframe.property.layer.index) + "_" +
				keyframe.property.name + "_" +
				std::to_string(keyframe.index);

			if (DragKeyframe.original_times.find(uid) == DragKeyframe.original_times.end())
				continue;

			float orig_time = DragKeyframe.original_times[uid];
			float target_time = snapToGrid(orig_time + total_delta);

			auto& properties = layers[keyframe.property.layer.index].get()->GetProperties();
			properties.applyToKr(
				keyframe.property.name, 
				[&](auto& kr) {
					auto* kf = kr.getKeyframeByID(keyframe.index);
					if (kf) {
						float delta_to_apply = target_time - kf->sample.time;
						if (delta_to_apply != 0.f) {
							kr.setDeltaTimeForKeyframeByID(delta_to_apply, keyframe.index);
						}
					}
				}
			);

		}
	}


	if (index_drag == UINT64_MAX &&
		ImGui::IsMouseHoveringRect(p_min, p_max) &&
		ImGui::IsMouseClicked(ImGuiMouseButton_Left) &&
		!DragKeyframe.drag_started)
	{
		ctx->selector->UnsetAll();
		ctx->areaSelection->SetStatus(true);
		ctx->areaSelection->SetStartMousePosition(ImGui::GetMousePos());
		ctx->areaSelection->SetAreaStart(SelectionArea_Type::SELECTION_VIEWER);

	}

	if (!ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
		ctx->areaSelection->SetStatus(false);
		DragKeyframe.drag_started = false;
		DragKeyframe.original_times.clear();
	}



	float left_range = ctx->horizontalTransform->getPostionScreen_lerped(ctx->timelineData->GetData().left_range_repeat);
	float right_range = ctx->horizontalTransform->getPostionScreen_lerped(ctx->timelineData->GetData().right_range_repeat);

	draw_list->AddRectFilled({ p_min.x + left_range, p_min.y}, { p_min.x + right_range, p_max.y }, ImColor(1.f, 1.f, 1.f, 0.1f), 0.f, 0);


	timeCursor->Draw(p_min, p_max);


	float delta = ctx->horizontalTransform->getPostionGlobal_lerped(ImGui::GetMousePos().x - cursor_pos.x) - drag_start_mouse_global;
	float step_frame = 1.f / (float)ctx->timelineData->GetData().framerate;

	auto snapToGrid = [step_frame](float value) -> float {
		return ImGui::IsKeyDown(ImGuiKey_LeftShift) ? value : std::round(value / step_frame) * step_frame;
	};

	for (const auto& layer : ctx->selector->getLayers()) {
		const auto& index = layer.index;
		const auto& status = draging_status;
		
		LayerInfo& info = layers[index].get()->GetLayerInfo();

		switch (status) {
		case ObjectStatus::LEFT_RESIZE:
		case ObjectStatus::RIGHT_RESIZE:
			CursorManager::getInstance().SetCursor(TypeCursor::RESIZE_HORIZONTAL);

			if (status == ObjectStatus::RIGHT_RESIZE) {

				info.time.End = snapToGrid(info.time.DragInfo.Prev_Begin + info.time.DragInfo.Prev_Length + delta);

				if (info.time.End <= info.time.Begin) {
					info.time.End = info.time.Begin + 0.1f;
				}

				if (info.time.End <= 0.f) {
					info.time.End = 0.f;
				}

			}
			else {
				float pos_begin = snapToGrid(info.time.DragInfo.Prev_Begin + delta);
				info.time.Begin = pos_begin;

				if (info.time.Begin >= info.time.End) {
					info.time.Begin = info.time.End - 0.1f;
				}

				if (info.time.Begin >= ctx->timelineData->GetData().totalTime) {
					info.time.Begin = ctx->timelineData->GetData().totalTime;
				}


				float tryingGetDelta = pos_begin - info.time.DragInfo.Prev_Begin;

				auto& properties = layers[index].get()->GetProperties();
				properties.forEachUnwrapped([&](const std::string& name, auto& value) {
					value.kr.setDeltaTime(-tryingGetDelta);
				});

			}


			break;

		case ObjectStatus::BODY_MOVE: {
			CursorManager::getInstance().SetCursor(TypeCursor::HAND_CLOSED);

			float pos_begin = snapToGrid(info.time.DragInfo.Prev_Begin + delta);

			info.time.Begin = pos_begin;
			info.time.End = pos_begin + info.time.DragInfo.Prev_Length;

			if (info.time.End <= 0.f) {
				info.time.End = 0.f;
				info.time.Begin = -info.time.DragInfo.Prev_Length;
			}

			if (info.time.Begin > ctx->timelineData->GetData().totalTime) {
				info.time.Begin = ctx->timelineData->GetData().totalTime;
				info.time.End = info.time.Begin + info.time.DragInfo.Prev_Length;
			}

			break;
		}
		case ObjectStatus::NOTHING:
			auto& properties = layers[index].get()->GetProperties();
			properties.forEachUnwrapped([&](const std::string& name, auto& value) {
				value.kr.applyDeltaTime();
			});
			break;
		}
	
	}
		

}

void Timeline_Layers_Viewer::Update() {

	


}