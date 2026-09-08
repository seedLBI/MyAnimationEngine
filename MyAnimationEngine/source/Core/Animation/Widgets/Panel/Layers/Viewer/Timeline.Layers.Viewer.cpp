#include "Timeline.Layers.Viewer.h"
#include "CursorManager/CursorManger.h"
#include "Core/DebugOverlay/DebugOverlay.h"
#include "Core/Animation/Widgets/Panel/Textures/Noise/Texture.Noise.h"
#include "Core/Animation/Widgets/Colors/Widgets.Colors.h"


#include "KeyframeRenderer/KeyframeRenderer.h"

Timeline_Layers_Viewer::Timeline_Layers_Viewer(
	TimelineContext* ctx,
	Timeline_TimeCursor* timeCursor,
	LayersContextMenu_Manager* layers_ctx_menu)
	: 
	ctx(ctx),
	timeCursor(timeCursor),
	layers_ctx_menu(layers_ctx_menu)
{

}

Timeline_Layers_Viewer::~Timeline_Layers_Viewer() {

}

float Timeline_Layers_Viewer::SnapToGrid(float value) const {
	const float frameRate = ctx->timelineData->GetData().framerate;

	if (ImGui::IsKeyDown(ImGuiKey_LeftShift))
		return value;

	const float frameTime = 1.0f / frameRate;
	return std::round(value / frameTime) * frameTime;
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
Timeline_Layers_Viewer::ObjectStatus Timeline_Layers_Viewer::GetHoveredStatus(const ImRect& rect) {
	if (IsHovered_LeftResize(rect))
		return ObjectStatus::LEFT_RESIZE;

	if (IsHovered_RightResize(rect))
		return ObjectStatus::RIGHT_RESIZE;

	if (IsHovered_BodyMove(rect))
		return ObjectStatus::BODY_MOVE;

	return ObjectStatus::NOTHING;
}

void Timeline_Layers_Viewer::DrawLayerBox(const ImRect& rect, const LayerInfo& info, const bool& hovered, bool isSelected, bool isBodyDragging) {
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

	auto hash_uint64 = [](uint64_t x) -> uint64_t {
		x ^= x >> 30;
		x *= 0xbf58476d1ce4e5b9ULL;
		x ^= x >> 27;
		x *= 0x94d049bb133111ebULL;
		x ^= x >> 31;
		return x;
	};
	float some_rand = float(hash_uint64(info.id) % 10000ULL);


	// вот именно с таким width классно выглядит
	float width		= ctx->horizontalTransform->getPostionScreen_real(info.time.End) - ctx->horizontalTransform->getPostionScreen_real(info.time.Begin) + some_rand;
	float height	= rect.Max.y - rect.Min.y;

	float uv_width = width / SIZE_TEX;
	float uv_height = height / SIZE_TEX;
	
	ImVec2 uv_min = { some_rand / SIZE_TEX , 0.f };
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


	if (isBodyDragging) {
		float screen_pos_x = ui_info.p_min.x + ctx->horizontalTransform->getPostionScreen_lerped(info.time.Begin + info.time.DragInfo.OffsetFromBegin);
		const int count = 4;
		const float gap = 3.f;
		const float offset = -gap * float(count) / 2.f;
		for (size_t index_line = 0; index_line < count; index_line++) {
			float g = gap * (float)index_line;
			draw_list->AddLine(
				{ std::round(screen_pos_x + offset + g), rect.Min.y + 4.f },
				{ std::round(screen_pos_x + offset + g), rect.Max.y - 4.f },
				ImColor(1.f, 1.f, 1.f, 0.5f)
			);
			draw_list->AddLine(
				{ std::round(screen_pos_x + 1.f + offset + g), rect.Min.y + 4.f },
				{ std::round(screen_pos_x + 1.f + offset + g), rect.Max.y - 4.f },
				ImColor(0.f, 0.f, 0.f, 0.35f)
			);
		}
	}

}

void Timeline_Layers_Viewer::DrawBackground() {
	ui_info.draw_list->AddRectFilled(ui_info.p_min, ui_info.p_max, ImColor(.15f, .15f, .15f, 1.f));
}

void Timeline_Layers_Viewer::DrawRange() {
	float left_range = ctx->horizontalTransform->getPostionScreen_lerped(ctx->timelineData->GetData().left_range_repeat);
	float right_range = ctx->horizontalTransform->getPostionScreen_lerped(ctx->timelineData->GetData().right_range_repeat);

	ui_info.draw_list->AddRectFilled(
		{ ui_info.p_min.x + left_range,  ui_info.p_min.y }, 
		{ ui_info.p_min.x + right_range, ui_info.p_max.y }, 
		ImColor(1.f, 1.f, 1.f, 0.1f)
	);
}

void Timeline_Layers_Viewer::ProcessLayers() {
	float itemHeight = ctx->verticalTransform->getScaledHeightOneObject();
	float width_window = ImGui::GetWindowWidth();

	auto& layers = ctx->layersData->GetData().layers;
	auto& draw_list = ui_info.draw_list;

	ImVec2 p = ui_info.p_min;
	p.y += ctx->verticalTransform->getOffset_lerped();
	ctx->layersData->forEach([&](LayerObject* layer, uint64_t index) {

		auto& layerInfo = layer->GetLayerInfo();

		const auto& id_layer = layerInfo.id;
		const auto& index_layer = ctx->layersData->GetIndexByID(id_layer);

		float time_begin = layerInfo.time.Begin;
		float time_end = layerInfo.time.End;

		float screen_time_begin = ctx->horizontalTransform->getPostionScreen_lerped(time_begin);
		float screen_time_end = ctx->horizontalTransform->getPostionScreen_lerped(time_end);

		ImVec2 full_rect_min = p;
		ImVec2 full_rect_max = {p.x + width_window, p.y + itemHeight};

		ImVec2 rect_min = { p.x + screen_time_begin, p.y };
		ImVec2 rect_max = { p.x + screen_time_end,	 p.y + itemHeight };

		ImRect rect = ImRect{ rect_min, rect_max };

		bool isLayerSelected = ctx->selector->ContainsLayer({ id_layer });
		bool isHoveredLayerLine = ImGui::IsMouseHoveringRect(full_rect_min, full_rect_max);



		ImColor bg_layer = COLOR_BG_LAYER;
		if (isLayerSelected)
			bg_layer = COLOR_BG_LAYER_CHOOSED;

		draw_list->AddRectFilled({ ui_info.p_min.x, round(p.y + 1.f) }, { ui_info.p_max.x, round(p.y + itemHeight) }, bg_layer);

		ImGui::SetCursorScreenPos({ rect_min.x - width_hover,rect_min.y });

		ImGui::PushID(static_cast<int>(index));
		ImGui::InvisibleButton("Rect_LAYER", { rect.GetWidth() + width_hover * 2.f, rect.GetHeight() });
		ImGui::PopID();

		bool hovered = ImGui::IsItemHovered();
		bool active = ImGui::IsItemActive();

		

		ObjectStatus isHovered = GetHoveredStatus(rect);

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
					ctx->selector->AddMoreSelectedLayer_Toggle({ id_layer });
				else if (!ctx->selector->ContainsLayer({ id_layer }))
					ctx->selector->SelectLayer({ id_layer });

				ImGui::IsMouseDragging(ImGuiMouseButton_Left);

				index_drag = index;

				drag_start_mouse_global = ctx->horizontalTransform->getPostionGlobal_real(ImGui::GetMousePos().x - ui_info.p_min.x);

				for (const auto& layer : ctx->selector->getLayers()) {
					LayerInfo& info = ctx->layersData->GetLayerByID(layer.id).get()->GetLayerInfo();

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

		if (isHoveredLayerLine && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
			layers_ctx_menu->OpenMenu_LayerSetting( id_layer );
			ctx->selector->AddMoreSelectedLayer({ id_layer });
		}

		bool isBodyDragging = draging_status == ObjectStatus::BODY_MOVE && index_drag == index;

		DrawLayerBox(rect, layerInfo, hovered || active, isLayerSelected, isBodyDragging);

		if (layerInfo.isVisible == false) {
			draw_list->AddRectFilled(
				{ ui_info.p_min.x,rect_min.y }, 
				{ ui_info.p_max.x,rect_max.y }, 
				ImColor(0.f, 0.f, 0.f, 0.7f)
			);
		}


		draw_list->AddLine(
			{ ui_info.p_min.x, round(rect_max.y) }, 
			{ ui_info.p_max.x, round(rect_max.y) }, 
			ImColor(ImGui::GetStyleColorVec4(ImGuiCol_Separator)), 
			1.0f
		);


		p.y += itemHeight;





		if (layerInfo.isExpanded) {

			auto& properties = layer->GetProperties();

			ImVec2 p_prop_begin = p;

			properties.forEachUnwrapped([&](const std::string& name_propertie, auto& value) {

				float textHeight = ImGui::GetTextLineHeight();
				float offsetY = (itemHeight - textHeight) * 0.5f;

				if (value.kr.getKeyframes().empty() == false) {
					auto& keyframes = value.kr.getKeyframes();
					for (size_t i = 0; i < keyframes.size(); i++) {
						auto& keyframe = keyframes[i];
						auto& sample = keyframe.sample;

						float pos_screen = ctx->horizontalTransform->getPostionScreen_lerped(layerInfo.time.Begin + sample.time + value.kr.getDeltaTime());

						ImVec2 min_hitbox = { p.x + pos_screen - textHeight / 2.f, p.y };
						ImVec2 max_hitbox = { p.x + pos_screen + textHeight / 2.f, p.y + textHeight };


						SelectedKeyframe current = { {id_layer, name_propertie}, keyframe.id };


						if (
							ctx->areaSelection->GetTypeAreaStart() == SelectionArea_Type::SELECTION_VIEWER &&
							ctx->areaSelection->IsActive())
						{

							if (ctx->areaSelection->CheckAABB({ min_hitbox, max_hitbox })) {
								ctx->selector->AddMoreSelectedKeyframe(current);
								ctx->selector->AddMoreSelectedProperty({ id_layer, name_propertie });
								ctx->selector->AddMoreSelectedLayer({ id_layer });
							}
							else {
								ctx->selector->EraseKeyframe(current);
								if (!ctx->selector->HaveSelectedKeyframesInProperty({ id_layer, name_propertie })) {
									ctx->selector->ErasePropertie({ id_layer, name_propertie });
								}
								if (!ctx->selector->HaveSelectedKeyframesInLayer({ id_layer })) {
									ctx->selector->ErasePropertie({ id_layer, name_propertie });
									ctx->selector->EraseLayer({ id_layer });
								}
							}
						}

						bool isHovered = ImGui::IsMouseHoveringRect(min_hitbox, max_hitbox);
						bool isSelected = ctx->selector->ContainsKeyframe(current);

						if (isHovered && ctx->selector->ContainsKeyframe(current)) {
							CursorManager::getInstance().SetCursor(TypeCursor::HAND_OPEN);
						}

						if (isHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
							DragKeyframe.drag_started = true;
							DragKeyframe.startedDrag_global_x = ui_info.relative_mouse_global_pos_x;
							DragKeyframe.original_times.clear();

							if (ctx->selector->ContainsKeyframe(current)) {
								for (const auto& sel_kf : ctx->selector->getKeyframes()) {
									auto& properties = ctx->layersData->GetLayerByID(sel_kf.property.layer.id).get()->GetProperties();

									properties.applyToKr(
										sel_kf.property.name,
										[&](auto& kr) {
											auto* kf = kr.getKeyframeByID(sel_kf.id);
											if (kf) {
												DragKeyframe.original_times.push_back({ sel_kf, kf->sample.time });
											}
										}
									);
								}
							}
							else {
								ctx->selector->UnsetAll();
								ctx->selector->AddMoreSelectedKeyframe(current);
								ctx->selector->AddMoreSelectedProperty({ id_layer, name_propertie });
								ctx->selector->AddMoreSelectedLayer({ id_layer });

								auto& properties = ctx->layersData->GetLayerByID(id_layer).get()->GetProperties();
								properties.applyToKr(
									name_propertie,
									[&](auto& kr) {
										auto* kf = kr.getKeyframeByID(current.id);
										if (kf) {
											DragKeyframe.original_times.push_back({ current, kf->sample.time });
										}
									}
								);

							}
						}

						if (isHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !DragKeyframe.drag_started) {
							ctx->selector->UnsetAll();
							ctx->selector->AddMoreSelectedKeyframe(current);
						}


						KeyframeRenderer::Parameters params;
						params.color = COLOR_KEYFRAME;
						params.color_side = COLOR_KEYFRAME_SIDE;
						params.outline = COLOR_KEYFRAME_OUTLINE;
						params.isLeft = i == 0;
						params.isRight = i == keyframes.size() - 1;
						params.isMiddle = !params.isLeft && !params.isMiddle;
						params.isOnlyOne = keyframes.size() == 1;
						params.isSelected = isSelected;
						params.size = textHeight / 4.f;

						auto type_in  = i == 0 ? TypeKeyframe::LINEAR : keyframes[i - 1].type;
						auto type_out = keyframe.type;

						KeyframeRenderer::Draw(
							draw_list,
							type_in, type_out,
							{ p.x + pos_screen, p.y + textHeight / 2.f },
							params
						);

					}

				}

				p.y += itemHeight;
				draw_list->AddLine(
					{ ui_info.p_min.x, round(p.y) }, 
					{ ui_info.p_max.x, round(p.y) },
					ImColor(ImGui::GetStyleColorVec4(ImGuiCol_Separator)), 
					1.0f
				);

				ImGui::SetCursorScreenPos(p);
			});

			ImVec2 p_prop_end = p;

			if (ImGui::IsMouseHoveringRect(p_prop_begin, { ui_info.p_max.x, p_prop_end.y }) &&  ImGui::IsMouseClicked(ImGuiMouseButton_Right) && layer->GetProperties().isEmpty() == false) {
				layers_ctx_menu->OpenMenu_Creation();
			}


		}


	});

	if (ImGui::IsMouseHoveringRect(p, ui_info.p_max) && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
		layers_ctx_menu->OpenMenu_Creation();
	}

}

void Timeline_Layers_Viewer::HandleGlobalInput() {

	const auto& layers = ctx->layersData->GetData().layers;

	if (ImGui::IsKeyPressed(ImGuiKey_Delete)) {
		if (ctx->selector->HasKeyframes()) {
			auto keyframes = ctx->selector->getKeyframes();
			for (auto& keyframe : keyframes) {
				auto index = ctx->layersData->GetIndexByID(keyframe.property.layer.id);
				auto& properties = layers[index].get()->GetProperties();
				properties.applyToKr(
					keyframe.property.name,
					[&](auto& kr) {
						kr.EraseByID(keyframe.id);
					}
				);
			}

			ctx->selector->UnsetAll_Keyframes();
		}
		else if (ctx->selector->HasLayers()) {
			auto layers = ctx->selector->getLayers();
			for (auto& layer : layers) {
				ctx->layersData->EraseByID(layer.id);
			}
			ctx->selector->UnsetAll();
		}


	}

}

void Timeline_Layers_Viewer::HandleKeyframeDragging() {

	const auto& layers = ctx->layersData->GetData().layers;

	if (DragKeyframe.drag_started) {
		CursorManager::getInstance().SetCursor(TypeCursor::HAND_CLOSED);

		float total_delta = ui_info.relative_mouse_global_pos_x - DragKeyframe.startedDrag_global_x;

		for (const auto& drag_keyframe : DragKeyframe.original_times)
		{
			float orig_time = drag_keyframe.original_time;
			float target_time = SnapToGrid(orig_time + total_delta);

			const auto& layers = ctx->layersData->GetLayerByID(drag_keyframe.keyframe.property.layer.id);
			auto& properties = layers.get()->GetProperties();

			properties.applyToKr(
				drag_keyframe.keyframe.property.name,
				[&](auto& kr) {
					auto* kf = kr.getKeyframeByID(drag_keyframe.keyframe.id);
					if (kf) {
						float delta_to_apply = target_time - kf->sample.time;
						if (delta_to_apply != 0.f) {
							kr.setDeltaTimeForKeyframeByID(delta_to_apply, drag_keyframe.keyframe.id);
						}
					}
				}
			);

		}
	}


	if (index_drag == UINT64_MAX &&
		ui_info.isHoveredArea &&
		ImGui::IsMouseClicked(ImGuiMouseButton_Left) &&
		!DragKeyframe.drag_started)
	{
		ctx->selector->UnsetAll();
		ctx->areaSelection->SetStatus(true);
		ctx->areaSelection->SetStartMousePosition(ImGui::GetMousePos());
		ctx->areaSelection->SetAreaStart(SelectionArea_Type::SELECTION_VIEWER);

	}

	if (!ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
		if (DragKeyframe.drag_started) {

			for (const auto& drag_keyframe : DragKeyframe.original_times) {
				const auto& layer = ctx->layersData->GetLayerByID(drag_keyframe.keyframe.property.layer.id);
				auto& properties = layer.get()->GetProperties();
				properties.applyToKr(
					drag_keyframe.keyframe.property.name,
					[&](auto& kr) {
						kr.resolveOverlaps(drag_keyframe.keyframe.id);
					}
				);

			}

		}

		ctx->areaSelection->SetStatus(false);
		DragKeyframe.drag_started = false;
		DragKeyframe.original_times.clear();
	}
}

void Timeline_Layers_Viewer::HandleLayerDragging() {
	float delta = ctx->horizontalTransform->getPostionGlobal_lerped(ImGui::GetMousePos().x - ui_info.p_min.x) - drag_start_mouse_global;

	const auto& layers = ctx->layersData->GetData().layers;

	for (const auto& layer : ctx->selector->getLayers()) {
		const auto& index = ctx->layersData->GetIndexByID(layer.id);
		const auto& status = draging_status;

		LayerInfo& info = layers[index].get()->GetLayerInfo();

		switch (status) {
		case ObjectStatus::LEFT_RESIZE:
		case ObjectStatus::RIGHT_RESIZE:
			CursorManager::getInstance().SetCursor(TypeCursor::RESIZE_HORIZONTAL);

			if (status == ObjectStatus::RIGHT_RESIZE) {

				info.time.End = SnapToGrid(info.time.DragInfo.Prev_Begin + info.time.DragInfo.Prev_Length + delta);

				if (info.time.End <= info.time.Begin) {
					info.time.End = info.time.Begin + 0.1f;
				}

				if (info.time.End <= 0.f) {
					info.time.End = 0.f;
				}

			}
			else {
				float pos_begin = SnapToGrid(info.time.DragInfo.Prev_Begin + delta);
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

			float pos_begin = SnapToGrid(info.time.DragInfo.Prev_Begin + delta);

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

void Timeline_Layers_Viewer::Draw() {

	ImDrawList* draw_list = ImGui::GetWindowDrawList();

	ImVec2 region_max = ImGui::GetContentRegionAvail();
	ImVec2 cursor_pos = ImGui::GetCursorScreenPos();

	ui_info.draw_list = draw_list;
	ui_info.relative_mouse_pos = { ImGui::GetMousePos().x - cursor_pos.x,ImGui::GetMousePos().y - cursor_pos.y };
	ui_info.relative_mouse_global_pos_x = ctx->horizontalTransform->getPostionGlobal_real(ui_info.relative_mouse_pos.x);
	ui_info.p_min = cursor_pos;
	ui_info.p_max = { cursor_pos.x + region_max.x, cursor_pos.y + region_max.y };
	ui_info.isHoveredArea = ImGui::IsMouseHoveringRect(ui_info.p_min, ui_info.p_max);

	DrawBackground();

	ProcessLayers();

	HandleGlobalInput();
	HandleKeyframeDragging();
	HandleLayerDragging();

	DrawRange();
	timeCursor->Draw(ui_info.p_min, ui_info.p_max);
}

