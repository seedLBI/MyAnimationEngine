#include "Timeline.Preview.h"

#include "ThirdParty/ImGui/imgui.h"
#include "ThirdParty/ImGui/imgui_internal.h"
#include "ThirdParty/IconFontCppHeaders/IconsFontAwesome6.h"

#include "Core/Animation/Widgets/Colors/Widgets.Colors.h"

Timeline_Preview::Timeline_Preview(RendererAnimationScene* renderer) : renderer(renderer) {

}
Timeline_Preview::~Timeline_Preview() {

}

void Timeline_Preview::Draw() {

	ImGui::PushStyleColor(ImGuiCol_WindowBg, COLOR_BG_LIST.Value);
	ImGui::PushStyleColor(ImGuiCol_Separator, COLOR_SEPARATOR.Value);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

	ImGui::Begin(u8"Предпросмотр", nullptr, ImGuiWindowFlags_NoScrollWithMouse);

	auto drawlist = ImGui::GetWindowDrawList();
	ImVec2 cursor_pos = ImGui::GetCursorScreenPos();
	ImVec2 region_max = ImGui::GetContentRegionAvail();

	ImVec2 p_min = cursor_pos;
	ImVec2 p_max = { cursor_pos.x + region_max.x, cursor_pos.y + region_max.y };



	DrawBorderForFocusedWindow(COLOR_WINDOW_BORDER_FOCUS);

	ImGui::End();

	ImGui::PopStyleVar(2);
	ImGui::PopStyleColor(2);


}
void Timeline_Preview::Update() {

}