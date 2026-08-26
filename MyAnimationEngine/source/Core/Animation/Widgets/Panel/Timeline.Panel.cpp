#include "Timeline.Panel.h"
#include "Colors/Panel.Colors.h"

Timeline_Panel::Timeline_Panel(
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
	graphTransformVertical		= new TransformScaled();
	
	selector = new Timeline_Selector();
	areaSelection = new Timeline_AreaSelection();

	timelineContext = new TimelineContext();
	timelineContext->selector = selector;
	timelineContext->areaSelection = areaSelection;
	timelineContext->horizontalTransform = timelineTransformHorizontal;
	timelineContext->verticalTransform = timelineTransformVertical;
	timelineContext->graphTransformVertical = graphTransformVertical;
	timelineContext->layerFactory = layerFactory;
	timelineContext->layersData = layersData;
	timelineContext->timelineController = timelineController;
	timelineContext->timelineData = timelineData;


	horizontalTickRenderer = new HorizontalTickRenderer(timelineContext);
	timelineTimeCursor = new Timeline_TimeCursor(timelineContext);

	scrollHorizontal = new Timeline_Scroll_Horizontal(timelineContext);
	scrollVertical = new Timeline_Scroll_Vertical(timelineContext);

	upperControls = new Timeline_UpperControls(timelineContext);

	timelineRuler = new Timeline_Ruler(timelineContext, timelineTimeCursor, horizontalTickRenderer);
	timelineRange = new Timeline_Range(timelineContext, timelineTimeCursor);

	layersViewer = new Timeline_Layers_Viewer(timelineContext, timelineTimeCursor);
	layersList = new Timeline_Layers_List(timelineContext);

	graph = new Timeline_Graph(timelineContext, timelineTimeCursor, horizontalTickRenderer);
	graph->NeedCenterize();

	NoiseTexture::getInstance();
}

Timeline_Panel::~Timeline_Panel() {
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

void Timeline_Panel::Draw() {

	ImGui::PushStyleColor(ImGuiCol_WindowBg, COLOR_BG_LIST.Value);
	ImGui::PushStyleColor(ImGuiCol_Separator, COLOR_SEPARATOR.Value);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

	ImGui::Begin(u8"Временная шкала", nullptr, ImGuiWindowFlags_NoScrollWithMouse);
	
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
		std::string str_offset = "offset: " + std::to_string(timelineTransformHorizontal->getOffset().GetReal());

		std::string str_left_range =  std::to_string(timelineTransformHorizontal->getLeftBoundView_global_real());
		std::string str_right_range = std::to_string(timelineTransformHorizontal->getRightBoundView_global_real());

		DebugOverlay::AddText(str_s_mouse, ImColor(1.f, 1.f, 1.f, 1.f));
		DebugOverlay::AddText(str_g_mouse, ImColor(1.f, 1.f, 1.f, 1.f));
		DebugOverlay::AddText(str_offset, ImColor(1.f, 1.f, 1.f, 1.f));

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

		DebugOverlay::AddText(

			{
				{u8"Кадр анимации  ", ImColor(1.f, 1.f, 1.f, 1.f)},
				{std::to_string(timelineData->GetData().currentFrame), ImColor(0.5f, 0.5f, 1.f, 1.f)}
			}

		);

	
	ImGui::End();

	ImGui::PopStyleVar(2);
	ImGui::PopStyleColor(2);

}

void Timeline_Panel::Draw_1Level(const float& thickness_separator) {

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
void Timeline_Panel::DrawLayerSide_1Level(const float& height) {
	ImGui::BeginChild(u8"Timeline_LayerSide_1Level", ImVec2(layersList->getLengthColumn(), height), 0, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

	upperControls->Draw();

	ImGui::EndChild();
}
void Timeline_Panel::DrawTimelineSide_1Level(const float& height) {
	ImGui::BeginChild(u8"Timeline_TimelineSide_1Level", ImVec2(ImGui::GetContentRegionAvail().x - 20, height), 0, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

	timelineRuler->Draw();

	ImGui::EndChild();
}
void Timeline_Panel::DrawVerticalSliderBox_1Level(const float& height) {
	ImGui::BeginChild(u8"Timeline_SliderSide_1Level", ImVec2(20, height));

	

	ImGui::EndChild();
}


void Timeline_Panel::Draw_2Level(const float& thickness_separator) {
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
void Timeline_Panel::DrawLayerSide_2Level(const float& height) {

	ImGui::BeginChild(u8"Timeline_LayerSide_2Level", ImVec2(layersList->getLengthColumn(), height), 0, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

	layersList->DrawColumnsHeader();

	ImGui::EndChild();

}
void Timeline_Panel::DrawTimelineSide_2Level(const float& height) {
	ImGui::BeginChild(u8"Timeline_TimelineSide_2Level", ImVec2(ImGui::GetContentRegionAvail().x - 20, height), 0, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

	timelineRange->Draw();

	ImGui::EndChild();
}
void Timeline_Panel::DrawVerticalSliderBox_2Level(const float& height) {
	ImGui::BeginChild(u8"Timeline_SliderSide_2Level", ImVec2(20, height));



	ImGui::EndChild();
}


void Timeline_Panel::Draw_3Level(const float& thickness_separator) {
	float height = ImGui::GetContentRegionAvail().y - 20.f;
	float width = ImGui::GetContentRegionAvail().x - 20.f;

	ImVec2 clip_min = ImGui::GetCursorScreenPos();
	ImVec2 clip_max = clip_min;
	clip_max.y += height;
	clip_max.x += width;

	DrawLayerSide_3Level(height);

	ImGui::SameLine();
	ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical, thickness_separator);
	ImGui::SameLine();

	DrawTimelineSide_3Level(height);




	auto* draw_list = ImGui::GetForegroundDrawList();
	draw_list->PushClipRect(clip_min, clip_max, true);
	areaSelection->Update();
	areaSelection->Draw(draw_list);
	draw_list->PopClipRect();

	ImGui::SameLine();
	ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical, thickness_separator);
	ImGui::SameLine();

	DrawVerticalSliderBox_3Level(height);
}
void Timeline_Panel::DrawLayerSide_3Level(const float& height) {
	ImGui::BeginChild(u8"Timeline_LayerSide_3Level", ImVec2(layersList->getLengthColumn(), height), 0, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

	layersList->Draw();

	ImGui::EndChild();
}
void Timeline_Panel::DrawTimelineSide_3Level(const float& height) {
	ImGui::BeginChild(u8"Timeline_TimelineSide_3Level", ImVec2(ImGui::GetContentRegionAvail().x - 20, height), 0, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

	if (upperControls->isGraphMode())
		graph->Draw();
	else
		layersViewer->Draw();
		

	ImGui::EndChild();
}
void Timeline_Panel::DrawVerticalSliderBox_3Level(const float& height) {
	ImGui::BeginChild(u8"Timeline_SliderSide_3Level", ImVec2(20, height));

	scrollVertical->Draw();

	ImGui::EndChild();
}


void Timeline_Panel::Draw_4Level(const float& thickness_separator) {
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
void Timeline_Panel::DrawLayerSide_4Level(const float& height) {
	ImGui::BeginChild(u8"Timeline_LayerSide_4Level", ImVec2(layersList->getLengthColumn(), height));



	ImGui::EndChild();
}
void Timeline_Panel::DrawTimelineSide_4Level(const float& height) {
	ImGui::BeginChild(u8"Timeline_TimelineSide_4Level", ImVec2(ImGui::GetContentRegionAvail().x - 20, height));

	scrollHorizontal->Draw();

	ImGui::EndChild();
}
void Timeline_Panel::DrawVerticalSliderBox_4Level(const float& height) {
	ImGui::BeginChild(u8"Timeline_SliderSide_4Level", ImVec2(20, height));



	ImGui::EndChild();
}


void Timeline_Panel::Update() {

	//timelineTransformHorizontal->centerize(timelineData->GetData().currentTime, false);

	float delta_time = deltaTime->GetDeltaTime();

	timelineTransformHorizontal->update(delta_time);
	timelineTransformVertical->update(delta_time);

	timelineTimeCursor->Update(delta_time);

	scrollHorizontal->Update();
	scrollVertical->Update();

	timelineRuler->Update(delta_time);
	timelineRange->Update();
	layersList->Update();

	layersViewer->Update();
	graph->Update(delta_time);


	upperControls->Update();

}