#include "Timeline.Preview.h"


#include "ThirdParty/ImGui/imgui.h"
#include "ThirdParty/ImGui/imgui_internal.h"
#include "ThirdParty/IconFontCppHeaders/IconsFontAwesome6.h"

Timeline_Preview::Timeline_Preview(RendererAnimationScene* renderer) : renderer(renderer) {

}
Timeline_Preview::~Timeline_Preview() {

}

void Timeline_Preview::Draw() {
	ImGui::Begin(u8"Предпросмотр", nullptr, ImGuiWindowFlags_NoScrollWithMouse);

	ImGui::End();
}
void Timeline_Preview::Update() {

}