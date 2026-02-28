#ifndef DEBUG_OVERLAY_H
#define DEBUG_OVERLAY_H

#include "ThirdParty/ImGui/imgui.h"
#include "ThirdParty/ImGui/imgui_impl_glfw.h"
#include "ThirdParty/ImGui/imgui_impl_opengl3.h"

#include <vector>
#include <string>

namespace DebugOverlay {

	extern struct LineTexts;

	namespace vars {
		extern std::vector<LineTexts> elements;
	}

	void DrawDebug();
	void AddText(const std::string& text, const ImColor& color);
	void AddText(const std::vector<std::string>& texts, const ImColor& color);
	void AddText(const std::vector<std::pair<std::string, ImColor>>& colored_texts);
}



#endif