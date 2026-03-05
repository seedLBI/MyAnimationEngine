#include "Timeline.Layers.Viewer.h"

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


void Timeline_Layers_Viewer::Draw() {
	ImVec2 ASize = ImGui::GetWindowSize();

	ImVec2 SizeBody{ ASize.x, ASize.y - 1 };



	auto& layers = layersData->GetData().layers;


	ImVec2 region_max = ImGui::GetContentRegionAvail();
	ImVec2 cursor_pos = ImGui::GetCursorScreenPos();


	ImVec2 p_min = cursor_pos;
	ImVec2 p_max = { cursor_pos.x + region_max.x, cursor_pos.y + region_max.y };


	float itemHeight = verticalTransform->getScaledHeightOneObject();
	float width_window = ImGui::GetWindowWidth();


	ImDrawList* draw_list = ImGui::GetWindowDrawList();


	ImVec2 p = p_min;

	for (size_t i = 0; i < layers.size(); i++) {


		float time_begin = layers[i].get()->GetLayerInfo().time.Begin;
		float time_end = layers[i].get()->GetLayerInfo().time.End;


		float screen_time_begin = horizontalTransform->getPostionScreen_lerped(time_begin);
		float screen_time_end = horizontalTransform->getPostionScreen_lerped(time_end);

		ImVec2 rect_min = { p.x + screen_time_begin, p.y };
		ImVec2 rect_max = { p.x + screen_time_end,	 p.y + itemHeight };

		draw_list->AddRectFilled(rect_min, rect_max, ImColor(0.6f, 0.1f, 0.1f, 1.f), 0.f, 0);


		p.y += itemHeight + 1.f;
	}

	timeCursor->Draw(p_min, p_max);


}

void Timeline_Layers_Viewer::Update() {

}