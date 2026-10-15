#include "Timeline.UpperControls.h"

#include "Core/Animation/Widgets/Colors/Widgets.Colors.h"

#include "ThirdParty/ImGui/imgui.h"
#include "ThirdParty/ImGui/imgui_internal.h"
#include <string>

#include "Utils/Text/Text.h"
#include "Utils/ImGui/Utils.ImGui.h"

#include "FontManager/FontManager.h"
#include "CursorManager/CursorManger.h"
#include "InputManager/InputManager.h"


Timeline_UpperControls::Timeline_UpperControls(TimelineContext* ctx) : ctx(ctx)
{

}

Timeline_UpperControls::~Timeline_UpperControls()
{

}

void Timeline_UpperControls::Draw() {

	ImVec2 cursor_pos = ImGui::GetCursorScreenPos();
	ImVec2 region_max = ImGui::GetContentRegionAvail();

	p_min = cursor_pos;
	p_max = { cursor_pos.x + region_max.x, cursor_pos.y + region_max.y };

	ImGui::SetCursorPosX(4.f);
	float widht_available = ImGui::GetContentRegionAvail().x;

	widht_available -= DrawTimer();
	widht_available -= DrawFinder(widht_available - 80.f);
	DrawButtons();
}

void Timeline_UpperControls::Update() {

}

std::string Timeline_UpperControls::getTimer() {
	const uint64_t framerate = ctx->timelineData->GetData().framerate;
	const uint64_t frame  = ctx->timelineData->GetData().currentFrame;

	uint64_t sec_frame = frame % framerate;
	uint64_t min_frame = frame / framerate;
	uint64_t hou_frame = min_frame / 60ULL;
	min_frame -= hou_frame * 60ULL;

	std::string hour_str = std::to_string(hou_frame);
	std::string min_str = std::to_string(min_frame);
	std::string sec_str = std::to_string(sec_frame);

	PadLeft(hour_str, '0', 2);
	PadLeft(min_str, '0', 2);
	PadLeft(sec_str, '0', 2);

	return "0;" + hour_str + ';' + min_str + ';' + sec_str;
}

std::string Timeline_UpperControls::getFrame() {
	const uint64_t framerate = ctx->timelineData->GetData().framerate;
	const uint64_t frame  = ctx->timelineData->GetData().currentFrame;

	std::string frame_str = std::to_string(frame);
	PadLeft(frame_str, '0', 5);

	frame_str += "(" + std::to_string(framerate) + u8" кадр/c)";

	return frame_str;
}

float Timeline_UpperControls::DrawTimer() {
	float scale_upper = 1.2f;
	float scale_bottom = 0.9f;
	colorRGB color_upper(COLOR_CHANGEBLE);
	colorRGB color_bottom(COLOR_CHANGEBLE_DARK);



	ImGui::SetWindowFontScale(scale_upper);
	Push_BoldFont();
	ImVec2 size_up_text = ImGui::CalcTextSize(getTimer().c_str());
	Pop_BoldFont();

	ImGui::SetWindowFontScale(scale_bottom);
	ImVec2 size_down_text = ImGui::CalcTextSize(getFrame().c_str());
	ImGui::SetWindowFontScale(1.f);

	float TextHeight = size_up_text.y + size_down_text.y;




	ImVec2 p = ImGui::GetCursorScreenPos();
	ImGui::SetCursorScreenPos({ p.x + 5.f, p.y });
	p = ImGui::GetCursorScreenPos();

	ImGui::BeginChild("##TIMER_UPPER_CONTROL", ImVec2(size_down_text.x, 0), 0, 0); {

		ImVec2 size = ImGui::GetContentRegionAvail();

		bool isHovered = ImGui::IsMouseHoveringRect(p, { p.x + size.x, p.y + size.y });
		static bool isDraging = false;
		static ImVec2 StartDrag = { 0,0 };
		static uint64_t frameStartDrag = 0;

		if (isHovered) {
			color_upper = COLOR_CHANGEBLE_HOVERED;
			color_bottom = COLOR_CHANGEBLE_DARK_HOVERED;
		}
		if (isDraging) {
			color_upper = COLOR_CHANGEBLE_HELD;
			color_bottom = COLOR_CHANGEBLE_DARK_HELD;
		}


		static bool PortalDisabled = true;

		static int x_delta = 0;
		
		if (isHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
			isDraging = true;
			StartDrag = ImGui::GetMousePos();
			frameStartDrag = ctx->timelineData->GetData().currentFrame;

			InputManager::SetBoundPortal({ p.x+1.f, p.y}, {p.x + size.x, p.y + size.y});
			InputManager::SetPortalMode(true);

			PortalDisabled = false;
			x_delta = 0;
		}

		if (isDraging && ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
			CursorManager::getInstance().SetCursor(TypeCursor::RESIZE_HORIZONTAL);
			
			x_delta -= InputManager::GetMouseDelta().x;

			if (x_delta < 0 && abs(x_delta) > frameStartDrag)
				ctx->timelineData->GetData().SetTime((uint64_t)0);
			else
				ctx->timelineData->GetData().SetTime(frameStartDrag + x_delta);
		}
		else {
			isDraging = false;
			if (PortalDisabled == false) {
				InputManager::SetPortalMode(false);
				PortalDisabled = true;
			}
		}




		ImDrawList* drawlist = ImGui::GetWindowDrawList();

		ImGui::SetWindowFontScale(scale_upper);
		Push_BoldFont();
		drawlist->AddText(ImVec2(p.x - size_up_text.x / 2.f + size_down_text.x / 2.f, p.y + size.y / 2.f - TextHeight / 2.f), get32uint(color_upper), getTimer().c_str());
		Pop_BoldFont();

		ImGui::SetWindowFontScale(scale_bottom);
		drawlist->AddText(ImVec2(p.x, p.y + size.y / 2.f - TextHeight / 2.f + size_up_text.y - 2.f), get32uint(color_bottom), getFrame().c_str());

		ImGui::SetWindowFontScale(1.f);
	}
	ImGui::EndChild();

	ImGui::SetCursorScreenPos({ p.x + size_down_text.x + 10.f, p.y });
	return size_down_text.x + 10.f;
}

float Timeline_UpperControls::DrawFinder(float availableWidth) {
	ImVec2 p = ImGui::GetCursorScreenPos();
	ImVec2 size = ImGui::GetContentRegionAvail();
	ImDrawList* draw_list = ImGui::GetWindowDrawList();

	const float height = 20.f;

	ImVec2 rect_min = { p.x, p.y + size.y / 2.f - height / 2.f };
	ImVec2 rect_max = { p.x + availableWidth, p.y + size.y / 2.f + height / 2.f };

	static char searchBuffer[256] = "";

	static bool is_input_active = false;

	bool hovered = ImGui::IsMouseHoveringRect(rect_min, rect_max);

	ImColor outline = COLOR_BG_BUTTONS_HOVERED;
	float thikn = 1.f;

	if (hovered) {
		CursorManager::getInstance().SetCursor(TypeCursor::TEXT);
		outline = COLOR_BG_BUTTONS_HELD;
	}

	if (is_input_active) {
		outline = COLOR_CHANGEBLE;
		thikn = 2.f;
	}

	draw_list->AddRectFilled(rect_min, rect_max, COLOR_BG_BUTTONS, 4.f);
	draw_list->AddRect(rect_min, rect_max, outline, 4.f, 0, thikn);


	Push_SmallestFont();

	float text_pos_y = rect_min.y + (height - ImGui::GetTextLineHeight()) / 2.f;
	ImGui::SetCursorScreenPos({ rect_min.x + 6.f, text_pos_y });
	ImGui::SetNextItemWidth(availableWidth - 12.f);

	ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0, 0, 0, 0));
	ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0, 0, 0, 0));
	ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0, 0, 0, 0));
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));


	if (ImGui::InputText("##finder_input", searchBuffer, IM_ARRAYSIZE(searchBuffer))) {
		ctx->layersData->setFindText(std::string(searchBuffer));
	}

	is_input_active = ImGui::IsItemActive();

	ImGui::PopStyleVar();
	ImGui::PopStyleColor(3);

	Pop_SmallestFont();


	ImGui::SetCursorScreenPos({ p.x + availableWidth, p.y });
	return availableWidth;
}

float Timeline_UpperControls::DrawButtons() {
	ImDrawList* draw_list = ImGui::GetWindowDrawList();
	ImVec2 p = ImGui::GetCursorScreenPos();
	ImVec2 size = ImGui::GetContentRegionAvail();

	float width_button = 26.f;
	float gap_between = 5.f;
	float all_width = width_button * 2.f + gap_between;

	ImVec2 mb_min = { p.x + size.x / 2.f - all_width / 2.f,					p.y + size.y / 2.f - width_button/2.f };
	ImVec2 mb_max = { p.x + size.x / 2.f - all_width / 2.f + width_button,	p.y + size.y / 2.f + width_button/2.f };

	bool mb_hovered, mb_held;

	if (MyButtonBehavior(mb_min, mb_max, "MOTION_BLUR_TOGGLE", mb_hovered, mb_held)) {
		ctx->statusGlobalButtons->SetFlag_MotionBlurMode(!ctx->statusGlobalButtons->isMotionBlurMode());
	}

	ImColor bg_mb = ImColor(0.f, 0.f, 0.f, 0.f);
	ImColor text_mb = COLOR_DEFAULT;


	if (ctx->statusGlobalButtons->isMotionBlurMode()) {
		bg_mb = COLOR_BG_BUTTONS_HELD;
		text_mb = ImColor(1.f, 1.f, 1.f, 1.f);
	}

	if (mb_hovered) {
		if (ctx->statusGlobalButtons->isMotionBlurMode())
			bg_mb = COLOR_DEFAULT;
		else
			bg_mb = COLOR_BG_BUTTONS_HOVERED;

		text_mb = ImColor(1.f,1.f,1.f,1.f);
	}

	ImVec2 text_size = ImGui::CalcTextSize(ICON_FA_CIRCLE);

	

	draw_list->AddRectFilled(mb_min, mb_max, bg_mb, 5.f);
	draw_list->AddText({ mb_min.x + width_button/2.f - text_size.x/2.f, mb_min.y + width_button/2.f - text_size.y/2.f - 1.f}, text_mb, ICON_FA_CIRCLE);



	ImVec2 mg_min = { p.x + size.x / 2.f - all_width / 2.f + width_button + gap_between,				p.y + size.y / 2.f - width_button / 2.f };
	ImVec2 mg_max = { p.x + size.x / 2.f - all_width / 2.f + width_button + gap_between + width_button,	p.y + size.y / 2.f + width_button / 2.f };

	bool mg_hovered, mg_held;

	if (MyButtonBehavior(mg_min, mg_max, "GRAPH_MODE_TOGGLE", mg_hovered, mg_held)) {
		ctx->statusGlobalButtons->SetFlag_GraphMode(!ctx->statusGlobalButtons->isGraphMode());
	}

	ImColor bg_mg = ImColor(0.f, 0.f, 0.f, 0.f);
	ImColor text_mg = COLOR_DEFAULT;


	if (ctx->statusGlobalButtons->isGraphMode()) {
		bg_mg = COLOR_BG_BUTTONS_HELD;
		text_mg = ImColor(1.f, 1.f, 1.f, 1.f);
	}

	if (mg_hovered) {
		if (ctx->statusGlobalButtons->isGraphMode())
			bg_mg = COLOR_DEFAULT;
		else
			bg_mg = COLOR_BG_BUTTONS_HOVERED;

		text_mg = ImColor(1.f, 1.f, 1.f, 1.f);
	}

	ImVec2 text_mg_size = ImGui::CalcTextSize(ICON_FA_CHART_LINE);

	draw_list->AddRectFilled(mg_min, mg_max, bg_mg, 5.f);
	draw_list->AddText({ mg_min.x + width_button / 2.f - text_mg_size.x / 2.f, mg_min.y + width_button / 2.f - text_mg_size.y / 2.f - 1.f }, text_mg, ICON_FA_CHART_LINE);

	return 0.f;
}