#include "Timeline.Panel.h"
#include "ThirdParty/ImGui/imgui.h"
#include "ThirdParty/ImGui/imgui_internal.h"

TimelinePanel::TimelinePanel(FPS_Timer* deltaTime, TimelineProviderData* timelineData, LayersDataProvider* layersData, LayerFactory* layerFactory) :
	deltaTime(deltaTime),
	timelineData(timelineData), 
	layersData(layersData),
	layerFactory(layerFactory)
{

	listScrollController = new Timeline_ListScrollController(layersData);
	upperControls = new Timeline_UpperControls(timelineData);
	layersList = new Timeline_LayersList(layersData, layerFactory, listScrollController);
	timelineEditor = new Timeline_Editor(timelineData, layersData, listScrollController);

	timelineTransform = new TimelineTransform(timelineData);



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
	timelineTransform->update(deltaTime->GetDeltaTime() * 20.f);

	layersList->Update();
	upperControls->Update();
	timelineEditor->Update();
	listScrollController->Update();

	if (ImGui::IsKeyPressed(ImGuiKey_1)) {
		timelineTransform->scaleZoomBy(0.5);
	}
	if (ImGui::IsKeyPressed(ImGuiKey_2)) {
		timelineTransform->scaleZoomBy(2.);
	}

	if (ImGui::IsKeyPressed(ImGuiKey_3)) {
		timelineTransform->addOffset(-0.5);
	}
	if (ImGui::IsKeyPressed(ImGuiKey_4)) {
		timelineTransform->addOffset(0.5);
	}






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



	auto drawlist = ImGui::GetWindowDrawList();

	ImVec2 cursor_pos = ImGui::GetCursorScreenPos();
	ImVec2 region_max = ImGui::GetContentRegionAvail();
	
	ImGui::InvisibleButton("Timeline_Ruler_canvas_button", region_max, ImGuiButtonFlags_MouseButtonLeft);


	timelineTransform->updateAvailableWidth(region_max.x);


	float mouse_real_screen = ImGui::GetMousePos().x;
	float mouse_fixed = mouse_real_screen - cursor_pos.x;
	float mouse_global = timelineTransform->getPostionGlobal_real(mouse_fixed);
	float mouse_screen = timelineTransform->getPostionScreen_real(mouse_global);
	float offset = timelineTransform->getOffset().GetReal();
	float scale = timelineTransform->getScale().GetReal();


	float scrol_zoom = ImGui::GetIO().MouseWheel;
	if (scrol_zoom > 0.f) {
		timelineTransform->zoomToPointByValue(mouse_global, 1.5f, false);
	}
	else if (scrol_zoom < 0.f) {
		timelineTransform->zoomToPointByValue(mouse_global, 0.75f, false);
	}



	std::string str_mouse = "mouse_x real screen: [" + std::to_string(mouse_real_screen) + "]";
	DebugOverlay::AddText(str_mouse, ImColor(1.f, 0.f, 0.f, 1.f));
	str_mouse = "mouse_x fixed: [" + std::to_string(mouse_fixed) + "]";
	DebugOverlay::AddText(str_mouse, ImColor(1.f, 0.f, 0.f, 1.f));
	str_mouse = "mouse_x global: [" + std::to_string(mouse_global) + "]";
	DebugOverlay::AddText(str_mouse, ImColor(1.f, 1.f, 1.f, 1.f));
	str_mouse = "mouse_x screen: [" + std::to_string(mouse_screen) + "]";
	DebugOverlay::AddText(str_mouse, ImColor(1.f, 1.f, 1.f, 1.f));
	std::string str_offset = "offset: [" + std::to_string(offset) + "]";
	DebugOverlay::AddText(str_offset, ImColor(0.2f, 1.f, 0.2f, 1.f));
	std::string str_scale = "scale: [" + std::to_string(scale) + "]";
	DebugOverlay::AddText(str_scale, ImColor(0.2f, 1.f, 0.2f, 1.f));


	bool static isDraging = false;

	if (isDraging && ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
		float delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left).x;
		timelineTransform->setOffsetDelta(delta);
	}
	else {
		isDraging = false;
		timelineTransform->applyOffsetDelta();
	}


	if (ImGui::IsItemHovered()) {
		if (ImGui::IsMouseClicked(ImGuiMouseButton_Left, false)) {
			isDraging = true;
		}

		if (ImGui::IsMouseClicked(ImGuiMouseButton_Right, false)) {
			timelineData->GetData().SetTime(long double(mouse_global));
		}

		if (ImGui::IsKeyPressed(ImGuiKey_A)) {
			timelineTransform->centerize(0.0);
		}
	}

	//timelineTransform->centerize(timelineData->GetData().currentTime);






	ImGui::SetCursorScreenPos(cursor_pos);


	ImVec2 p_min = cursor_pos;
	ImVec2 p_max = { cursor_pos.x + region_max.x, cursor_pos.y + region_max.y};

	float padding_x = 0.f;

	p_min.x += padding_x;
	p_max.x -= padding_x;




	float distance_between_Main_tick = timelineTransform->getLengthTick_lerped();


	int countMainTick_for_Render = region_max.x / distance_between_Main_tick + 1.f;


	float height_main_tick = ImGui::GetTextLineHeight();
	float height_sub_minor_tick = ImGui::GetTextLineHeight() / 3.f;
	float height_sub_major_tick = ImGui::GetTextLineHeight() / 2.f;


	float width_main_tick = 5.f;

	ImVec2 block_min = cursor_pos;
	ImVec2 block_max = { cursor_pos.x + timelineTransform->getPostionScreen_lerped(0.f), cursor_pos.y + region_max.y };

	drawlist->AddRectFilled(
		block_min,
		block_max,
		ImColor(0.f, 0.f, 1.f, 1.f), 
		0.f, 
		0.f
	);

	block_min = { cursor_pos.x + timelineTransform->getPostionScreen_lerped(timelineData->GetData().totalTime), cursor_pos.y };
	block_max = { cursor_pos.x + region_max.x, cursor_pos.y + region_max.y };

	drawlist->AddRectFilled(
		block_min,
		block_max,
		ImColor(0.f, 0.f, 1.f, 1.f),
		0.f,
		0.f
	);




	for (size_t i = 0; i < 100; i++) {

		
		double tick_value = double(i);

		if (long double (tick_value) > timelineData->GetData().totalTime + 0.0001l)
			break;

		float pos_main_tick = p_min.x + timelineTransform->getPostionScreen_lerped(tick_value);

		
		
		ImVec2 min_pos_main_tick = { pos_main_tick - width_main_tick / 2.f, p_max.y - height_main_tick };
		ImVec2 max_pos_main_tick = { pos_main_tick + width_main_tick / 2.f, p_max.y };

		drawlist->AddRectFilled(min_pos_main_tick, max_pos_main_tick, ImColor(1.f,1.f,1.f,1.f), 2.f, 0.f);


		for (size_t j = 1; j < timelineData->GetData().framerate; j++) {

			double sub_tick_value = double(j) / double(timelineData->GetData().framerate);
			double sum_tick_value = tick_value + sub_tick_value;

			if (long double(sum_tick_value) > timelineData->GetData().totalTime + 0.0001l)
				break;

			float pos_sub_tick = p_min.x + timelineTransform->getPostionScreen_lerped(sum_tick_value);

			ImVec2 min_pos_sub_tick = { pos_sub_tick - width_main_tick / 3.f, p_max.y };

			min_pos_sub_tick.y -= j % 2 == 0 ? height_sub_minor_tick : height_sub_major_tick;


			ImVec2 max_pos_sub_tick = { pos_sub_tick + width_main_tick / 3.f, p_max.y };

			drawlist->AddRectFilled(min_pos_sub_tick, max_pos_sub_tick, ImColor(1.f, 1.f, 1.f, 0.4f), 2.f, 0.f);


		}




		char buf[32];
		if (i == 0)
			snprintf(buf, sizeof(buf), "0");
		else
			snprintf(buf, sizeof(buf), "%.1f", tick_value);

		ImVec2 text_size = ImGui::CalcTextSize(buf, nullptr, true);
		float text_x = pos_main_tick - text_size.x * 0.5f;
		float text_y = min_pos_main_tick.y - text_size.y - 1.0f;

		ImU32 text_col = ImGui::GetColorU32(ImGuiCol_Text);
		drawlist->AddText(ImGui::GetFont(), ImGui::GetFontSize(), ImVec2(text_x, text_y), text_col, buf);



	}

	float pos_timeline = p_min.x + timelineTransform->getPostionScreen_lerped(timelineData->GetData().currentTime);

	float height_timeline = ImGui::GetTextLineHeight() * 2.f;

	ImVec2 min_pos_main_tick = { pos_timeline - width_main_tick / 2.5f, p_max.y - height_timeline };
	ImVec2 max_pos_main_tick = { pos_timeline + width_main_tick / 2.5f, p_max.y };


	ImColor color_main_tick_timeline = ImColor(1.f, 0.3f, 0.3f, 1.f);
	ImColor color_main_tick_timeline_transparent = ImColor(1.f, 0.3f, 0.3f, 0.f);

	drawlist->AddRectFilled(min_pos_main_tick, max_pos_main_tick, color_main_tick_timeline, 2.f, 0.f);

	min_pos_main_tick = { pos_timeline - width_main_tick * 6.f, p_max.y - height_timeline };
	max_pos_main_tick = { pos_timeline, p_max.y };

	drawlist->AddRectFilledMultiColor(
		min_pos_main_tick, max_pos_main_tick,
		color_main_tick_timeline_transparent, color_main_tick_timeline, color_main_tick_timeline, color_main_tick_timeline_transparent);
	



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




