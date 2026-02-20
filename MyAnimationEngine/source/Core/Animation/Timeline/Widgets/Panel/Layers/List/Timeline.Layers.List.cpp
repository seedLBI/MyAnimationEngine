#include "Timeline.Layers.List.h"

Timeline_Layers_List::Timeline_Layers_List(LayersDataProvider* layersData, TimelineTransformVertical* verticalTransform)
	: layersData(layersData),
	  verticalTransform(verticalTransform)
{

}

Timeline_Layers_List::~Timeline_Layers_List()
{
}



ImVec2 Timeline_Layers_List::DrawInfoLine(const ImVec2& p) {
	ImDrawList* draw_list = ImGui::GetWindowDrawList();
	float height_object = ImGui::GetTextLineHeight();
	float width_window = ImGui::GetWindowWidth();

	ImVec2 p2(p.x + width_window, p.y + height_object);

	draw_list->AddRectFilled(p, p2, ImColor(0.3f, 0.3f, 0.3f, 1.f), 0.f, 0);




	return ImVec2(p.x, p.y + height_object);
}

ImVec2 Timeline_Layers_List::DrawObject(const ImVec2& p, std::shared_ptr<LayerObject>& object)
{
	ImDrawList* draw_list = ImGui::GetWindowDrawList();

	float height_object = verticalTransform->getScaledHeightOneObject();
	float width_window = ImGui::GetWindowWidth();

	ImVec2 p2(p.x + width_window, p.y + height_object);

	draw_list->AddRectFilled(p, p2, ImColor(0.6f, 0.1f, 0.1f, 1.f), 0.f, 0);
	//draw_list->AddRect(p, p2, ImColor(0.6f, 1.0f, 0.1f, 1.f), 0.f, 0,3.f);
	draw_list->AddText(p, ImColor(1.f, 1.f, 1.f, 1.f), object->GetLayerInfo().name.c_str());



	return ImVec2(p.x, p.y + height_object + 1.f);
}





void Timeline_Layers_List::Draw() {
	auto data = layersData->GetData().layers;

	float headerHeight = ImGui::GetTextLineHeight();

	ImGui::BeginChild("LayersList_Header", ImVec2(0, headerHeight), 0,
		ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
	{
		ImVec2 p = ImGui::GetCursorScreenPos();
		DrawInfoLine(p);
	}
	ImGui::EndChild();

	ImGui::BeginChild("LayersList_Body", ImVec2(0, 0), 0,
		ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
	{
		ImVec2 p = ImGui::GetCursorScreenPos();
		for (size_t i = 0; i < data.size(); i++) {
			p = DrawObject(p, data[i]);
		}
	}
	ImGui::EndChild();


}


void Timeline_Layers_List::Update() {

}