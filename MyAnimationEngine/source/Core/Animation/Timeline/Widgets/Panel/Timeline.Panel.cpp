#include "Timeline.Panel.h"
#include "ThirdParty/ImGui/imgui.h"
#include "ThirdParty/ImGui/imgui_internal.h"

TimelinePanel::TimelinePanel(TimelineProviderData* timelineData, LayersDataProvider* layersData, LayerFactory* layerFactory) :
	timelineData(timelineData), 
	layersData(layersData),
	layerFactory(layerFactory)
{

	listScrollController = new Timeline_ListScrollController(layersData);
	upperControls = new Timeline_UpperControls(timelineData);
	layersList = new Timeline_LayersList(layersData, layerFactory, listScrollController);
	timelineEditor = new Timeline_Editor(timelineData, layersData, listScrollController);





	layerFactory->CreateSolid();
	layerFactory->CreateSolid();
	layerFactory->CreateSolid();
	layerFactory->CreateSolid();
	layerFactory->CreateSolid();
	layerFactory->CreateSolid();
	layerFactory->CreateSolid();
	layerFactory->CreateSolid();
	layerFactory->CreateSolid();
	layerFactory->CreateSolid();
	layerFactory->CreateSolid();
	layerFactory->CreateSolid();
	layerFactory->CreateSolid();
	layerFactory->CreateSolid();
	layerFactory->CreateSolid();


}

TimelinePanel::~TimelinePanel() {
	delete layersList;
	delete listScrollController;
	delete upperControls;
	delete timelineEditor;
}

void TimelinePanel::Draw() {

	//WindowPadding iTemSpacing


	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

	if (ImGui::Begin(u8"TimelinePanel")) {

		
		DrawLeftSide();

		ImGui::SameLine();

		ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical, 3.f);

		ImGui::SameLine();

		DrawRightSide();

		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 3.f);

		DrawBottomControls();


		
	}
	ImGui::End();

	ImGui::PopStyleVar(2);

}

void TimelinePanel::Update() {
	layersList->Update();
	upperControls->Update();
	timelineEditor->Update();
	listScrollController->Update();
}

void TimelinePanel::DrawLeftSide() {
	ImGui::BeginChild(u8"Timeline_LeftSide",ImVec2(500,ImGui::GetWindowHeight() - ImGui::GetTextLineHeight()*2.5f));

		DrawUpperControls();
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 3.f);
		DrawLayerList();

	ImGui::EndChild();
}

void TimelinePanel::DrawRightSide() {
	ImGui::BeginChild(u8"Timeline_RightSide",ImVec2(0, ImGui::GetWindowHeight() - ImGui::GetTextLineHeight() * 2.5f));

		DrawRuler();
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 3.f);
		DrawEditors();

	ImGui::EndChild();
}

void TimelinePanel::DrawRuler() {
	ImGui::BeginChild(u8"Timeline_Ruler", ImVec2(0, ImGui::GetTextLineHeight() * 2.f));

	ImGui::Text("Ruler");

	ImGui::EndChild();
}

void TimelinePanel::DrawUpperControls() {
	ImGui::BeginChild(u8"Timeline_UpperControls", ImVec2(0, ImGui::GetTextLineHeight()*2.f));

	upperControls->Draw();

	ImGui::EndChild();
}

void TimelinePanel::DrawBottomControls() {
	ImGui::BeginChild(u8"Timeline_BottomControls", ImVec2(0, ImGui::GetTextLineHeight()));

	ImGui::Text("BottomControl");


	ImGui::EndChild();
}

void TimelinePanel::DrawLayerList() {
	ImGui::BeginChild(u8"Timeline_LayerList");

		layersList->Draw();

	ImGui::EndChild();
}

void TimelinePanel::DrawEditors() {
	ImGui::BeginChild(u8"Timeline_Editors",ImVec2(0,0),0, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

		timelineEditor->Draw();

	ImGui::EndChild();
}




