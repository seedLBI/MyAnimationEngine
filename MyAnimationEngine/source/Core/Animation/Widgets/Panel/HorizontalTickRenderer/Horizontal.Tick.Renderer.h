#ifndef HORIZONTAL_TICK_RENDERER_H
#define HORIZONTAL_TICK_RENDERER_H

#include "Core/Animation/Widgets/Panel/Context/Timeline.Context.h"

#include "ThirdParty/ImGui/imgui.h"
#include "ThirdParty/ImGui/imgui_internal.h"

#include <algorithm>

class HorizontalTickRenderer {
public:
	HorizontalTickRenderer(TimelineContext* ctx);
	~HorizontalTickRenderer();

	void DrawTicks(
		const ImVec2& p_min, const ImVec2& p_max,
		ImColor colorMain, ImColor colorSub,
		float widthMain, float widthSub,
		float heightMain, float heightSubMinor, float heightSubMajor,
		bool drawText, bool drawAllSide_SubTicks = false
	);

private:
	TimelineContext* ctx;
};





#endif