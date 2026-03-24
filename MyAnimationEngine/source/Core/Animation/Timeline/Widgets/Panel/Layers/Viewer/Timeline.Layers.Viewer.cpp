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


	float left_range  = horizontalTransform->getPostionScreen_lerped(timelineData->GetData().left_range_repeat);
	float right_range = horizontalTransform->getPostionScreen_lerped(timelineData->GetData().right_range_repeat);

	ImDrawList* draw_list = ImGui::GetWindowDrawList();


	ImVec2 p = p_min;

	for (size_t i = 0; i < layers.size(); i++) {

		auto layer = layers[i].get();
		auto& layerInfo = layer->GetLayerInfo();

		float time_begin = layerInfo.time.Begin;
		float time_end = layerInfo.time.End;

		float screen_time_begin = horizontalTransform->getPostionScreen_lerped(time_begin);
		float screen_time_end = horizontalTransform->getPostionScreen_lerped(time_end);

		ImVec2 rect_min = { p.x + screen_time_begin, p.y };
		ImVec2 rect_max = { p.x + screen_time_end,	 p.y + itemHeight };

		draw_list->AddRectFilled(rect_min, rect_max, ImColor(layerInfo.color.r, layerInfo.color.g, layerInfo.color.b, 1.f), 0.f, 0);

		colorRGB out;
		if (getLuminance(layerInfo.color) > 0.6f) {
			out = lerp(layerInfo.color, colorRGB{ 0,0,0 }, 0.55f);
		}
		else {
			out = lerp(layerInfo.color, colorRGB{ 1,1,1 }, 0.45f);
		}


		draw_list->AddRect(rect_min, rect_max, ImColor(out.r, out.g, out.b, 1.f), 0.f, 0,2.f);

		draw_list->AddLine({ p_min.x,rect_max.y }, { p_max.x,rect_max.y }, ImColor(0.3f, 0.3f, 0.3f), 1.f);


		p.y += itemHeight + 1.f;
	}


	draw_list->AddRectFilled({ p_min.x + left_range, p_min.y}, { p_min.x + right_range, p_max.y }, ImColor(1.f, 1.f, 1.f, 0.1f), 0.f, 0);



	timeCursor->Draw(p_min, p_max);





}

void Timeline_Layers_Viewer::Update() {

}