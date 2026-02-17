#include "Timeline.ListScrollController.h"

#include "ThirdParty/ImGui/imgui.h"
#include "ThirdParty/ImGui/imgui_internal.h"

Timeline_ListScrollController::Timeline_ListScrollController(LayersDataProvider* layersData): layersData(layersData)
{

}

Timeline_ListScrollController::~Timeline_ListScrollController()
{

}

void Timeline_ListScrollController::Draw()
{
	ImVec2 SizeWindow = ImGui::GetWindowSize();
	ImDrawList* drawlist = ImGui::GetWindowDrawList();

	SizeWindow.x -= 1;
	SizeWindow.y -= ImGui::GetTextLineHeight();

	ImVec2 p_min = ImGui::GetCursorScreenPos();
	ImVec2 p_max{ p_min.x + SizeWindow.x, p_min.y + SizeWindow.y };

	drawlist->AddRectFilled(p_min, p_max, ImColor(0, 0, 0, 255), 0);
	//drawlist->AddRect(p_min, p_max, ImColor(255, 255,255, 255), 0, 0, 1.f);

	const float padding = 2.f;
	const float heightWindow = SizeWindow.y - padding*2.f;
	const float heightLayers = ImGui::GetTextLineHeight() * layersData->GetData().layers.size();

	float fillness = heightWindow / heightLayers;
	fillness = fillness > 1.f ? 1.f : fillness;


	p_max = { p_min.x + SizeWindow.x - padding, p_min.y + SizeWindow.y * fillness - padding };
	p_min = { p_min.x + padding, p_min.y + padding };

	ImVec2 SizeSlider{ SizeWindow.x - padding, SizeWindow.y * fillness - padding };

	drawlist->AddRectFilled(p_min, p_max, ImColor(255, 0, 0, 255), 10.f);


	ImGui::SetCursorScreenPos(p_min);

	ImGui::InvisibleButton("SliderScrollTimeline", SizeSlider, ImGuiButtonFlags_PressedOnDragDropHold);
		
	
}

void Timeline_ListScrollController::Update()
{

}

float Timeline_ListScrollController::GetHeightListObject() {
	return ImGui::GetTextLineHeight();
}

float Timeline_ListScrollController::GetWidth() {
	return widthScroll;
}
