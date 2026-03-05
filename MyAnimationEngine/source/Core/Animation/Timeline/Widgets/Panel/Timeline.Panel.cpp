#include "Timeline.Panel.h"


TimelinePanel::TimelinePanel(
	FPS_Timer* deltaTime, 
	TimelineController* timelineController,
	TimelineProviderData* timelineData, 
	LayersDataProvider* layersData, 
	LayerFactory* layerFactory) 
	 :	deltaTime(deltaTime),
		timelineController(timelineController),
		timelineData(timelineData), 
		layersData(layersData),
		layerFactory(layerFactory)
{


	timelineTransformHorizontal = new TimelineTransformHorizontal(timelineData);
	timelineTransformVertical	= new TimelineTransformVertical();


	timelineTimeCursor = new Timeline_TimeCursor(timelineData, timelineTransformHorizontal);

	scrollHorizontal = new Timeline_Scroll_Horizontal(timelineData, timelineTransformHorizontal);
	scrollVertical = new Timeline_Scroll_Vertical(layersData, timelineTransformVertical);

	upperControls = new Timeline_UpperControls(timelineData);

	timelineRuler = new Timeline_Ruler(timelineTimeCursor, timelineController, timelineData, timelineTransformHorizontal);
	timelineRange = new Timeline_Range(timelineTimeCursor, timelineData, timelineTransformHorizontal);

	layersViewer = new Timeline_Layers_Viewer(timelineTimeCursor, layersData, timelineData, timelineTransformVertical, timelineTransformHorizontal);

	layersList = new Timeline_Layers_List(layersData, timelineTransformVertical);


	layerFactory->CreateSolid(0.f, 2.f);
	layerFactory->CreateSolid(1.f, 2.f);
	layerFactory->CreateSolid(2.f, 2.f);
	layerFactory->CreateSolid(3.f, 2.f);
	layerFactory->CreateSolid(4.f, 2.f);
	layerFactory->CreateSolid(5.f, 2.f);
	layerFactory->CreateSolid(6.f, 2.f);
	layerFactory->CreateSolid(7.f, 2.f);
	layerFactory->CreateSolid(8.f, 2.f);

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

	ImGui::Begin(u8"Временная шкала");
	
		const float thickness_separator = 1.f;

		Draw_1Level(thickness_separator);
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, thickness_separator);
		Draw_2Level(thickness_separator);
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, thickness_separator);
		Draw_3Level(thickness_separator);
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, thickness_separator);
		Draw_4Level(thickness_separator);



		std::string str_s_mouse = "screen: " + std::to_string(timelineRuler->getScreenMouse());
		std::string str_g_mouse = "global: " + std::to_string(timelineRuler->getGlobalMouse());

		std::string str_left_range =  std::to_string(timelineTransformHorizontal->getLeftBoundView_global());
		std::string str_right_range = std::to_string(timelineTransformHorizontal->getRightBoundView_global());

		DebugOverlay::AddText(str_s_mouse, ImColor(1.f, 1.f, 1.f, 1.f));
		DebugOverlay::AddText(str_g_mouse, ImColor(1.f, 1.f, 1.f, 1.f));

		DebugOverlay::AddText(
			{
				{u8"Видимые границы", ImColor(1.f, 1.f, 1.f, 1.f)},
				{str_left_range, ImColor(0.f, 1.f, 0.f, 1.f)},
				{str_right_range, ImColor(1.f, 0.f, 0.f, 1.f)}
			}
		);

		DebugOverlay::AddText(
			
			{
				{u8"Время анимации ", ImColor(1.f, 1.f, 1.f, 1.f)},
				{std::to_string(timelineData->GetData().currentTime), ImColor(0.5f, 0.5f, 1.f, 1.f)}
			}
		
		);

	
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

	layersList->Draw();

	ImGui::EndChild();
}
void TimelinePanel::DrawTimelineSide_3Level(const float& height) {
	ImGui::BeginChild(u8"Timeline_TimelineSide_3Level", ImVec2(ImGui::GetContentRegionAvail().x - 20, height));

	layersViewer->Draw();

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

	timelineTimeCursor->Update(deltaTime->GetDeltaTime());

	scrollHorizontal->Update();
	scrollVertical->Update();

	timelineRuler->Update();
	timelineRange->Update();
	layersList->Update();
	layersViewer->Update();
	upperControls->Update();
}
