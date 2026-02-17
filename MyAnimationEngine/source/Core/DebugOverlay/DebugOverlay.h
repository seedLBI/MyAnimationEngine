#ifndef DEBUG_OVERLAY_H
#define DEBUG_OVERLAY_H

#include "ThirdParty/ImGui/imgui.h"
#include "ThirdParty/ImGui/imgui_impl_glfw.h"
#include "ThirdParty/ImGui/imgui_impl_opengl3.h"

#include <vector>
#include <string>

namespace DebugOverlay {

	namespace vars {
		extern std::vector<std::pair<std::string, ImColor>> elements;
	}

	void DrawDebug();
	void AddText(const std::string& text, const ImColor& color);
}



#endif