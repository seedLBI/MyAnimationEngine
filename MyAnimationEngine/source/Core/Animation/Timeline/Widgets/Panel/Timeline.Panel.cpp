#include "Timeline.Panel.h"
#include "ThirdParty/ImGui/imgui.h"
#include "ThirdParty/ImGui/imgui_internal.h"

TimelinePanel::TimelinePanel(FPS_Timer* deltaTime, TimelineProviderData* timelineData, LayersDataProvider* layersData, LayerFactory* layerFactory) :
	deltaTime(deltaTime),
	timelineData(timelineData), 
	layersData(layersData),
	layerFactory(layerFactory)
{


	timelineTransformHorizontal = new TimelineTransformHorizontal(timelineData);
	timelineTransformVertical	= new TimelineTransformVertical();

	scrollHorizontal = new Timeline_Scroll_Horizontal(timelineData, timelineTransformHorizontal);
	scrollVertical = new Timeline_Scroll_Vertical(layersData, timelineTransformVertical);

	upperControls = new Timeline_UpperControls(timelineData);

	timelineRuler = new Timeline_Ruler(timelineData, timelineTransformHorizontal);
	timelineRange = new Timeline_Range(timelineData, timelineTransformHorizontal);

	layersList = new Timeline_Layers_List(layersData, timelineTransformVertical);
	layersViewer = new Timeline_Layers_Viewer(layersData, timelineData, timelineTransformVertical, timelineTransformHorizontal);

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
	delete layersViewer;
	delete timelineRuler;
	delete timelineRange;
	delete upperControls;

	delete scrollHorizontal;
	delete scrollVertical;

	delete timelineTransformHorizontal;
	delete timelineTransformVertical;

}

void TimelinePanel::Draw() {
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

	if (ImGui::Begin(u8"TimelinePanel",0,ImGuiWindowFlags_NoScrollbar)) {

		const float thickness_separator = 1.f;

		Draw_1Level(thickness_separator);
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, thickness_separator);
		Draw_2Level(thickness_separator);
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, thickness_separator);
		Draw_3Level(thickness_separator);
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, thickness_separator);
		Draw_4Level(thickness_separator);

		
	}
	ImGui::End();

	ImGui::PopStyleVar(2);

}

void TimelinePanel::Draw_1Level(const float& thickness_separator) {

	float height = ImGui::GetTextLineHeight() * 2.f;

	DrawLayerSide_1Level(height);

	ImGui::SameLine();
	ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical, thickness_separator);
	ImGui::SameLine();

	DrawTimelineSide_1Level(height);

	ImGui::SameLine();
	ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical, thickness_separator);
	ImGui::SameLine();

	DrawVerticalSliderBox_1Level(height);

}
void TimelinePanel::DrawLayerSide_1Level(const float& height) {
	ImGui::BeginChild(u8"Timeline_LayerSide_1Level", ImVec2(500, height));

	upperControls->Draw();

	ImGui::EndChild();
}
void TimelinePanel::DrawTimelineSide_1Level(const float& height) {
	ImGui::BeginChild(u8"Timeline_TimelineSide_1Level", ImVec2(ImGui::GetContentRegionAvail().x - 20, height));

	timelineRuler->Draw();

	ImGui::EndChild();
}
void TimelinePanel::DrawVerticalSliderBox_1Level(const float& height) {
	ImGui::BeginChild(u8"Timeline_SliderSide_1Level", ImVec2(20, height));

	

	ImGui::EndChild();
}


void TimelinePanel::Draw_2Level(const float& thickness_separator) {
	float height = ImGui::GetTextLineHeight();

	DrawLayerSide_2Level(height);

	ImGui::SameLine();
	ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical, thickness_separator);
	ImGui::SameLine();

	DrawTimelineSide_2Level(height);

	ImGui::SameLine();
	ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical, thickness_separator);
	ImGui::SameLine();

	DrawVerticalSliderBox_2Level(height);

}
void TimelinePanel::DrawLayerSide_2Level(const float& height) {

	ImGui::BeginChild(u8"Timeline_LayerSide_2Level", ImVec2(500, height));



	ImGui::EndChild();

}
void TimelinePanel::DrawTimelineSide_2Level(const float& height) {
	ImGui::BeginChild(u8"Timeline_TimelineSide_2Level", ImVec2(ImGui::GetContentRegionAvail().x - 20, height));

	timelineRange->Draw();

	ImGui::EndChild();
}
void TimelinePanel::DrawVerticalSliderBox_2Level(const float& height) {
	ImGui::BeginChild(u8"Timeline_SliderSide_2Level", ImVec2(20, height));



	ImGui::EndChild();
}


void TimelinePanel::Draw_3Level(const float& thickness_separator) {
	float height = ImGui::GetContentRegionAvail().y - 20.f;

	DrawLayerSide_3Level(height);

	ImGui::SameLine();
	ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical, thickness_separator);
	ImGui::SameLine();

	DrawTimelineSide_3Level(height);

	ImGui::SameLine();
	ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical, thickness_separator);
	ImGui::SameLine();

	DrawVerticalSliderBox_3Level(height);
}
void TimelinePanel::DrawLayerSide_3Level(const float& height) {
	ImGui::BeginChild(u8"Timeline_LayerSide_3Level", ImVec2(500, height));



	ImGui::EndChild();
}
void TimelinePanel::DrawTimelineSide_3Level(const float& height) {
	ImGui::BeginChild(u8"Timeline_TimelineSide_3Level", ImVec2(ImGui::GetContentRegionAvail().x - 20, height));



	ImGui::EndChild();
}
void TimelinePanel::DrawVerticalSliderBox_3Level(const float& height) {
	ImGui::BeginChild(u8"Timeline_SliderSide_3Level", ImVec2(20, height));

	scrollVertical->Draw();

	ImGui::EndChild();
}


void TimelinePanel::Draw_4Level(const float& thickness_separator) {
	float height = 20.f;

	DrawLayerSide_4Level(height);

	ImGui::SameLine();
	ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical, thickness_separator);
	ImGui::SameLine();

	DrawTimelineSide_4Level(height);

	ImGui::SameLine();
	ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical, thickness_separator);
	ImGui::SameLine();

	DrawVerticalSliderBox_4Level(height);
}
void TimelinePanel::DrawLayerSide_4Level(const float& height) {
	ImGui::BeginChild(u8"Timeline_LayerSide_4Level", ImVec2(500, height));



	ImGui::EndChild();
}
void TimelinePanel::DrawTimelineSide_4Level(const float& height) {
	ImGui::BeginChild(u8"Timeline_TimelineSide_4Level", ImVec2(ImGui::GetContentRegionAvail().x - 20, height));

	scrollHorizontal->Draw();

	ImGui::EndChild();
}
void TimelinePanel::DrawVerticalSliderBox_4Level(const float& height) {
	ImGui::BeginChild(u8"Timeline_SliderSide_4Level", ImVec2(20, height));



	ImGui::EndChild();
}


void TimelinePanel::Update() {
	timelineTransformHorizontal->update(deltaTime->GetDeltaTime());
	timelineTransformVertical->update(deltaTime->GetDeltaTime());

	scrollHorizontal->Update();
	scrollVertical->Update();

	timelineRuler->Update();
	timelineRange->Update();
	layersList->Update();
	layersViewer->Update();
	upperControls->Update();
}
