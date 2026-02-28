#include "Timeline.Range.h"

Timeline_Range::Timeline_Range(
	TimelineProviderData* timelineData,
	TimelineTransformHorizontal* horizontalTransform)
	: timelineData(timelineData),
	horizontalTransform(horizontalTransform)
{

}

Timeline_Range::~Timeline_Range() {

}

void Timeline_Range::Draw() {
	auto drawlist = ImGui::GetWindowDrawList();
	ImVec2 cursor_pos = ImGui::GetCursorScreenPos();
	ImVec2 region_max = ImGui::GetContentRegionAvail();

	ImGui::InvisibleButton("Timeline_Range_canvas_button", region_max, ImGuiButtonFlags_MouseButtonLeft);


	float left_range  = horizontalTransform->getPostionScreen_lerped(0.f);
	float right_range = horizontalTransform->getPostionScreen_lerped(timelineData->GetData().totalTime);

	float padding = 8.f;

	ImVec2 pmin = { cursor_pos.x + left_range, cursor_pos.y };
	ImVec2 pmax = { cursor_pos.x + right_range, cursor_pos.y + region_max.y };

	pmin.x -= padding;
	pmax.x += padding;

	drawlist->AddRectFilled(pmin, pmax, ImColor(1.f, 0.3f, 0.3f, 1.f), padding / 2.f);

	pmin.x += padding;
	pmax.x -= padding;

	drawlist->AddRectFilled(pmin, pmax, ImColor(0.4f, 0.4f, 1.0f, 1.f),0.f);







}
void Timeline_Range::Update() {

}