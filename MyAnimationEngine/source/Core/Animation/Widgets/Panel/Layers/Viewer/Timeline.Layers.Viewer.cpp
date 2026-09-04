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







void Timeline_Layers_Viewer::DrawKeyframe_Uniform(
	TypeKeyframe type, TypeKeyframe type_left, TypeKeyframe type_right,
	const ImVec2& pos_center,
	float size,
	ImColor color,
	ImColor color_side,
	bool isLeft, bool isMiddle, bool isRight, bool isOnlyOne,
	bool isSelected,
	ImColor colorSelected
) {
	ImDrawList* draw_list = ImGui::GetWindowDrawList();

	const float x_l = pos_center.x - size;
	const float x_m = pos_center.x;
	const float x_r = pos_center.x + size;
	
	const float y_u = pos_center.y - size;
	const float y_m = pos_center.y;
	const float y_d = pos_center.y + size;

	// LU--MU--RU
	// |   |   |
	// LM--MM--RM
	// |   |   |
	// LD--MD--RD

	const ImVec2 LU{ x_l, y_u };
	const ImVec2 LM{ x_l, y_m };
	const ImVec2 LD{ x_l, y_d };

	const ImVec2 MU{ x_m, y_u };
	const ImVec2 MM{ x_m, y_m };
	const ImVec2 MD{ x_m, y_d };

	const ImVec2 RU{ x_r, y_u };
	const ImVec2 RM{ x_r, y_m };
	const ImVec2 RD{ x_r, y_d };

	const float smooth = size / 3.f;
	const float offset_center = smooth * 1.2f;
	const float overlap = 1.0f;

	// b_ - means bezier, (hourglass shape) 
	// _o - overlap for fix anti aliasing

	const ImVec2 b_LU = { LU.x, LU.y + smooth };
	const ImVec2 b_RU = { RU.x, RU.y + smooth };
	const ImVec2 b_RD = { RD.x, RD.y - smooth };
	const ImVec2 b_LD = { LD.x, LD.y - smooth };

	const ImVec2 b_MM_L_U = { MM.x - offset_center, MM.y - smooth / 2.f };
	const ImVec2 b_MM_R_U = { MM.x + offset_center, MM.y - smooth / 2.f };
	const ImVec2 b_MM_L_D = { MM.x - offset_center, MM.y + smooth / 2.f };
	const ImVec2 b_MM_R_D = { MM.x + offset_center, MM.y + smooth / 2.f };

	const ImVec2 b_MM_U = { MM.x, MM.y - smooth / 2.f };
	const ImVec2 b_MM_D = { MM.x, MM.y + smooth / 2.f };

	const ImVec2 b_MM_U_o = { MM.x, b_MM_U.y - overlap };
	const ImVec2 b_MM_D_o = { MM.x, b_MM_D.y + overlap };

	const ImVec2 b_MM_L_U_o = { b_MM_L_U.x, b_MM_L_U.y - overlap };
	const ImVec2 b_MM_R_U_o = { b_MM_R_U.x, b_MM_R_U.y - overlap };
	const ImVec2 b_MM_L_D_o = { b_MM_L_D.x, b_MM_L_D.y + overlap };
	const ImVec2 b_MM_R_D_o = { b_MM_R_D.x, b_MM_R_D.y + overlap };

	type      = TypeKeyframe::BEZIER;
	type_left = TypeKeyframe::BEZIER;
	isMiddle  = true;

	switch (type)
	{
	case TypeKeyframe::LINEAR: {

		if (isMiddle && type_left == TypeKeyframe::BEZIER) {
			if (isSelected) {
				const ImVec2 bezier_linear_pts_path_cw[9] = {
					LU, MU, RM, MD, LD, b_LD, b_MM_L_D, b_MM_L_U, b_LU
				};
				draw_list->AddPolyline(bezier_linear_pts_path_cw, 9, colorSelected, ImDrawFlags_Closed, 3.2f);
			}
			const ImVec2 bl_top[5]    = { LU, MU, RM, b_MM_L_U, b_LU };
			const ImVec2 bl_middle[3] = { RM, b_MM_L_D_o, b_MM_L_U_o };
			const ImVec2 bl_bottom[5] = { RM, MD, LD, b_LD, b_MM_L_D };

			draw_list->AddConvexPolyFilled(bl_bottom, 5, color_side);
			draw_list->AddConvexPolyFilled(bl_top, 5, color_side);
			draw_list->AddConvexPolyFilled(bl_middle, 3, color_side);
		}
		else if (isMiddle && type_left == TypeKeyframe::BINARY) {
			const ImVec2 binary_linear_pts_path_cw[5] = { LU, MU, RM, MD, LD };
			if (isSelected)
				draw_list->AddPolyline(binary_linear_pts_path_cw, 5, colorSelected, ImDrawFlags_Closed, 3.2f);
			draw_list->AddConvexPolyFilled(binary_linear_pts_path_cw, 5, color);
		}
		else {
			if (isSelected) {
				const float out_size = size + (size / 3.f);
				const float r_offset = size / 4.f;

				const ImVec2 outline[8] = {
					{pos_center.x - r_offset, pos_center.y - out_size},
					{pos_center.x + r_offset, pos_center.y - out_size},
					{pos_center.x + out_size, pos_center.y - r_offset},
					{pos_center.x + out_size, pos_center.y + r_offset},
					{pos_center.x + r_offset, pos_center.y + out_size},
					{pos_center.x - r_offset, pos_center.y + out_size},
					{pos_center.x - out_size, pos_center.y + r_offset},
					{pos_center.x - out_size, pos_center.y - r_offset}
				};
				draw_list->AddConvexPolyFilled(outline, 8, colorSelected);
			}

			draw_list->AddQuadFilled(MU, RM, MD, LM, color);

			if (!isOnlyOne) {
				if (isLeft)
					draw_list->AddTriangleFilled(MU, RM, MD, color_side);
				else if (isRight)
					draw_list->AddTriangleFilled(MD, LM, MU, color_side);
				else if (isMiddle)
					draw_list->AddQuadFilled(MU, RM, MD, LM, color_side);
			}
		}

		break;
	}
	case TypeKeyframe::BINARY: {

		if (isMiddle && type_left == TypeKeyframe::LINEAR) {
			//linear-binary
			const ImVec2 linear_binary_pts_path_cw[5] = { LM, MU, RU, RD, MD };
			if (isSelected)
				draw_list->AddPolyline(linear_binary_pts_path_cw, 5, colorSelected, ImDrawFlags_Closed, 3.2f);
			draw_list->AddConvexPolyFilled(linear_binary_pts_path_cw, 5, color);
		}
		else if (isMiddle && type_left == TypeKeyframe::BEZIER) {
			//bezier-binary
			if (isSelected) {
				const ImVec2 bezier_binary_pts_path_cw[8] = {
					LU, RU, RD, LD, b_LD, b_MM_L_D, b_MM_L_U, b_LU
				};
				draw_list->AddPolyline(bezier_binary_pts_path_cw, 8, colorSelected, ImDrawFlags_Closed, 3.2f);
			}

			draw_list->AddQuadFilled(LU, RU, b_MM_L_U, b_LU, color_side);
			draw_list->AddQuadFilled(RU, RD, b_MM_L_D_o, b_MM_L_U_o, color_side);
			draw_list->AddQuadFilled(b_MM_L_D, RD, LD, b_LD, color_side);
		}
		else {
			draw_list->AddRectFilled(LU, RD, color);
		}

		break;
	}
	case TypeKeyframe::BEZIER: {

		if (isMiddle && type_left == TypeKeyframe::LINEAR) {

			// linear-bezier

			if (isSelected) {
				const ImVec2 linear_bezier_pts_path_cw[9] = {
					LM, MU, RU, b_RU, b_MM_R_U, b_MM_R_D, b_RD, RD, MD
				};
				draw_list->AddPolyline(linear_bezier_pts_path_cw, 9, colorSelected, ImDrawFlags_Closed, 3.2f);
			}
			const ImVec2 lb_top[5]    = { LM, MU, RU, b_RU, b_MM_R_U };
			const ImVec2 lb_middle[3] = { LM, b_MM_R_U_o, b_MM_R_D_o};
			const ImVec2 lb_bottom[5] = { b_MM_R_D, b_RD, RD, MD, LM };

			draw_list->AddConvexPolyFilled(lb_bottom, 5, color_side);
			draw_list->AddConvexPolyFilled(lb_top, 5, color_side);
			draw_list->AddConvexPolyFilled(lb_middle, 3, color_side);


		}
		else if (isMiddle && type_left == TypeKeyframe::BINARY) {

			// binary-bezier

			if (isSelected) {
				const ImVec2 binary_bezier_pts_path_cw[8] = {
					LU, RU, b_RU, b_MM_R_U, b_MM_R_D, b_RD, RD, LD 
				};
				draw_list->AddPolyline(binary_bezier_pts_path_cw, 8, colorSelected, ImDrawFlags_Closed, 3.2f);
			}

			draw_list->AddQuadFilled(LU, RU, b_RU, b_MM_R_U, color_side);
			draw_list->AddQuadFilled(LU, b_MM_R_U_o, b_MM_R_D_o, LD, color_side);
			draw_list->AddQuadFilled(b_MM_R_D, b_RD, RD, LD, color_side);

		}
		else {
			// bezier

			if (isSelected) {
				const ImVec2 bezier_pts_path_cw[12] = {
					LU, RU, b_RU, b_MM_R_U, b_MM_R_D, b_RD, RD, LD, b_LD, b_MM_L_D, b_MM_L_U, b_LU
				};
				draw_list->AddPolyline(bezier_pts_path_cw, 12, colorSelected, ImDrawFlags_Closed, 3.2f);
			}

			const ImVec2 b_top[6] = { LU, RU, b_RU, b_MM_R_U, b_MM_L_U, b_LU };
			const ImVec2 b_bottom[6] = { b_MM_R_D, b_RD, RD, LD, b_LD,  b_MM_L_D };
			ImColor color_full_body = (isMiddle && !isOnlyOne) == false ? color : color_side;

			draw_list->AddConvexPolyFilled(b_top, 6, color_full_body);
			draw_list->AddQuadFilled(b_MM_R_U_o, b_MM_R_D_o, b_MM_L_D_o, b_MM_L_U_o, color_full_body);
			draw_list->AddConvexPolyFilled(b_bottom, 6, color_full_body);


			if (!isOnlyOne && (isLeft || isRight)) {

				if (isLeft) {
					const ImVec2 b_top_right[5] = { MU, RU, b_RU, b_MM_R_U, b_MM_U };
					const ImVec2 b_bottom_right[5] = { b_MM_D, b_MM_R_D, b_RD, RD, MD };
					const ImVec2 b_connect_right[4] = { b_MM_U_o, b_MM_R_U_o, b_MM_R_D_o, b_MM_D_o };

					draw_list->AddConvexPolyFilled(b_top_right, 5, color_side);
					draw_list->AddConvexPolyFilled(b_connect_right, 4, color_side);
					draw_list->AddConvexPolyFilled(b_bottom_right, 5, color_side);

				}
				else if (isRight) {
					const ImVec2 b_top_left[5] = { MU, b_MM_U, b_MM_L_U, b_LU, LU };
					const ImVec2 b_connect_left[4] = { b_MM_U_o, b_MM_D_o, b_MM_L_D_o, b_MM_L_U_o };
					const ImVec2 b_bottom_left[5] = { b_MM_D, MD, LD, b_LD, b_MM_L_D };

					draw_list->AddConvexPolyFilled(b_top_left, 5, color_side);
					draw_list->AddConvexPolyFilled(b_connect_left, 4, color_side);
					draw_list->AddConvexPolyFilled(b_bottom_left, 5, color_side);
				}

			}
		}

		break;
	}
	case TypeKeyframe::AUTOBEZIER: {
		if (isSelected) {
			const float out_size = size + (size / 3.f);
			draw_list->AddCircleFilled(pos_center, out_size, colorSelected);
		}

		draw_list->AddCircleFilled(pos_center, size, color);

		if (!isOnlyOne) {
			if (isMiddle) {
				draw_list->AddCircleFilled(pos_center, size, color_side);
			}
			else if (isLeft) {
				draw_list->PathArcTo(pos_center, size, -IM_PI / 2.0f, IM_PI / 2.0f, 0);
				draw_list->PathFillConvex(color_side);
			}
			else if (isRight) {
				draw_list->PathArcTo(pos_center, size, IM_PI / 2.0f, IM_PI * 1.5f, 0);
				draw_list->PathFillConvex(color_side);
			}
		}
		break;
	}
	default:
		break;
	}

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

						bool isLeftKeyframe   = i == 0;
						bool isRightKeyframe  = i == keyframes.size() - 1;
						bool isMiddleKeyframe = !isLeftKeyframe && !isRightKeyframe;
						bool isOnlyOneKeyframe = keyframes.size() == 1;


						DrawKeyframe_Uniform(
							keyframe.type,
							i == 0 ? TypeKeyframe::LINEAR : keyframes[i - 1].type,
							i == keyframes.size() - 1 ? TypeKeyframe::LINEAR : keyframes[i + 1].type,
							{
								p.x + pos_screen,
								p.y + textHeight / 2.f
							},
							textHeight / 4.f,
							COLOR_KEYFRAME,
							COLOR_KEYFRAME_SIDE,
							isLeftKeyframe, isMiddleKeyframe, isRightKeyframe, isOnlyOneKeyframe,
							isSelected,
							COLOR_KEYFRAME_OUTLINE
						);

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
		if (DragKeyframe.drag_started) {
			for (const auto& keyframe : ctx->selector->getKeyframes()) {
				std::string uid = 
					std::to_string(keyframe.property.layer.index) + "_" +
					keyframe.property.name + "_" +
					std::to_string(keyframe.index);

				if (DragKeyframe.original_times.find(uid) != DragKeyframe.original_times.end()) {
					auto& properties = layers[keyframe.property.layer.index].get()->GetProperties();
					properties.applyToKr(
						keyframe.property.name,
						[&](auto& kr) {
							kr.resolveOverlaps(keyframe.index);
						}
					);
				}
			}
		}

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