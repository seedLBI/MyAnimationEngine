#include "DebugOverlay.h"

namespace DebugOverlay {

	namespace vars {
		std::vector<std::pair<std::string, ImColor>> elements;
	}

	void DrawDebug() {
        ImGuiViewport* viewport = ImGui::GetMainViewport();

        ImGui::SetNextWindowPos({ viewport->Pos.x,  viewport->Pos.y + 60.f });
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);

        ImGuiWindowFlags flags =
            ImGuiWindowFlags_NoDecoration |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoSavedSettings |
            ImGuiWindowFlags_NoBringToFrontOnFocus |
            ImGuiWindowFlags_NoFocusOnAppearing |
            ImGuiWindowFlags_NoInputs |
            ImGuiWindowFlags_NoNav;

        ImGui::Begin("DebugOverlay", nullptr, flags);


        for (const auto& text : vars::elements) {
            ImGui::TextColored(text.second, text.first.c_str());
        }

        ImGui::End();


		vars::elements.clear();
	}

	void AddText(const std::string& text, const ImColor& color) {
        if (vars::elements.size() > 1024)
            return;

        vars::elements.push_back({ text,color });
	}
}