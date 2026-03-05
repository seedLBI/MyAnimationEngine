#include "DebugOverlay.h"

namespace DebugOverlay {

	struct LineTexts {
		std::vector < std::pair<std::string, ImColor> > texts;
	};

	namespace vars {
		std::vector<LineTexts> elements;
		LerpedValue_float timer_show = LerpedValue_float(1.f);
		bool isShow = true;
	}

	void Show() {
		vars::isShow = true;
		vars::timer_show.Set(1.f);
	}
	void Hide() {
		vars::isShow = false;
		vars::timer_show.Set(0.f);
	}

	void ToggleShow() {
		if (vars::isShow)
			Hide();
		else
			Show();
	}

	void Update(const float& dt) {
		vars::timer_show.Update(dt*10.f);

		if (vars::isShow) {
			vars::timer_show.Set(1.f);
		}

	}

	void DrawDebug() {
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImVec2 pos_begin = { viewport->Pos.x + 40.f,  viewport->Pos.y + 40.f };

        ImDrawList* draw_list = ImGui::GetForegroundDrawList();



		if (vars::timer_show.GetLerped() < 0.01f)
			return;

		float AddPos_x = -300.f + vars::timer_show.GetLerped() * (40.f - (-300.f));



        for (const auto& line : vars::elements) {

			pos_begin.x = viewport->Pos.x + AddPos_x;

			for (const auto& text : line.texts)
			{
				ImVec2 sizeText = ImGui::CalcTextSize(text.first.c_str());

				ImColor color_foreground = text.second;
				ImColor color_background = ImColor(0.f, 0.f, 0.f, 0.8f * vars::timer_show.GetLerped());


				color_foreground.Value.w *= vars::timer_show.GetLerped();


				float padding_x = 6.f;

				ImVec2 pmin = { pos_begin.x - padding_x,  pos_begin.y };
				ImVec2 pmax = { pos_begin.x + sizeText.x + padding_x, pos_begin.y + sizeText.y };


				pmin.x -= 2.f;
				pmin.y -= 2.f;

				pmax.x += 2.f;
				pmax.y += 2.f;

				draw_list->AddRect(pmin, pmax, color_foreground, 10.f,0,2.f);

				pmin.x += 2.f;
				pmin.y += 2.f;

				pmax.x -= 2.f;
				pmax.y -= 2.f;


				draw_list->AddRectFilled(pmin, pmax, color_background, 10.f);

				draw_list->AddText(pos_begin, color_foreground, text.first.c_str());

				pos_begin.x += sizeText.x + 3.f * padding_x + 3.f;

			}

			pos_begin.y += ImGui::GetTextLineHeight() + 6.f;
        }

		vars::elements.clear();
	}

	void AddText(const std::string& text, const ImColor& color) {

		vars::elements.push_back(
			DebugOverlay::LineTexts{ 
				{
					{text,color}
				}
			}
		);

	}

	void AddText(const std::vector<std::string>& texts, const ImColor& color) {

		LineTexts result;
		result.texts.reserve(texts.size());
		
		for (const auto& text : texts) {
			result.texts.push_back({ text, color });
		}

		vars::elements.push_back( result );

	}
	void AddText(const std::vector<std::pair<std::string, ImColor>>& colored_texts) {

		vars::elements.push_back(
			DebugOverlay::LineTexts{
				colored_texts
			}
		);

	}

}