#include "Timeline.Editor.h"

#include "ThirdParty/ImGui/imgui.h"
#include "ThirdParty/ImGui/imgui_internal.h"

Timeline_Editor::~Timeline_Editor() {

}

void Timeline_Editor::Draw() {

	ImVec2 ASize = ImGui::GetWindowSize();

	ImVec2 SizeBody{ ASize.x - listScrollController->GetWidth(), ASize.y - 1 };


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


	ImGui::BeginChild(u8"TimelineEditorBody", SizeBody,0, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
		
	for (size_t i = 0; i < layersData->GetData().layers.size(); i++) {




	}


	ImGui::EndChild();

	ImGui::SameLine();

	ImGui::BeginChild(u8"TimelineEditorScroll", ImVec2(0, ASize.y), 0, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
		listScrollController->Draw();
	ImGui::EndChild();

}

void Timeline_Editor::Update() {

}

Timeline_Editor::Timeline_Editor(
	FPS_Timer* deltaTime,
	TimelineTransformHorizontal* timelineTransformHorizontal,
	TimelineProviderData* timelineData,
	LayersDataProvider* layersData,
	Timeline_ListScrollController* listScrollController) : 
	deltaTime(deltaTime),
	timelineTransformHorizontal(timelineTransformHorizontal),
	timelineData(timelineData), 
	layersData(layersData), 
	listScrollController(listScrollController)
{


}
