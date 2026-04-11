#include "Timeline.UpperControls.h"


#include "ThirdParty/ImGui/imgui.h"
#include "ThirdParty/ImGui/imgui_internal.h"
#include <string>

#include "Utils/Text/Text.h"

#include "FontManager/FontManager.h"



Timeline_UpperControls::Timeline_UpperControls(TimelineProviderData* timelineData) : timelineData(timelineData)
{

}

Timeline_UpperControls::~Timeline_UpperControls()
{

}

void Timeline_UpperControls::Draw()
{
	ImVec2 p =  ImGui::GetCursorScreenPos();

	p = DrawTimer(p);
}

void Timeline_UpperControls::Update() {

}

ImVec2 Timeline_UpperControls::DrawTimer(const ImVec2& p) {

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

	std::string timer_str;
	timer_str += "0";
	timer_str += ";" + hour_str;
	timer_str += ";" + min_str;
	timer_str += ";" + sec_str;

	std::string frame_str = std::to_string(frame);
	PadLeft(frame_str, '0', 5);


	frame_str += "(" + std::to_string(framerate) + u8" кадр/c)";



	ImVec2 WindowSize = ImGui::GetWindowSize();
	ImDrawList* drawlist = ImGui::GetWindowDrawList();



	ImGui::SetWindowFontScale(scale_upper);
		Push_BoldFont();
			ImVec2 size_up_text = ImGui::CalcTextSize(timer_str.c_str());
		Pop_BoldFont();
	ImGui::SetWindowFontScale(scale_bottom);
		ImVec2 size_down_text = ImGui::CalcTextSize(frame_str.c_str());
	ImGui::SetWindowFontScale(1.f);



	ImGui::SetWindowFontScale(scale_upper);
		Push_BoldFont();
			drawlist->AddText(ImVec2(p.x - size_up_text.x / 2.f + size_down_text.x / 2.f, p.y), color_upperTimerText, timer_str.c_str());
		Pop_BoldFont();
	ImGui::SetWindowFontScale(scale_bottom);
		drawlist->AddText(ImVec2(p.x,p.y + size_up_text.y), color_bottomTimerText, frame_str.c_str());
	ImGui::SetWindowFontScale(1.f);


	return ImVec2(p.x + size_up_text.x + 1.f,p.y);
}