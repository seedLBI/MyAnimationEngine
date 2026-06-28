#include "Timeline.Graph.h"

#include "Core/DebugOverlay/DebugOverlay.h"


Timeline_Graph::Timeline_Graph(Timeline_TimeCursor* timeCursor,
	LayersDataProvider* layersData,
	TimelineProviderData* timelineData,
	TimelineController* timelineController,
	TimelineTransformHorizontal* horizontalTransform,
	TransformScaled* verticalTransform,
	HorizontalTickRenderer* horizontalTickRenderer)
	:
	timeCursor(timeCursor),
	layersData(layersData),
	timelineData(timelineData),
	timelineController(timelineController),
	horizontalTransform(horizontalTransform),
	verticalTransform(verticalTransform),
	horizontalTickRenderer(horizontalTickRenderer)
{
	
	verticalTransform->setBaseStep(-100.f);
	//verticalTransform.setBoundsOffset(-100.f, 100.f);
	verticalTransform->setBoundsScale(0.001f, 1000.f);
	

}

Timeline_Graph::~Timeline_Graph() {

}

void Timeline_Graph::NeedCenterize() {
	this->NeedCenterized = true;
}


void Timeline_Graph::Draw() {
	auto drawlist = ImGui::GetWindowDrawList();
	ImVec2 cursor_pos = ImGui::GetCursorScreenPos();
	ImVec2 region_max = ImGui::GetContentRegionAvail();

	ImVec2 p_min = cursor_pos;
	ImVec2 p_max = { cursor_pos.x + region_max.x, cursor_pos.y + region_max.y };

	verticalTransform->updateAvailableScreenValue(region_max.y);
	
	MyButtonBehavior(p_min, p_max, "Timeline_Graph_canvas_button", isHovered, isHeld);

	isHovered = ImGui::IsItemHovered();
	scroll_zoom = ImGui::GetIO().MouseWheel;


	float mouse_real_screen = ImGui::GetMousePos().y;
	float mouse_fixed = mouse_real_screen - cursor_pos.y;
	mouse_global = verticalTransform->getPosGlobal_real(mouse_fixed);
	mouse_screen = verticalTransform->getPosScreen_real(mouse_global);

	ImGui::SetCursorScreenPos(cursor_pos);



	horizontalTickRenderer->DrawTicks(
		p_min, p_max,
		ImColor(0.4f, 0.4f, 0.4f, 1.f), ImColor(0.3f, 0.3f, 0.3f, 1.f),
		1.5f, 1.f,
		region_max.y, region_max.y, region_max.y,
		false,true
	);


	//float pos_y_center_line     = cursor_pos.y + verticalTransform->getPosScreen_lerped(0.f);


	DebugOverlay::AddText("  up_bound" + std::to_string(verticalTransform->getMaxBoundView_lerped()), ImColor(1.f, 1.f, 1.f, 1.f));
	DebugOverlay::AddText("down_bound" + std::to_string(verticalTransform->getMinBoundView_lerped()), ImColor(1.f, 1.f, 1.f, 1.f));


	float pixels_per_unit = std::abs(verticalTransform->getLengthTick_lerped());

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

	float pos_y_center_line = cursor_pos.y + verticalTransform->getPosScreen_lerped(0.f);
	drawlist->AddLine({ p_min.x, pos_y_center_line }, { p_max.x, pos_y_center_line }, ImColor(0.4f, 0.4f, 0.4f, 1.f), 3.f);

	uint64_t count_ticks = 1;

	float bound_a = verticalTransform->getMinBoundView_lerped();
	float bound_b = verticalTransform->getMaxBoundView_lerped();
	float v_min = std::min(bound_a, bound_b);
	float v_max = std::max(bound_a, bound_b);

	float start_val = std::floor(v_min / tick_step) * tick_step;

	for (float val = start_val; val <= v_max + tick_step; val += tick_step) {
		if (std::abs(val) > 0.0001f * tick_step) {
			float pos = cursor_pos.y + verticalTransform->getPosScreen_lerped(val);

			if (pos >= p_min.y && pos <= p_max.y) {
				ImColor main_color = ImColor(0.4f, 0.4f, 0.4f, 1.f);
				drawlist->AddLine({ p_min.x, pos }, { p_max.x, pos }, main_color,2.f);

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

			float pos_sub = cursor_pos.y + verticalTransform->getPosScreen_lerped(sub_val);

			if (pos_sub >= p_min.y && pos_sub <= p_max.y) {
				drawlist->AddLine({ p_min.x, pos_sub }, { p_max.x, pos_sub }, ImColor(0.3f, 0.3f, 0.3f, 1.f), 1.f);
				count_ticks++;
			}
		}
	}

	DebugOverlay::AddText("rendered " + std::to_string(count_ticks), ImColor(1.f, 0.5f, 0.5f, 1.f));


	auto& color		= layersData->GetData().layers.front().get()->GetProperties().get<TPColor>("color");
	auto& keyframes = color.kr.getKeyframes();

	float size_keyframe = 10.f;

	ImColor colors[3] = {
		ImColor(1.f,0.3f,0.3f,1.f),
		ImColor(0.3f,1.f,0.3f,1.f),
		ImColor(0.3f,0.3f,1.f,1.f)
	};

	for (size_t i = 0; i < keyframes.size(); i++) {
		
		float pos_x = cursor_pos.x + horizontalTransform->getPostionScreen_lerped(keyframes[i].sample.time);

		for (size_t j = 0; j < keyframes[i].sample.value.size(); j++)
		{
			float pos_y = cursor_pos.y + verticalTransform->getPosScreen_lerped(keyframes[i].sample.value[j]);






			if (i != 0) {
				float prev_pos_x = cursor_pos.x + horizontalTransform->getPostionScreen_lerped(keyframes[i - 1].sample.time);
				float prev_pos_y = cursor_pos.y + verticalTransform->getPosScreen_lerped(keyframes[i - 1].sample.value[j]);

				drawlist->AddLine({ prev_pos_x, prev_pos_y }, { pos_x,pos_y }, (j<=2? colors[j]: ImColor(0.5f, 0.5f, 0.2f, 1.f)), 2.f);
			}

			drawlist->AddRectFilled(
				{ pos_x - size_keyframe / 2.f, pos_y - size_keyframe / 2.f },
				{ pos_x + size_keyframe / 2.f, pos_y + size_keyframe / 2.f },
				ImColor(1.f, 1.f, 0.f, 1.f));





		}

		


	}

	timeCursor->Draw(p_min, p_max);
}

void Timeline_Graph::Update(const float& dt) {

	if (NeedCenterized) {
		if (verticalTransform->getAvailableScreenValue() != 0.f) {
			verticalTransform->centerize(0.f);
			NeedCenterized = false;
		}
	}
	verticalTransform->update(dt * 20.f);



	static bool isOfsetDeltaApplied = false;

	if (isDraging && ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
		isOfsetDeltaApplied = true;
		float delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left).y;
		verticalTransform->setOffsetDelta(delta);
	}
	else {
		isDraging = false;
		if (isOfsetDeltaApplied)
			verticalTransform->applyOffsetDelta();
		isOfsetDeltaApplied = false;
	}

	if (!isHovered) return;

	if (scroll_zoom > 0.f) {
		verticalTransform->zoomToPointByValue(mouse_global, 1.5f, false);
	}
	else if (scroll_zoom < 0.f) {
		verticalTransform->zoomToPointByValue(mouse_global, 0.75f, false);
	}

	if (ImGui::IsMouseClicked(ImGuiMouseButton_Left, false)) {
		isDraging = true;
	}

	if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
		verticalTransform->centerize(0.f);
	}





}