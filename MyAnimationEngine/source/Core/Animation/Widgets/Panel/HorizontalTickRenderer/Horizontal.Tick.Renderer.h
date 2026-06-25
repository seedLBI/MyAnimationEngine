#ifndef HORIZONTAL_TICK_RENDERER_H
#define HORIZONTAL_TICK_RENDERER_H

#include "Core/Animation/Widgets/Panel/Transform/Horizontal/Timeline.Transform.Horizontal.h"
#include "Core/Animation/Timeline/ProviderData/Timeline.ProviderData.h"

#include "ThirdParty/ImGui/imgui.h"
#include "ThirdParty/ImGui/imgui_internal.h"

#include <algorithm>

class HorizontalTickRenderer {
public:
	HorizontalTickRenderer(
		TimelineTransformHorizontal* timelineTransformHorizontal,
		TimelineProviderData* timelineData);
	~HorizontalTickRenderer();

	void DrawTicks(
		const ImVec2& p_min, const ImVec2& p_max,
		ImColor colorMain, ImColor colorSub,
		float widthMain, float widthSub,
		float heightMain, float heightSubMinor, float heightSubMajor,
		bool drawText
	);

private:
	TimelineTransformHorizontal* timelineTransformHorizontal;
	TimelineProviderData* timelineData;
};





#endif