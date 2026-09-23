#include "Timeline.Graph.h"

#include "Core/DebugOverlay/DebugOverlay.h"
#include "CursorManager/CursorManger.h"
#include <Utils/Math/Math.Collision.h>

#include "ThirdParty/LivePT/LivePT.h"

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
	
	graphButtonsDrawer = new GraphButtonsDrawer(ctx);
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


	float selected_min_time = FLT_MAX, selected_max_time = -FLT_MAX;
	float selected_min_value = FLT_MAX, selected_max_value = -FLT_MAX;

	bool isHaveSelected = ctx->selector->HasKeyframes();
	bool isHaveSelectedMoreThenOne = ctx->selector->getKeyframes().size() > 1;


	static uint64_t s_drag_layer_id = 0;
	static std::string s_drag_prop_name = "";
	static uint64_t s_drag_left_kf_id = 0;
	static uint64_t s_drag_right_kf_id = 0;
	static int s_drag_dim_index = -1;

	static bool isDragAll = false;

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


			float front_begin = cursor_pos.x;
			float back_end = cursor_pos.x + region_max.x;

			if (keyframes.empty()) {

				const auto& values = propertie.kr.getDefaultValue();
				for (size_t j = 0; j < values.size(); j++) {
					float pos_y = cursor_pos.y + ctx->graphTransformVertical->getPosScreen_lerped(values[j]);
					ImColor color_line = ctx->colors_params[j];
					drawlist->AddLine({ front_begin, pos_y }, { back_end,pos_y }, color_line, 2.f);
				}

			}
			else {

				for (size_t j = 0; j < keyframes.front().sample.value.size(); j++) {
					float front_pos_x = cursor_pos.x + ctx->horizontalTransform->getPostionScreen_lerped(time_begin + keyframes.front().sample.time);
					float front_pos_y = cursor_pos.y + ctx->graphTransformVertical->getPosScreen_lerped(keyframes.front().sample.value[j]);

					float back_pos_x = cursor_pos.x + ctx->horizontalTransform->getPostionScreen_lerped(time_begin + keyframes.back().sample.time);
					float back_pos_y = cursor_pos.y + ctx->graphTransformVertical->getPosScreen_lerped(keyframes.back().sample.value[j]);


					ImColor color_line = ctx->colors_params[j];

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
			bool isHovered_CurveSegment = false;
			int hovered_left_kf = -1;
			int hovered_right_kf = -1;
			int hovered_dim_index = -1;

			const auto& samples = propertie.kr.getSamples();
			if (samples.size() > 1) {
				if (!kf_DragHandler->IsDragging()) {

					size_t current_kf_idx = 1;

					for (size_t i = 1; i < samples.size(); i++) {

						float t_curr_base = samples[i].time[0];
						while (current_kf_idx < keyframes.size() && keyframes[current_kf_idx].sample.time < t_curr_base) {
							current_kf_idx++;
						}
						if (isHovered_CurveSegment) break;



						for (size_t j = 0; j < samples[i].time.size(); j++) {
							float t_curr = samples[i].time[j];
							float t_prev = samples[i - 1].time[j];

							float pos_x = cursor_pos.x + ctx->horizontalTransform->getPostionScreen_lerped(time_begin + t_curr);
							float prev_pos_x = cursor_pos.x + ctx->horizontalTransform->getPostionScreen_lerped(time_begin + t_prev);

							const float capsule_radius = 4.f;

							if (m.x < prev_pos_x - capsule_radius || m.x > pos_x + capsule_radius) {
								continue;
							}

							float pos_y = cursor_pos.y + ctx->graphTransformVertical->getPosScreen_lerped(samples[i].value[j]);
							float prev_pos_y = cursor_pos.y + ctx->graphTransformVertical->getPosScreen_lerped(samples[i - 1].value[j]);

							ImVec2 b = { prev_pos_x, prev_pos_y };
							ImVec2 e = { pos_x, pos_y };

							if (!isHoveredPoint && !isHovered_CurveSegment) {
								if (Collision::Check({ m.x, m.y }, Capsule{ Segment{ {b.x, b.y}, {e.x, e.y} }, capsule_radius })) {
									isHovered_CurveSegment = true;
									hovered_dim_index = j;

									if (current_kf_idx < keyframes.size()) {
										hovered_left_kf = current_kf_idx - 1;
										hovered_right_kf = current_kf_idx;
									}
								}
							}
						}
					}
				}

				float active_drag_left_time = -1.f;
				float active_drag_right_time = -1.f;

				if (kf_DragHandler->IsDragging() && s_drag_layer_id == id_layer && s_drag_prop_name == name_property) {
					for (const auto& kf : keyframes) {
						if (kf.id == s_drag_left_kf_id) active_drag_left_time = kf.sample.time;
						if (kf.id == s_drag_right_kf_id) active_drag_right_time = kf.sample.time;
					}
				}


				for (size_t i = 1; i < samples.size(); i++) {
					for (size_t j = 0; j < samples[i].time.size(); j++) {
						float t_curr = samples[i].time[j];
						float t_prev = samples[i - 1].time[j];

						float pos_x = cursor_pos.x + ctx->horizontalTransform->getPostionScreen_lerped(time_begin + t_curr);
						float pos_y = cursor_pos.y + ctx->graphTransformVertical->getPosScreen_lerped(samples[i].value[j]);
						float prev_pos_x = cursor_pos.x + ctx->horizontalTransform->getPostionScreen_lerped(time_begin + t_prev);
						float prev_pos_y = cursor_pos.y + ctx->graphTransformVertical->getPosScreen_lerped(samples[i - 1].value[j]);

						ImColor color_line = ctx->colors_params[j];
						ImVec2 b = { prev_pos_x, prev_pos_y };
						ImVec2 e = { pos_x, pos_y };

						float line_thickness = 2.f;

						if (isDragAll == false) {
							if (kf_DragHandler->IsDragging()) {
								if (s_drag_layer_id == id_layer &&
									s_drag_prop_name == name_property &&
									s_drag_dim_index == j) {

									if (active_drag_left_time != -1.f && active_drag_right_time != -1.f) {
										if (t_prev >= active_drag_left_time - 0.0001f && t_curr <= active_drag_right_time + 0.0001f) {
											line_thickness = 4.f;
										}
									}
								}
							}
							else {
								if (hovered_left_kf != -1 && hovered_right_kf != -1 && j == hovered_dim_index) {
									float kf_left_time = keyframes[hovered_left_kf].sample.time;
									float kf_right_time = keyframes[hovered_right_kf].sample.time;

									if (t_prev >= kf_left_time - 0.0001f && t_curr <= kf_right_time + 0.0001f) {
										line_thickness = 4.f;
									}
								}
							}
						}

						drawlist->AddLine(b, e, color_line, line_thickness);
					}
				}
			}

			if (isHovered_CurveSegment && !OneIsHovered && hovered_left_kf != -1 && hovered_right_kf != -1) {
				CursorManager::getInstance().SetCursor(TypeCursor::HAND_OPEN);
				OneIsHovered = true;

				if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
					SelectedKeyframe left;
					left.id = keyframes[hovered_left_kf].id;
					left.property.name = name_property;
					left.property.layer.id = id_layer;

					SelectedKeyframe right;
					right.id = keyframes[hovered_right_kf].id;
					right.property.name = name_property;
					right.property.layer.id = id_layer;

					ctx->selector->SelectKeyframe(left);
					ctx->selector->AddMoreSelectedKeyframe(right);

					s_drag_layer_id = id_layer;
					s_drag_prop_name = name_property;
					s_drag_left_kf_id = left.id;
					s_drag_right_kf_id = right.id;
					s_drag_dim_index = hovered_dim_index;

					kf_DragHandler->BeginDrag(mouse_time_global_x, mouse_time_global_y, hovered_dim_index, left, KeyframeDragMode::TranslateXY);
				}
			}

			for (uint64_t i = 0; i < keyframes.size(); i++) {

				SelectedKeyframe current;
				current.id = keyframes[i].id;
				current.property.name = name_property;
				current.property.layer.id = id_layer;

				bool isSelected = ctx->selector->ContainsKeyframe(current);

				bool isRightSelected = i != keyframes.size() - 1 ? ctx->selector->ContainsKeyframe({ current.property, keyframes[i + 1].id }) : false;
				bool isLeftSelected = i != 0 ? ctx->selector->ContainsKeyframe({ current.property, keyframes[i - 1].id }) : false;


				if (isSelected) {

					if (selected_min_time == FLT_MAX) {
						selected_min_time = time_begin + keyframes[i].sample.time;
						selected_max_time = time_begin + keyframes[i].sample.time;
					}

					selected_min_time = (std::min)(selected_min_time, time_begin + keyframes[i].sample.time);
					selected_max_time = (std::max)(selected_max_time, time_begin + keyframes[i].sample.time);

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
						ctx->areaSelection->GetTypeAreaStart() == SelectionArea_Type::SELECTION_RIGHT_HALF &&
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

					ImVec2 pos_current_keyframe = { pos_x,pos_y };

					ImVec2 min_hitbox = { pos_x - size_offset, pos_y - size_offset };
					ImVec2 max_hitbox = { pos_x + size_offset, pos_y + size_offset };
					ImColor color_line = ctx->colors_params[j];


					bool isHoveredPoint = ImGui::IsMouseHoveringRect(min_hitbox, max_hitbox);

					if (isHoveredPoint) {
						CursorManager::getInstance().SetCursor(TypeCursor::HAND_OPEN);


						if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
							kf_DragHandler->BeginDrag(mouse_time_global_x, mouse_time_global_y, j, current, KeyframeDragMode::TranslateXY);
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
					
					if (keyframes[i].anchors.in.has_value() && i != 0 && (isSelected ||  isLeftSelected)) {
						float prev_pos_x = cursor_pos.x + ctx->horizontalTransform->getPostionScreen_lerped(time_begin + keyframes[i - 1].sample.time);

						float  pos_anchor_in_x = pos_x - (pos_x - prev_pos_x) * keyframes[i].anchors.in.value().time[j];
						float  pos_anchor_in_y = cursor_pos.y + ctx->graphTransformVertical->getPosScreen_lerped(keyframes[i].anchors.in.value().value[j]);

						ImVec2 pos_anchor_in = { pos_anchor_in_x, pos_anchor_in_y };

						drawlist->AddLine(pos_anchor_in, pos_current_keyframe, ImColor(1.f, 1.f, 0.f, 1.f), 1.f);
						drawlist->AddCircleFilled(pos_anchor_in, 2.5f, ImColor(1.f, 1.f, 0.f, 1.f));

					}

					if (keyframes[i].anchors.out.has_value() && i != keyframes.size() - 1 && (isSelected || isRightSelected )) {
						float next_pos_x = cursor_pos.x + ctx->horizontalTransform->getPostionScreen_lerped(time_begin + keyframes[i + 1].sample.time);

						float  pos_anchor_out_x = pos_x + (next_pos_x - pos_x) * keyframes[i].anchors.out.value().time[j];
						float  pos_anchor_out_y = cursor_pos.y + ctx->graphTransformVertical->getPosScreen_lerped(keyframes[i].anchors.out.value().value[j]);

						ImVec2 pos_anchor_out = { pos_anchor_out_x, pos_anchor_out_y };

						drawlist->AddLine(pos_anchor_out, pos_current_keyframe, ImColor(1.f, 1.f, 0.f, 1.f), 1.f);
						drawlist->AddCircleFilled(pos_anchor_out, 2.5f, ImColor(1.f, 1.f, 0.f, 1.f));

					}


					if (keyframes[i].anchors.in.has_value() && i != 0 && (isSelected || isLeftSelected)) {
						float prev_pos_x = cursor_pos.x + ctx->horizontalTransform->getPostionScreen_lerped(time_begin + keyframes[i - 1].sample.time);
						float pos_anchor_in_x = pos_x - (pos_x - prev_pos_x) * keyframes[i].anchors.in.value().time[j];
						float pos_anchor_in_y = cursor_pos.y + ctx->graphTransformVertical->getPosScreen_lerped(keyframes[i].anchors.in.value().value[j]);

						ImVec2 pos_anchor_in = { pos_anchor_in_x, pos_anchor_in_y };
						drawlist->AddLine(pos_anchor_in, pos_current_keyframe, ImColor(1.f, 1.f, 0.f, 1.f), 1.f);
						drawlist->AddCircleFilled(pos_anchor_in, 2.5f, ImColor(1.f, 1.f, 0.f, 1.f));

						ImVec2 min_hitbox_in = { pos_anchor_in_x - size_offset, pos_anchor_in_y - size_offset };
						ImVec2 max_hitbox_in = { pos_anchor_in_x + size_offset, pos_anchor_in_y + size_offset };
						if (ImGui::IsMouseHoveringRect(min_hitbox_in, max_hitbox_in)) {
							CursorManager::getInstance().SetCursor(TypeCursor::HAND_OPEN);
							if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
								kf_DragHandler->BeginDrag(mouse_time_global_x, mouse_time_global_y, j, current, KeyframeDragMode::DragAnchorIn);
							}
						}
					}

					if (keyframes[i].anchors.out.has_value() && i != keyframes.size() - 1 && (isSelected || isRightSelected)) {
						float next_pos_x = cursor_pos.x + ctx->horizontalTransform->getPostionScreen_lerped(time_begin + keyframes[i + 1].sample.time);
						float pos_anchor_out_x = pos_x + (next_pos_x - pos_x) * keyframes[i].anchors.out.value().time[j];
						float pos_anchor_out_y = cursor_pos.y + ctx->graphTransformVertical->getPosScreen_lerped(keyframes[i].anchors.out.value().value[j]);

						ImVec2 pos_anchor_out = { pos_anchor_out_x, pos_anchor_out_y };
						drawlist->AddLine(pos_anchor_out, pos_current_keyframe, ImColor(1.f, 1.f, 0.f, 1.f), 1.f);
						drawlist->AddCircleFilled(pos_anchor_out, 2.5f, ImColor(1.f, 1.f, 0.f, 1.f));

						ImVec2 min_hitbox_out = { pos_anchor_out_x - size_offset, pos_anchor_out_y - size_offset };
						ImVec2 max_hitbox_out = { pos_anchor_out_x + size_offset, pos_anchor_out_y + size_offset };
						if (ImGui::IsMouseHoveringRect(min_hitbox_out, max_hitbox_out)) {
							CursorManager::getInstance().SetCursor(TypeCursor::HAND_OPEN);
							if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
								kf_DragHandler->BeginDrag(mouse_time_global_x, mouse_time_global_y, j, current, KeyframeDragMode::DragAnchorOut);
							}
						}
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





		}, variant_prop);




	}

	DebugOverlay::AddText("isDragAll" + std::to_string(isDragAll), ImColor{ 1.f,1.f,1.f,1.f });

	if (ImGui::IsMouseDown(ImGuiMouseButton_Left) == false) {
		isDragAll = false;
	}

	// draw selected zone
	if (isHaveSelectedMoreThenOne) {

		float min_pos_x = cursor_pos.x + ctx->horizontalTransform->getPostionScreen_lerped(selected_min_time);
		float max_pos_x = cursor_pos.x + ctx->horizontalTransform->getPostionScreen_lerped(selected_max_time);

		float min_pos_y = cursor_pos.y + ctx->graphTransformVertical->getPosScreen_lerped(selected_max_value);
		float max_pos_y = cursor_pos.y + ctx->graphTransformVertical->getPosScreen_lerped(selected_min_value);

		static const float padding = 6.f;

		ImVec2 r_min = { min_pos_x - padding, min_pos_y - padding };
		ImVec2 r_max = { max_pos_x + padding, max_pos_y + padding };

		float r_mid_y = (min_pos_y + max_pos_y) / 2.f;
		float r_mid_x = (min_pos_x + max_pos_x) / 2.f;


		ImVec2 s_left_min = { min_pos_x - padding - size_offset, r_mid_y - size_offset };
		ImVec2 s_left_max = { min_pos_x - padding + size_offset, r_mid_y + size_offset };
		bool is_left_drag_scale_hovered = ImGui::IsMouseHoveringRect(s_left_min, s_left_max);
		is_left_drag_scale_hovered |= ImGui::IsMouseHoveringRect(
			{ min_pos_x - padding - size_offset, min_pos_y - size_offset },
			{ min_pos_x - padding + size_offset, max_pos_y + size_offset });

		if (is_left_drag_scale_hovered) {
			s_left_min.x -= 1.f;
			s_left_min.y -= 1.f;
			s_left_max.x += 1.f;
			s_left_max.y += 1.f;
		}

		ImVec2 s_right_min = { max_pos_x + padding - size_offset, r_mid_y - size_offset };
		ImVec2 s_right_max = { max_pos_x + padding + size_offset, r_mid_y + size_offset };
		bool is_right_drag_scale_hovered = ImGui::IsMouseHoveringRect(s_right_min, s_right_max);
		is_right_drag_scale_hovered |= ImGui::IsMouseHoveringRect(
			{ max_pos_x + padding - size_offset, min_pos_y - size_offset },
			{ max_pos_x + padding + size_offset, max_pos_y + size_offset });

		if (is_right_drag_scale_hovered) {
			s_right_min.x -= 1.f;
			s_right_min.y -= 1.f;
			s_right_max.x += 1.f;
			s_right_max.y += 1.f;
		}


		ImVec2 s_up_min = { r_mid_x - size_offset, min_pos_y  - size_offset - padding };
		ImVec2 s_up_max = { r_mid_x + size_offset, min_pos_y  + size_offset - padding };
		bool is_up_drag_scale_hovered = ImGui::IsMouseHoveringRect(s_up_min, s_up_max);
		is_up_drag_scale_hovered |= ImGui::IsMouseHoveringRect(
			{ min_pos_x - padding - size_offset, min_pos_y - size_offset - padding },
			{ max_pos_x + padding + size_offset, min_pos_y + size_offset - padding });

		if (is_up_drag_scale_hovered) {
			s_up_min.x -= 1.f;
			s_up_min.y -= 1.f;
			s_up_max.x += 1.f;
			s_up_max.y += 1.f;
		}

		ImVec2 s_down_min = { r_mid_x - size_offset, max_pos_y - size_offset + padding };
		ImVec2 s_down_max = { r_mid_x + size_offset, max_pos_y + size_offset + padding };
		bool is_down_drag_scale_hovered = ImGui::IsMouseHoveringRect(s_down_min, s_down_max);
		is_down_drag_scale_hovered |= ImGui::IsMouseHoveringRect(
			{ min_pos_x - padding - size_offset, max_pos_y - size_offset + padding },
			{ max_pos_x + padding + size_offset, max_pos_y + size_offset + padding });

		if (is_down_drag_scale_hovered) {
			s_down_min.x -= 1.f;
			s_down_min.y -= 1.f;
			s_down_max.x += 1.f;
			s_down_max.y += 1.f;
		}




		// background selection
		drawlist->AddRectFilled(r_min, r_max, ImColor(1.f, 1.f, 1.f, 0.1f), 2.f);
		// outline selection
		drawlist->AddRect(r_min, r_max, ImColor(1.f, 1.f, 1.f, 0.4f), 2.f, 0.f, 2.f);


		// left drag scale
		drawlist->AddRectFilled(s_left_min, s_left_max, ImColor(1.f, 1.f, 1.f, 1.0f), 0.f);
		// right drag scale 
		drawlist->AddRectFilled(s_right_min, s_right_max, ImColor(1.f, 1.f, 1.f, 1.0f), 0.f);
		// up drag scale 
		drawlist->AddRectFilled(s_up_min, s_up_max, ImColor(1.f, 1.f, 1.f, 1.0f), 0.f);
		// down drag scale 
		drawlist->AddRectFilled(s_down_min, s_down_max, ImColor(1.f, 1.f, 1.f, 1.0f), 0.f);


		if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !kf_DragHandler->IsDragging()) {
			if (is_left_drag_scale_hovered) {
				kf_DragHandler->BeginDrag(mouse_time_global_x, mouse_time_global_y, -1, *ctx->selector->getKeyframes().begin(), KeyframeDragMode::ScaleLeft);
			}
			else if (is_right_drag_scale_hovered) {
				kf_DragHandler->BeginDrag(mouse_time_global_x, mouse_time_global_y, -1, *ctx->selector->getKeyframes().begin(), KeyframeDragMode::ScaleRight);
			}
			else if (is_up_drag_scale_hovered) {
				kf_DragHandler->BeginDrag(mouse_time_global_x, mouse_time_global_y, -1, *ctx->selector->getKeyframes().begin(), KeyframeDragMode::ScaleUp);
			}
			else if (is_down_drag_scale_hovered) {
				kf_DragHandler->BeginDrag(mouse_time_global_x, mouse_time_global_y, -1, *ctx->selector->getKeyframes().begin(), KeyframeDragMode::ScaleDown);
			}
			else if (ImGui::IsMouseHoveringRect(r_min, r_max)) {
				kf_DragHandler->BeginDrag(mouse_time_global_x, mouse_time_global_y, -1, *ctx->selector->getKeyframes().begin(), KeyframeDragMode::TranslateXY);
			}

			if (kf_DragHandler->IsDragging())
				isDragAll = true;
		}




	}

}

void Timeline_Graph::DrawSpeedGraph(ImVec2 p_min, ImVec2 p_max) {
	auto drawlist = ImGui::GetWindowDrawList();
	ImVec2 cursor_pos = ImGui::GetCursorScreenPos();
	ImVec2 region_max = ImGui::GetContentRegionAvail();

	if (ctx->layersData->GetData().layers.empty() || ctx->selector->getProperties().empty())
		return;

	const auto props = ctx->selector->getProperties();

	float min_vertical_bound = ctx->graphTransformVertical->getMinBoundView_lerped();
	float max_vertical_bound = ctx->graphTransformVertical->getMaxBoundView_lerped();

	float height_vertical_bound = (max_vertical_bound - min_vertical_bound);
	float padding_vertical_bound = height_vertical_bound * (1.f - eval(0.9f));

	min_vertical_bound += padding_vertical_bound / 2.f;
	max_vertical_bound -= padding_vertical_bound / 2.f;

	for (const auto& selected_property : props) {
		const auto& id_layer = selected_property.layer.id;
		const auto& name_property = selected_property.name;

		auto* layerObject = ctx->layersData->GetLayerByID(id_layer).get();
		float time_begin = layerObject->GetLayerInfo().time.Begin;

		auto& variant_prop = layerObject->GetProperties().getVariant(name_property);

		std::visit([&](auto& propertie) {
			const auto& speed_samples = propertie.kr.getSpeedSamples();

			const float& min_speed = propertie.kr.getMinSpeed();
			const float& max_speed = propertie.kr.getMaxSpeed();

			float height_speed = max_speed - min_speed;

			

			for (size_t i = 1; i < speed_samples.size(); i++)
			{
				for (size_t j = 0; j < speed_samples[i].time.size(); j++) {
					float t_curr = speed_samples[i].time[j];
					float t_prev = speed_samples[i - 1].time[j];

					float v_curr_norm = (speed_samples[i].value[j] - min_speed) / height_speed;
					float v_prev_norm = (speed_samples[i - 1].value[j] - min_speed) / height_speed;

					float v_curr = min_vertical_bound + v_curr_norm * (max_vertical_bound - min_vertical_bound);
					float v_prev = min_vertical_bound + v_prev_norm * (max_vertical_bound - min_vertical_bound);

					float pos_x = cursor_pos.x + ctx->horizontalTransform->getPostionScreen_lerped(time_begin + t_curr);
					float pos_y = cursor_pos.y + ctx->graphTransformVertical->getPosScreen_lerped(v_curr);

					float prev_pos_x = cursor_pos.x + ctx->horizontalTransform->getPostionScreen_lerped(time_begin + t_prev);
					float prev_pos_y = cursor_pos.y + ctx->graphTransformVertical->getPosScreen_lerped(v_prev);
					
					ImColor color_line = ctx->colors_params[j];
					color_line.Value.w = 0.25f;

					ImVec2 b = { prev_pos_x, prev_pos_y };
					ImVec2 e = { pos_x, pos_y };

					float line_thickness = 2.f;

					drawlist->AddLine(b, e, color_line, line_thickness);
				}
			}

		}, variant_prop);

	}

}


void Timeline_Graph::Draw() {

	auto drawlist = ImGui::GetWindowDrawList();
	ImVec2 cursor_pos = ImGui::GetCursorScreenPos();
	ImVec2 region_max = ImGui::GetContentRegionAvail();

	height_menu_buttons = eval(41.0f);

	region_max.y -= height_menu_buttons;

	ImVec2 p_min = cursor_pos;
	ImVec2 p_max = { cursor_pos.x + region_max.x, cursor_pos.y + region_max.y };

	ctx->graphTransformVertical->updateAvailableScreenValue(region_max.y);

	MyButtonBehavior(p_min, p_max, "Timeline_Graph_canvas_button", isHovered, isHeld);

	isHovered = ImGui::IsItemHovered();
	scroll_zoom = ImGui::GetIO().MouseWheel;

	float mouse_real_screen = ImGui::GetMousePos().y;
	float mouse_fixed = mouse_real_screen - cursor_pos.y;
	mouse_global_y = ctx->graphTransformVertical->getPosGlobal_real(mouse_fixed);
	mouse_screen_y = ctx->graphTransformVertical->getPosScreen_real(mouse_global_y);

	mouse_global_x = ctx->horizontalTransform->getPostionGlobal_real(ImGui::GetMousePos().x - cursor_pos.x);
	mouse_screen_x = ctx->horizontalTransform->getPostionScreen_real(mouse_global_x);


	ImGui::SetCursorScreenPos(cursor_pos);

	DrawTicks(p_min, p_max);
	DrawSpeedGraph(p_min, p_max);
	DrawGraphs(p_min, p_max);
	timeCursor->Draw(p_min, p_max);

	float left_range = ctx->horizontalTransform->getPostionScreen_lerped(ctx->timelineData->GetData().left_range_repeat);
	float right_range = ctx->horizontalTransform->getPostionScreen_lerped(ctx->timelineData->GetData().right_range_repeat);

	drawlist->AddRectFilled({ p_min.x + left_range, p_min.y }, { p_min.x + right_range, p_max.y }, ImColor(1.f, 1.f, 1.f, 0.1f), 0.f, 0);
	
	auto rect = getRectGraph();

	float mouse_x_fixed = ImGui::GetMousePos().x - cursor_pos.x;
	float mouse_y_fixed = ImGui::GetMousePos().y - cursor_pos.y;
	float mouse_time_global_x = ctx->horizontalTransform->getPostionGlobal_real(mouse_x_fixed);
	float mouse_time_global_y = ctx->graphTransformVertical->getPosGlobal_real(mouse_y_fixed);

	kf_DragHandler->ProcessDrag(mouse_time_global_x, mouse_time_global_y);

	if (ImGui::IsKeyDown(ImGuiKey_LeftShift) && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
		ctx->areaSelection->SetStatus(true);
		ctx->areaSelection->SetStartMousePosition(ImGui::GetMousePos());
		ctx->areaSelection->SetAreaStart(SelectionArea_Type::SELECTION_RIGHT_HALF);
	}
	if (!ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
		ctx->areaSelection->SetStatus(false);
	}

	p_min = { p_min.x, p_max.y};
	p_max = { p_max.x, p_max.y + height_menu_buttons };

	graphButtonsDrawer->Draw(p_min, p_max);
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
		ctx->graphTransformVertical->zoomToPointByValue(mouse_global_y, 1.5f, false);
		if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl))
			ctx->horizontalTransform->zoomToPointByValue(mouse_global_x, 1.5f, false);
		
	}
	else if (scroll_zoom < 0.f) {
		ctx->graphTransformVertical->zoomToPointByValue(mouse_global_y, 0.75f, false);
		if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl))
			ctx->horizontalTransform->zoomToPointByValue(mouse_global_x, 0.75f, false);
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