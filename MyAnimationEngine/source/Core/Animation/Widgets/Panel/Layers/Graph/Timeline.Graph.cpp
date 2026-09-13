#include "Timeline.Graph.h"

#include "Core/DebugOverlay/DebugOverlay.h"
#include "CursorManager/CursorManger.h"
#include <Utils/Math/Math.Collision.h>

Timeline_Graph::Timeline_Graph(
	TimelineContext* ctx,
	KeyframeDragHandler* kf_DragHandler,
	Timeline_TimeCursor* timeCursor,
	HorizontalTickRenderer* horizontalTickRenderer)
	:
	ctx(ctx),
	kf_DragHandler(kf_DragHandler),
	timeCursor(timeCursor),
	horizontalTickRenderer(horizontalTickRenderer)
{
	
	ctx->graphTransformVertical->setBaseStep(-100.f);
	ctx->graphTransformVertical->setBoundsOffset(-10.f - 0.01f, 10.f + 0.01f);
	ctx->graphTransformVertical->setBoundsScale(0.1f, 100.f);
	

}

Timeline_Graph::~Timeline_Graph() {

}

void Timeline_Graph::NeedCenterize() {
	this->NeedCenterized = true;
}


ImRect Timeline_Graph::getRectGraph() {
	ImRect result;
	result.Min = { FLT_MAX, FLT_MAX };
	result.Max = { FLT_MIN, FLT_MIN };

	for (const auto& selected_property : ctx->selector->getProperties()) {
		const auto& id_layer = selected_property.layer.id;
		const auto& name_property = selected_property.name;
		auto* layerObject = ctx->layersData->GetLayerByID(id_layer).get();
		auto& variant_prop = layerObject->GetProperties().getVariant(name_property);

		std::visit([&](auto& propertie) {
			auto& keyframes = propertie.kr.getKeyframes();

			if (keyframes.empty()) {
				const auto& values = propertie.kr.getDefaultValue();

				for (size_t i = 0; i < values.size(); i++) {
					result.Min.y = (std::min)(result.Min.y, (float)values[i]);
					result.Max.y = (std::max)(result.Max.y, (float)values[i]);
				}

			}
			else {
				result.Min.x = keyframes.front().sample.time;
				result.Max.x = keyframes.back().sample.time;

				for (size_t i = 0; i < keyframes.size(); i++) {
					result.Min.y = (std::min)(result.Min.y, (float)keyframes[i].sample.MinValue());
					result.Max.y = (std::max)(result.Max.y, (float)keyframes[i].sample.MaxValue());
				}
			}


		}, variant_prop);
	}

	if ((result.Min.x == FLT_MAX && result.Min.y == FLT_MAX) &&
		(result.Max.x != FLT_MIN && result.Max.y != FLT_MIN)) {

		result.Min.x = ctx->timelineData->GetData().currentTime - 0.1f;
		result.Max.x = ctx->timelineData->GetData().currentTime + 0.1f;
	}

	if (result.Min.x == result.Max.x) {
		float temp = result.Min.x;
		result.Min.x -= 0.1f;
		result.Max.x += 0.1f;
	}



	return result;
}


void Timeline_Graph::DrawTicks(ImVec2 p_min, ImVec2 p_max) {
	auto drawlist = ImGui::GetWindowDrawList();
	ImVec2 cursor_pos = ImGui::GetCursorScreenPos();
	ImVec2 region_max = ImGui::GetContentRegionAvail();

	horizontalTickRenderer->DrawTicks(
		p_min, p_max,
		colorLight, colorDark,
		1.5f, 1.f,
		region_max.y, region_max.y, region_max.y,
		false, true
	);

	//DebugOverlay::AddText("  up_bound" + std::to_string(verticalTransform->getMaxBoundView_lerped()), ImColor(1.f, 1.f, 1.f, 1.f));
	//DebugOverlay::AddText("down_bound" + std::to_string(verticalTransform->getMinBoundView_lerped()), ImColor(1.f, 1.f, 1.f, 1.f));


	float pixels_per_unit = std::abs(ctx->graphTransformVertical->getLengthTick_lerped());

	float target_spacing = 60.f;

	float tick_step = 1.f;
	int count_add_lines = 5;

	if (pixels_per_unit > 0.0001f) {
		double raw_step = target_spacing / pixels_per_unit;
		double exponent = std::floor(std::log10(raw_step));
		double fraction = raw_step / std::pow(10.0, exponent);

		double nice_fraction;
		if (fraction <= 1.5) {
			nice_fraction = 1.0;
			count_add_lines = 5;
		}
		else if (fraction <= 3.5) {
			nice_fraction = 2.0;
			count_add_lines = 4;
		}
		else if (fraction <= 7.5) {
			nice_fraction = 5.0;
			count_add_lines = 5;
		}
		else {
			nice_fraction = 10.0;
			count_add_lines = 5;
		}

		tick_step = static_cast<float>(nice_fraction * std::pow(10.0, exponent));
	}

	float sub_tick_step = tick_step / count_add_lines;



	uint64_t count_ticks = 0;

	float bound_a = ctx->graphTransformVertical->getMinBoundView_lerped();
	float bound_b = ctx->graphTransformVertical->getMaxBoundView_lerped();
	float v_min = (std::min)(bound_a, bound_b);
	float v_max = (std::max)(bound_a, bound_b);

	if (v_min < 0.f && v_max > 0.f) {
		float pos_y_center_line = cursor_pos.y + ctx->graphTransformVertical->getPosScreen_lerped(0.f);
		drawlist->AddLine({ p_min.x, pos_y_center_line }, { p_max.x, pos_y_center_line }, ImColor(0.4f, 0.4f, 0.4f, 1.f), 4.f);
		count_ticks++;

		const char* text_zero = "0";

		ImVec2 size_text = ImGui::CalcTextSize(text_zero);
		ImVec2 pos_text = { p_min.x + 5.f + 3.f, pos_y_center_line - size_text.y / 2.f };

		drawlist->AddRectFilled(pos_text, { pos_text.x + size_text.x, pos_text.y + size_text.y }, ImColor(ImGui::GetStyleColorVec4(ImGuiCol_WindowBg)), 4.f);
		drawlist->AddText(pos_text, ImColor(0.7f, 0.7f, 0.7f, 1.f), text_zero);
	}



	float start_val = std::floor(v_min / tick_step) * tick_step;

	for (float val = start_val; val <= v_max + tick_step; val += tick_step) {
		if (std::abs(val) > 0.0001f * tick_step) {
			float pos = cursor_pos.y + ctx->graphTransformVertical->getPosScreen_lerped(val);

			if (pos >= p_min.y && pos <= p_max.y) {
				ImColor main_color = colorLight;
				drawlist->AddLine({ p_min.x, pos }, { p_max.x, pos }, main_color, 2.f);

				std::string text_val = std::to_string(val);
				text_val.erase(text_val.find_last_not_of('0') + 1, std::string::npos);
				if (!text_val.empty() && text_val.back() == '.') text_val.pop_back();

				ImVec2 size_text = ImGui::CalcTextSize(text_val.c_str());
				ImVec2 pos_text = { p_min.x + 5.f + (val > 0.f ? 3.f : 0.f), pos - size_text.y / 2.f };

				drawlist->AddRectFilled(pos_text, { pos_text.x + size_text.x, pos_text.y + size_text.y }, ImColor(ImGui::GetStyleColorVec4(ImGuiCol_WindowBg)), 4.f);
				drawlist->AddText(pos_text, ImColor(0.7f, 0.7f, 0.7f, 1.f), text_val.c_str());


				count_ticks++;
			}
		}

		for (int j = 1; j < count_add_lines; j++) {
			float sub_val = val + sub_tick_step * j;

			if (sub_val > v_max) break;
			if (std::abs(sub_val) < 0.0001f * tick_step) continue;

			float pos_sub = cursor_pos.y + ctx->graphTransformVertical->getPosScreen_lerped(sub_val);

			if (pos_sub >= p_min.y && pos_sub <= p_max.y) {
				drawlist->AddLine({ p_min.x, pos_sub }, { p_max.x, pos_sub }, colorDark, 1.f);
				count_ticks++;
			}
		}
	}

	//DebugOverlay::AddText("rendered " + std::to_string(count_ticks), ImColor(1.f, 0.5f, 0.5f, 1.f));

}

void Timeline_Graph::DrawGraphs(ImVec2 p_min, ImVec2 p_max) {
	auto drawlist = ImGui::GetWindowDrawList();
	ImVec2 cursor_pos = ImGui::GetCursorScreenPos();
	ImVec2 region_max = ImGui::GetContentRegionAvail();


	if (ctx->layersData->GetData().layers.empty() || ctx->selector->getProperties().empty()) 
		return;

	float mouse_x_fixed = ImGui::GetMousePos().x - cursor_pos.x;
	float mouse_y_fixed = ImGui::GetMousePos().y - cursor_pos.y;
	float mouse_time_global_x = ctx->horizontalTransform->getPostionGlobal_real(mouse_x_fixed);
	float mouse_time_global_y = ctx->graphTransformVertical->getPosGlobal_real(mouse_y_fixed);

	const auto props = ctx->selector->getProperties();

	for (const auto& selected_property : props) {

		const auto& id_layer = selected_property.layer.id;
		const auto& name_property = selected_property.name;

		auto* layerObject = ctx->layersData->GetLayerByID(id_layer).get();
		float time_begin = layerObject->GetLayerInfo().time.Begin;

		auto& variant_prop = layerObject->GetProperties().getVariant(name_property);


		std::visit([&](auto& propertie) {
			auto& keyframes = propertie.kr.getKeyframes();
			auto& range = propertie.kr.getRange();
			
			if (range.has_value()) {
				float min_value = range.value().Min.front();
				float max_value = range.value().Max.front();

				min_graph = min_value;
				max_graph = max_value;

				float screen_min_range = ctx->graphTransformVertical->getPosScreen_lerped(min_value);
				float screen_max_range = ctx->graphTransformVertical->getPosScreen_lerped(max_value);

				drawlist->AddRectFilled(
					{ p_min.x, p_min.y + screen_min_range },
					{ p_max.x, p_max.y },
					ImColor(1.f, 0.f, 0.f, 0.1f)
				);

				drawlist->AddRectFilled(
					{ p_min.x, p_min.y },
					{ p_max.x, p_max.y - (region_max.y - screen_max_range) },
					ImColor(1.f, 0.f, 0.f, 0.1f)
				);

			}

			ImColor colors[9] = {
				ImColor{.82f, .34f, .34f, 1.f},
				ImColor{.44f, .81f, .47f, 1.f},
				ImColor{.56f, .76f, .95f, 1.f},
				ImColor{.95f, .69f, .4f, 1.f},
				ImColor{.84f, .71f, .82f, 1.f},
				ImColor{.92f, .79f, .5f, 1.f},
				ImColor{.97f, .58f, .84f, 1.f},
				ImColor{.32f, .84f, .8f, 1.f},
				ImColor{.37f, .75f, .84f, 1.f}
			};


			float front_begin = cursor_pos.x;
			float back_end = cursor_pos.x + region_max.x;

			if (keyframes.empty()) {

				const auto& values = propertie.kr.getDefaultValue();
				for (size_t j = 0; j < values.size(); j++) {
					float pos_y = cursor_pos.y + ctx->graphTransformVertical->getPosScreen_lerped(values[j]);
					ImColor color_line = colors[j];
					drawlist->AddLine({ front_begin, pos_y }, { back_end,pos_y }, color_line, 2.f);
				}

			}
			else {

				for (size_t j = 0; j < keyframes.front().sample.value.size(); j++) {
					float front_pos_x = cursor_pos.x + ctx->horizontalTransform->getPostionScreen_lerped(time_begin + keyframes.front().sample.time);
					float front_pos_y = cursor_pos.y + ctx->graphTransformVertical->getPosScreen_lerped(keyframes.front().sample.value[j]);

					float back_pos_x = cursor_pos.x + ctx->horizontalTransform->getPostionScreen_lerped(time_begin + keyframes.back().sample.time);
					float back_pos_y = cursor_pos.y + ctx->graphTransformVertical->getPosScreen_lerped(keyframes.back().sample.value[j]);


					ImColor color_line = colors[j];

					if (keyframes.size() == 1) {
						drawlist->AddLine({ front_begin, front_pos_y }, { back_end,front_pos_y }, color_line, 2.f);
					}
					else {
						const float gap = 3.f;
						const float length = 6.f;

						AddDashedLine(drawlist,
							{ front_pos_x, front_pos_y },
							{ front_begin, front_pos_y },
							color_line,
							1.f,
							length, gap);

						AddDashedLine(drawlist,
							{ back_pos_x, back_pos_y },
							{ back_end, back_pos_y },
							color_line,
							1.f,
							length, gap);
					}

				}
			}

			ImVec2 m = ImGui::GetMousePos();

			bool isHovered_Segment = false;
			bool isHoveredPoint = false;


			for (uint64_t i = 0; i < keyframes.size(); i++) {
				float pos_x = cursor_pos.x + ctx->horizontalTransform->getPostionScreen_lerped(time_begin + keyframes[i].sample.time);
				for (size_t j = 0; j < keyframes[i].sample.value.size(); j++) {
					float pos_y = cursor_pos.y + ctx->graphTransformVertical->getPosScreen_lerped(keyframes[i].sample.value[j]);
					ImVec2 min_hitbox = { pos_x - size_offset, pos_y - size_offset };
					ImVec2 max_hitbox = { pos_x + size_offset, pos_y + size_offset };
					if (ImGui::IsMouseHoveringRect(min_hitbox, max_hitbox)) {
						isHoveredPoint = true;
						break;
					}
				}
				if (isHoveredPoint)
					break;
			}

			bool OneIsHovered = false;

			for (size_t i = 1; i < keyframes.size(); i++) {
				float pos_x = cursor_pos.x + ctx->horizontalTransform->getPostionScreen_lerped(time_begin + keyframes[i].sample.time);
				float prev_pos_x = cursor_pos.x + ctx->horizontalTransform->getPostionScreen_lerped(time_begin + keyframes[i - 1].sample.time);

				for (size_t j = 0; j < keyframes[i].sample.value.size(); j++) {
					float pos_y = cursor_pos.y + ctx->graphTransformVertical->getPosScreen_lerped(keyframes[i].sample.value[j]);
					float prev_pos_y = cursor_pos.y + ctx->graphTransformVertical->getPosScreen_lerped(keyframes[i - 1].sample.value[j]);

					ImColor color_line = colors[j];


					bool isPressedLBM = false;

					if (keyframes[i - 1].type == TypeKeyframe::BINARY)
					{
						ImVec2 b1 = { prev_pos_x, prev_pos_y };
						ImVec2 e1 = { pos_x,prev_pos_y };

						ImVec2 b2 = { pos_x,prev_pos_y };
						ImVec2 e2 = { pos_x, pos_y };

						if (isHoveredPoint == false) {
							isHovered_Segment =  Collision::Check({ m.x,m.y }, Capsule{ Segment{ {b1.x,b1.y},{e1.x,e1.y} }, 4.f });
							isHovered_Segment |= Collision::Check({ m.x,m.y }, Capsule{ Segment{ {b2.x,b2.y},{e2.x,e2.y} }, 4.f });
						}
						
						if (isHovered_Segment && OneIsHovered == false) {
							CursorManager::getInstance().SetCursor(TypeCursor::HAND_OPEN);
							drawlist->AddLine(b1, e1, color_line, 4.f);
							drawlist->AddLine({ pos_x,prev_pos_y }, { pos_x, pos_y }, color_line, 4.f);
							OneIsHovered = true;

							isPressedLBM = ImGui::IsMouseClicked(ImGuiMouseButton_Left);
						}
						else {
							drawlist->AddLine(b1, e1, color_line, 2.f);
							drawlist->AddLine({ pos_x,prev_pos_y }, { pos_x, pos_y }, color_line, 2.f);
						}
					}
					else {

						ImVec2 b = { prev_pos_x, prev_pos_y };
						ImVec2 e = { pos_x,pos_y };

						if (isHoveredPoint == false) {
							isHovered_Segment = Collision::Check({ m.x,m.y }, Capsule{ Segment{ {b.x,b.y},{e.x,e.y} }, 4.f });
						}

						if (isHovered_Segment && OneIsHovered == false) {
							CursorManager::getInstance().SetCursor(TypeCursor::HAND_OPEN);
							drawlist->AddLine(b, e, color_line, 4.f);
							OneIsHovered = true;
							isPressedLBM = ImGui::IsMouseClicked(ImGuiMouseButton_Left);
						}
						else
							drawlist->AddLine(b, e, color_line, 2.f);
						
					}


					if (isPressedLBM) {

						SelectedKeyframe left;
						left.id = keyframes[i-1].id;
						left.property.name = name_property;
						left.property.layer.id = id_layer;

						SelectedKeyframe right;
						right.id = keyframes[i].id;
						right.property.name = name_property;
						right.property.layer.id = id_layer;

						//ctx->selector->UnsetAll();
						ctx->selector->SelectKeyframe(left);
						ctx->selector->AddMoreSelectedKeyframe(right);


						kf_DragHandler->BeginDrag(mouse_time_global_x, mouse_time_global_y, j, left);
					}

				}
			}

			
			float selected_min_time  = FLT_MAX, selected_max_time  = FLT_MIN;
			float selected_min_value = FLT_MAX, selected_max_value = FLT_MIN;

			bool isHaveSelected = ctx->selector->HasKeyframes();
			bool isHaveSelectedMoreThenOne = ctx->selector->getKeyframes().size() > 1;

			for (uint64_t i = 0; i < keyframes.size(); i++) {

				SelectedKeyframe current;
				current.id = keyframes[i].id;
				current.property.name = name_property;
				current.property.layer.id = id_layer;

				bool isSelected = ctx->selector->ContainsKeyframe(current);

				if (isSelected) {
					selected_min_time = (std::min)(selected_min_time, keyframes[i].sample.time);
					selected_max_time = (std::max)(selected_max_time, keyframes[i].sample.time);

					selected_min_value = (std::min)(selected_min_value, (float)keyframes[i].sample.MinValue());
					selected_min_value = (std::min)(selected_min_value, (float)keyframes[i].sample.MaxValue());

					selected_max_value = (std::max)(selected_max_value, (float)keyframes[i].sample.MinValue());
					selected_max_value = (std::max)(selected_max_value, (float)keyframes[i].sample.MaxValue());
				}



				float pos_x = cursor_pos.x + ctx->horizontalTransform->getPostionScreen_lerped(time_begin + keyframes[i].sample.time);

				for (uint64_t j = 0; j < keyframes[i].sample.value.size(); j++) {
					float pos_y = cursor_pos.y + ctx->graphTransformVertical->getPosScreen_lerped(keyframes[i].sample.value[j]);

					ImVec2 min_hitbox = { pos_x - size_offset, pos_y - size_offset };
					ImVec2 max_hitbox = { pos_x + size_offset, pos_y + size_offset };

					if (
						ctx->areaSelection->GetTypeAreaStart() == SelectionArea_Type::SELECTION_VIEWER &&
						ctx->areaSelection->IsActive())
					{

						if (ctx->areaSelection->CheckAABB({ min_hitbox, max_hitbox })) {
							ctx->selector->AddMoreSelectedKeyframe(current);
							ctx->selector->AddMoreSelectedProperty({ id_layer, name_property });
							ctx->selector->AddMoreSelectedLayer({ id_layer });
							break;
						}
						else {
							ctx->selector->EraseKeyframe(current);
						}
					}

				}

				for (uint64_t j = 0; j < keyframes[i].sample.value.size(); j++) {
					float pos_y = cursor_pos.y + ctx->graphTransformVertical->getPosScreen_lerped(keyframes[i].sample.value[j]);
					ImVec2 min_hitbox = { pos_x - size_offset, pos_y - size_offset };
					ImVec2 max_hitbox = { pos_x + size_offset, pos_y + size_offset };
					ImColor color_line = colors[j];


					bool isHoveredPoint = ImGui::IsMouseHoveringRect(min_hitbox, max_hitbox);



					if (isHoveredPoint) {
						CursorManager::getInstance().SetCursor(TypeCursor::HAND_OPEN);


						if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
							kf_DragHandler->BeginDrag(mouse_time_global_x, mouse_time_global_y, j, current);
						}

						if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !kf_DragHandler->IsDragging()) {
							ctx->selector->UnsetAll();
							ctx->selector->AddMoreSelectedKeyframe(current);
						}


						min_hitbox.x -= 1.f;
						min_hitbox.y -= 1.f;
						max_hitbox.x += 1.f;
						max_hitbox.y += 1.f;
					}
							

					if (isHaveSelected) {
						if (isSelected)
							drawlist->AddRectFilled(min_hitbox, max_hitbox, COLOR_GRAPH_SELECTED_KEYFRAME);
						else
							drawlist->AddRect(min_hitbox, max_hitbox, COLOR_GRAPH_SELECTED_KEYFRAME, 0, 0, 1.f);
					}
					else
						drawlist->AddRectFilled(min_hitbox, max_hitbox, color_line);
				}
				
			}



			// draw selected zone
			if (isHaveSelectedMoreThenOne) {

				float min_pos_x = cursor_pos.x + ctx->horizontalTransform->getPostionScreen_lerped(time_begin + selected_min_time);
				float max_pos_x = cursor_pos.x + ctx->horizontalTransform->getPostionScreen_lerped(time_begin + selected_max_time);

				float min_pos_y = cursor_pos.y + ctx->graphTransformVertical->getPosScreen_lerped(selected_max_value);
				float max_pos_y = cursor_pos.y + ctx->graphTransformVertical->getPosScreen_lerped(selected_min_value);

				static const float padding = 6.f;

				ImVec2 r_min = { min_pos_x - padding, min_pos_y - padding };
				ImVec2 r_max = { max_pos_x + padding, max_pos_y + padding };

				float r_mid_y = (min_pos_y + max_pos_y) / 2.f;

				ImVec2 s_left_min = { min_pos_x - padding - size_offset, r_mid_y - size_offset };
				ImVec2 s_left_max = { min_pos_x - padding + size_offset, r_mid_y + size_offset };
				bool is_left_drag_scale_hovered = ImGui::IsMouseHoveringRect(s_left_min, s_left_max);


				ImVec2 s_right_min = { max_pos_x + padding - size_offset, r_mid_y - size_offset };
				ImVec2 s_right_max = { max_pos_x + padding + size_offset, r_mid_y + size_offset };
				bool is_right_drag_scale_hovered = ImGui::IsMouseHoveringRect(s_right_min, s_right_max);


				drawlist->AddRectFilled(r_min, r_max, ImColor(1.f, 1.f, 1.f, 0.1f), 2.f);
				drawlist->AddRect(r_min, r_max, ImColor(1.f, 1.f, 1.f, 0.4f), 2.f, 0.f, 2.f);

				drawlist->AddRectFilled(s_left_min, s_left_max, ImColor(1.f, 1.f, 1.f, 1.0f), 0.f);
				drawlist->AddRectFilled(s_right_min, s_right_max, ImColor(1.f, 1.f, 1.f, 1.0f), 0.f);




				if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !kf_DragHandler->IsDragging()) {
					if (is_left_drag_scale_hovered) {
						kf_DragHandler->BeginDrag(mouse_time_global_x, mouse_time_global_y, -1, *ctx->selector->getKeyframes().begin(), KeyframeDragMode::ScaleLeft);
					}
					else if (is_right_drag_scale_hovered) {
						kf_DragHandler->BeginDrag(mouse_time_global_x, mouse_time_global_y, -1, *ctx->selector->getKeyframes().begin(), KeyframeDragMode::ScaleRight);
					}
					else if (ImGui::IsMouseHoveringRect(r_min, r_max)) {
						kf_DragHandler->BeginDrag(mouse_time_global_x, mouse_time_global_y, -1, *ctx->selector->getKeyframes().begin(), KeyframeDragMode::Translate);
					}
				}


			}


		}, variant_prop);
	}

}


void Timeline_Graph::Draw() {

	auto drawlist = ImGui::GetWindowDrawList();
	ImVec2 cursor_pos = ImGui::GetCursorScreenPos();
	ImVec2 region_max = ImGui::GetContentRegionAvail();

	ImVec2 p_min = cursor_pos;
	ImVec2 p_max = { cursor_pos.x + region_max.x, cursor_pos.y + region_max.y };

	ctx->graphTransformVertical->updateAvailableScreenValue(region_max.y);

	MyButtonBehavior(p_min, p_max, "Timeline_Graph_canvas_button", isHovered, isHeld);

	isHovered = ImGui::IsItemHovered();
	scroll_zoom = ImGui::GetIO().MouseWheel;

	float mouse_real_screen = ImGui::GetMousePos().y;
	float mouse_fixed = mouse_real_screen - cursor_pos.y;
	mouse_global = ctx->graphTransformVertical->getPosGlobal_real(mouse_fixed);
	mouse_screen = ctx->graphTransformVertical->getPosScreen_real(mouse_global);

	ImGui::SetCursorScreenPos(cursor_pos);

	DrawTicks(p_min, p_max);
	DrawGraphs(p_min, p_max);
	timeCursor->Draw(p_min, p_max);

	float left_range = ctx->horizontalTransform->getPostionScreen_lerped(ctx->timelineData->GetData().left_range_repeat);
	float right_range = ctx->horizontalTransform->getPostionScreen_lerped(ctx->timelineData->GetData().right_range_repeat);

	drawlist->AddRectFilled({ p_min.x + left_range, p_min.y }, { p_min.x + right_range, p_max.y }, ImColor(1.f, 1.f, 1.f, 0.1f), 0.f, 0);
	
	auto rect = getRectGraph();

	DebugOverlay::AddText(
		"GetRectGraph", ImColor(1.f, 1.f, 1.f, 1.f)
	);
	DebugOverlay::AddText({
			{ "Min: ", ImColor(1.f, 0.2f, 0.2f, 1.f) },
			{ std::to_string(rect.Min.x), ImColor(1.f,1.f,1.f,1.f)},
			{ std::to_string(rect.Min.y), ImColor(1.f,1.f,1.f,1.f) }
		}
	);
	DebugOverlay::AddText({
			{ "Max: ", ImColor(0.2f, 0.2f, 1.f, 1.f) },
			{ std::to_string(rect.Max.x), ImColor(1.f,1.f,1.f,1.f)},
			{ std::to_string(rect.Max.y), ImColor(1.f,1.f,1.f,1.f) }
		}
	);


	float mouse_x_fixed = ImGui::GetMousePos().x - cursor_pos.x;
	float mouse_y_fixed = ImGui::GetMousePos().y - cursor_pos.y;
	float mouse_time_global_x = ctx->horizontalTransform->getPostionGlobal_real(mouse_x_fixed);
	float mouse_time_global_y = ctx->graphTransformVertical->getPosGlobal_real(mouse_y_fixed);

	kf_DragHandler->ProcessDrag(mouse_time_global_x, mouse_time_global_y);


	if (ImGui::IsKeyDown(ImGuiKey_LeftShift) && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
		ctx->areaSelection->SetStatus(true);
		ctx->areaSelection->SetStartMousePosition(ImGui::GetMousePos());
		ctx->areaSelection->SetAreaStart(SelectionArea_Type::SELECTION_VIEWER);
	}
	if (!ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
		ctx->areaSelection->SetStatus(false);
	}

}

void Timeline_Graph::Update(const float& dt) {

	if (NeedCenterized) {
		if (ctx->graphTransformVertical->getAvailableScreenValue() > 100.f) {
			ctx->graphTransformVertical->centerize(0.f);
			NeedCenterized = false;
		}
	}
	ctx->graphTransformVertical->update(dt * 20.f);



	static bool isOfsetDeltaApplied = false;

	if (isDraging && ImGui::IsMouseDown(ImGuiMouseButton_Left) && !ImGui::IsKeyDown(ImGuiKey_LeftShift) && !kf_DragHandler->IsDragging()) {
		isOfsetDeltaApplied = true;
		float delta_y = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left).y;
		float delta_x = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left).x;
		ctx->graphTransformVertical->setOffsetDelta(delta_y);
		ctx->horizontalTransform->setOffsetDelta(delta_x);
	}
	else {
		isDraging = false;
		if (isOfsetDeltaApplied) {
			ctx->graphTransformVertical->applyOffsetDelta();
			ctx->horizontalTransform->applyOffsetDelta();
		}
		isOfsetDeltaApplied = false;
	}

	if (!isHovered) return;

	if (scroll_zoom > 0.f) {
		ctx->graphTransformVertical->zoomToPointByValue(mouse_global, 1.5f, false);
	}
	else if (scroll_zoom < 0.f) {
		ctx->graphTransformVertical->zoomToPointByValue(mouse_global, 0.75f, false);
	}

	if (ImGui::IsMouseClicked(ImGuiMouseButton_Left, false)) {
		isDraging = true;
	}

	if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
		auto rect = getRectGraph();
		if (((rect.Min.x != FLT_MAX && rect.Min.y != FLT_MAX) && (rect.Max.x != FLT_MIN && rect.Max.y != FLT_MIN))) {

			float width = rect.GetWidth()/1.8f;
			float height = rect.GetHeight()/4.f;

			rect.Min.x -= width / 2.f;
			rect.Max.x += width / 2.f;

			rect.Min.y -= height / 2.f;
			rect.Max.y += height / 2.f;


			ctx->graphTransformVertical->fitToRanges(rect.Max.y, rect.Min.y);

			ctx->horizontalTransform->fitToRanges(rect.Min.x, rect.Max.x);
		}
		else {
			ctx->graphTransformVertical->centerize(0.f);
		}

	}

}