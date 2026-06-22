#include "Timeline.UpperControls.h"

#include "Core/Animation/Widgets/Panel/Colors/Panel.Colors.h"

#include "ThirdParty/ImGui/imgui.h"
#include "ThirdParty/ImGui/imgui_internal.h"
#include <string>

#include "Utils/Text/Text.h"

#include "FontManager/FontManager.h"
#include "CursorManager/CursorManger.h"



Timeline_UpperControls::Timeline_UpperControls(TimelineProviderData* timelineData) : timelineData(timelineData)
{

}

Timeline_UpperControls::~Timeline_UpperControls()
{

}
const bool& Timeline_UpperControls::isGraphMode() {
	return GraphMode;
}


void Timeline_UpperControls::Draw() {
	ImGui::SetCursorPosX(4.f);

	DrawTimer();
	DrawFinder();
	DrawButtons();

}

void Timeline_UpperControls::Update() {

}

std::string Timeline_UpperControls::getTimer() {
	const uint64_t framerate = timelineData->GetData().framerate;
	const uint64_t frame = timelineData->GetData().currentFrame;

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
	const uint64_t framerate = timelineData->GetData().framerate;
	const uint64_t frame = timelineData->GetData().currentFrame;

	std::string frame_str = std::to_string(frame);
	PadLeft(frame_str, '0', 5);

	frame_str += "(" + std::to_string(framerate) + u8" кадр/c)";

	return frame_str;
}


void Timeline_UpperControls::DrawTimer() {
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

		if (isHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
			isDraging = true;
			StartDrag = ImGui::GetMousePos();
			frameStartDrag = timelineData->GetData().currentFrame;
		}

		if (isDraging && ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
			CursorManager::getInstance().SetCursor(TypeCursor::RESIZE_HORIZONTAL);
			int x_delta = int(ImGui::GetMousePos().x - StartDrag.x) / 2;

			if (x_delta < 0 && abs(x_delta) > frameStartDrag)
				timelineData->GetData().SetTime((uint64_t)0);
			else
				timelineData->GetData().SetTime(frameStartDrag + x_delta);
		}
		else
			isDraging = false;




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

	ImGui::SetCursorScreenPos({ p.x + size_down_text.x, p.y });
}

void Timeline_UpperControls::DrawFinder() {

	ImVec2 size = ImGui::GetContentRegionAvail();

}
void Timeline_UpperControls::DrawButtons() {

}