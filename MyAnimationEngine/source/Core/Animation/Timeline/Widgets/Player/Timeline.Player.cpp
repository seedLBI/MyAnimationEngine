#include "Timeline.Player.h"

#include "ThirdParty/ImGui/imgui.h"
#include "ThirdParty/ImGui/imgui_internal.h"

#include "ThirdParty/IconFontCppHeaders/IconsFontAwesome6.h"



Timeline_Player::Timeline_Player(TimelineController* controller) : controller(controller)
{

}

Timeline_Player::~Timeline_Player()
{

}

void Timeline_Player::Draw() {

	ImGui::Begin(u8"Проигрыватель композиций");

	if (ImGui::Button(ICON_FA_BACKWARD_FAST)) {
		controller->FirstFrame();
	}
	ImGui::SameLine();

	if (ImGui::Button(ICON_FA_BACKWARD)) {
		controller->PrevFrame();
	}
	ImGui::SameLine();

	if (ImGui::Button(ICON_FA_PLAY)) {
		controller->Play();
	}
	ImGui::SameLine();

	if (ImGui::Button(ICON_FA_FORWARD)) {
		controller->NextFrame();
	}
	ImGui::SameLine();

	if (ImGui::Button(ICON_FA_FORWARD_FAST)) {
		controller->LastFrame();
	}

	ImGui::End();

}

void Timeline_Player::Update()
{

}


