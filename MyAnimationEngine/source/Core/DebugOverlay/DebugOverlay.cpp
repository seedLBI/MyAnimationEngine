#include "DebugOverlay.h"

namespace DebugOverlay {

	namespace vars {
		std::vector<std::pair<std::string, ImColor>> elements;
	}

	void DrawDebug() {
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImVec2 pos_begin = { viewport->Pos.x + 40.f,  viewport->Pos.y + 40.f };

        ImDrawList* draw_list = ImGui::GetForegroundDrawList();

        for (const auto& text : vars::elements) {

			ImVec2 sizeText = ImGui::CalcTextSize(text.first.c_str());

			ImColor color_foreground = text.second;
			ImColor color_background = ImColor(0.f,0.f,0.f,1.f );

			float padding_x = 6.f;

			ImVec2 pmin = { pos_begin.x - padding_x,  pos_begin.y };
			ImVec2 pmax = { pos_begin.x + sizeText.x + padding_x, pos_begin.y + sizeText.y };


			pmin.x -= 2.f;
			pmin.y -= 2.f;

			pmax.x += 2.f;
			pmax.y += 2.f;

			draw_list->AddRectFilled(pmin, pmax, color_foreground, 10.f);

			pmin.x += 2.f;
			pmin.y += 2.f;

			pmax.x -= 2.f;
			pmax.y -= 2.f;


			draw_list->AddRectFilled(pmin, pmax, color_background, 10.f);

            draw_list->AddText(pos_begin, text.second, text.first.c_str());
            pos_begin.y += ImGui::GetTextLineHeight() + 6.f;
        }

		vars::elements.clear();
	}

	void AddText(const std::string& text, const ImColor& color) {
        if (vars::elements.size() > 1024)
            return;

        vars::elements.push_back({ text,color });
	}
}