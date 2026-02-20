#include "Timeline.Layers.Viewer.h"

Timeline_Layers_Viewer::Timeline_Layers_Viewer(
	LayersDataProvider* layersData,
	TimelineProviderData* timelineData,
	TimelineTransformVertical* verticalTransform,
	TimelineTransformHorizontal* horizontalTransform)
	: layersData(layersData),
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


	float headerHeight = ImGui::GetTextLineHeight();

	ImGui::BeginChild("TimelineEditorBody_EmptyInfo", ImVec2(0, headerHeight), 0, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
	{
		ImVec2 p = ImGui::GetCursorScreenPos();
		ImDrawList* draw_list = ImGui::GetWindowDrawList();
		float height_object = ImGui::GetTextLineHeight();
		float width_window = ImGui::GetWindowWidth();

		ImVec2 p2(p.x + width_window, p.y + height_object);

		draw_list->AddRectFilled(p, p2, ImColor(0.3f, 0.3f, 0.3f, 1.f), 0.f, 0);
	}
	ImGui::EndChild();


	ImGui::BeginChild(u8"TimelineEditorBody", SizeBody, 0, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

	for (size_t i = 0; i < layersData->GetData().layers.size(); i++) {




	}


	ImGui::EndChild();

}

void Timeline_Layers_Viewer::Update() {

}