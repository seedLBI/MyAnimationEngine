#ifndef TIMELINE_RULER_H
#define TIMELINE_RULER_H

#include "Core/Animation/Timeline/ProviderData/Timeline.ProviderData.h"
#include "Core/Animation/Timeline/Transform/Horizontal/Timeline.Transform.Horizontal.h"
#include "Core/Animation/Timeline/Controller/Timeline.Controller.h"
#include "Utils/Timer/Timer.Framerate.h"

#include "Core/Animation/Timeline/Widgets/Panel/TimeCursor/Timeline.TimeCursor.h"


#include "ThirdParty/ImGui/imgui.h"
#include "ThirdParty/ImGui/imgui_internal.h"

class Timeline_Ruler {
public:
	Timeline_Ruler(Timeline_TimeCursor* timeCursor, TimelineController* timelineController, TimelineProviderData* timelineData, TimelineTransformHorizontal* timelineTransformHorizontal);
	~Timeline_Ruler();

	void Draw();
	void Update(const float& dt);

	const float& getGlobalMouse();
	const float& getScreenMouse();



private:
	Timeline_TimeCursor* timeCursor = nullptr;

	TimelineController* timelineController = nullptr;
	TimelineProviderData* timelineData = nullptr;
	TimelineTransformHorizontal* timelineTransformHorizontal = nullptr;
	
	float mouse_global;
	float mouse_screen;

	ImVec2 cursor_pos{ 0,0 }, region_max{0,0};

	bool isHovered = false;
	bool isDraging = false;
	float scrol_zoom = 0.f;

	void UpdateVars(const float& dt);
	void UpdateZoom();


	void FitAllTimeline();


	bool OutOfView(const ImVec2& pos);

	bool OutOfView(const ImVec2& pos_min, const ImVec2& pos_max);


	const float width_gradient_bound = 20.f;


	LerpedValue_float time_left_bound;
	LerpedValue_float time_right_bound;

	ImColor color_background = { 0.f,0.f,0.f,1.f };
	ImColor color_bounds = { 0.2f,0.7f,1.f,1.f };
	ImColor color_bounds_offsetMax = { 1.f,0.2f,0.4f,1.f };
	ImColor color_mainTick = { 1.f,1.f,1.f,1.f };
	ImColor color_subTick = { 1.f,0.8f,0.8f,0.4f };
	ImColor color_reaction_overScale = { 1.f,0.4f,0.1f,1.f };
};



#endif