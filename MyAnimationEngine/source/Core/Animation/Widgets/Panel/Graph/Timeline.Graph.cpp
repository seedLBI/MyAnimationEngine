#include "Timeline.Graph.h"

Timeline_Graph::Timeline_Graph(LayersDataProvider* layersData,
	TimelineProviderData* timelineData,
	TimelineController* timelineController,
	TimelineTransformHorizontal* horizontalTransform,
	HorizontalTickRenderer* horizontalTickRenderer)
	:
	layersData(layersData),
	timelineData(timelineData),
	timelineController(timelineController),
	horizontalTransform(horizontalTransform),
	horizontalTickRenderer(horizontalTickRenderer)
{
	verticalTransform = new Graph_VerticalTransform();
}

Timeline_Graph::~Timeline_Graph() {

}

void Timeline_Graph::Draw() {
	auto drawlist = ImGui::GetWindowDrawList();
	ImVec2 cursor_pos = ImGui::GetCursorScreenPos();
	ImVec2 region_max = ImGui::GetContentRegionAvail();

	ImGui::InvisibleButton("Timeline_Graph_canvas_button", region_max, ImGuiButtonFlags_MouseButtonLeft);

	//isHovered = ImGui::IsItemHovered();
	//scrol_zoom = ImGui::GetIO().MouseWheel;

	ImGui::SetCursorScreenPos(cursor_pos);

	ImVec2 p_min = cursor_pos;
	ImVec2 p_max = { cursor_pos.x + region_max.x, cursor_pos.y + region_max.y };

	horizontalTickRenderer->DrawTicks(
		p_min,
		p_max,
		ImColor(0.4f, 0.4f, 0.4f, 1.f), ImColor(0.3f, 0.3f, 0.3f, 1.f),
		1.5f, 1.f,
		region_max.y, region_max.y, region_max.y,
		false
	);


}

void Timeline_Graph::Update() {

}