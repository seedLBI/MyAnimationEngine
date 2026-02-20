#ifndef TIMELINE_RULER_H
#define TIMELINE_RULER_H

#include "Core/Animation/Timeline/ProviderData/Timeline.ProviderData.h"
#include "Core/Animation/Timeline/Transform/Horizontal/Timeline.Transform.Horizontal.h"
#include "Utils/Timer/Timer.Framerate.h"


#include "ThirdParty/ImGui/imgui.h"
#include "ThirdParty/ImGui/imgui_internal.h"

class Timeline_Ruler {
public:
	Timeline_Ruler(TimelineProviderData* timelineData, TimelineTransformHorizontal* timelineTransformHorizontal);
	~Timeline_Ruler();

	void Draw();
	void Update();

private:

	TimelineProviderData* timelineData = nullptr;
	TimelineTransformHorizontal* timelineTransformHorizontal = nullptr;
	
	float mouse_global;
	float mouse_screen;

	ImVec2 cursor_pos{ 0,0 }, region_max{0,0};

	bool isHovered = false;
	bool isDraging = false;
	float scrol_zoom = 0.f;

	void UpdateVars();
	void UpdateZoom();


};



#endif