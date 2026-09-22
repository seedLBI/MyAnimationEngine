#ifndef TIMELINE_GRAPH_H
#define TIMELINE_GRAPH_H


#include "Core/Animation/Widgets/Panel/Context/Timeline.Context.h"
#include "Core/Animation/Widgets/Panel/HorizontalTickRenderer/Horizontal.Tick.Renderer.h"
#include "Core/Animation/Widgets/Panel/Layers/KeyframeDragHandler/Keyframe.DragHandler.h"
#include "Core/Animation/Widgets/Panel/TimeCursor/Timeline.TimeCursor.h"
#include "Core/Animation/Widgets/Colors/Widgets.Colors.h"

#include "ButtonsDrawer/Graph.Buttons.Drawer.h"

#include "Utils/ImGui/Utils.ImGui.h"

class Timeline_Graph{
public:
	Timeline_Graph(
		TimelineContext* ctx,
		KeyframeDragHandler* kf_DragHandler,
		Timeline_TimeCursor* timeCursor, 
		HorizontalTickRenderer* horizontalTickRenderer);
	~Timeline_Graph();

	void Draw();
	void Update(const float& dt);

	void NeedCenterize();

private:
	TimelineContext* ctx;
	KeyframeDragHandler* kf_DragHandler;
	Timeline_TimeCursor* timeCursor;
	HorizontalTickRenderer* horizontalTickRenderer;

	GraphButtonsDrawer* graphButtonsDrawer;

	ImRect getRectGraph();


	bool isHovered = false, isDraging = false, isHeld = false;
	float mouse_global_y = 0.f, mouse_screen_y = 0.f;
	float mouse_global_x = 0.f, mouse_screen_x = 0.f;

	float scroll_zoom;
	float size_keyframe = 8.f;
	float size_offset = size_keyframe / 2.f;

	float min_graph = 0.f, max_graph = 0.f;

	bool NeedCenterized = false;

	ImColor colorLight = ImColor(0.2f, 0.2f, 0.2f, 1.f);
	ImColor colorDark  = ImColor(0.15f, 0.15f, 0.15f, 1.f);


	void DrawSpeedGraph(ImVec2 p_min, ImVec2 p_max);


	void DrawTicks(ImVec2 p_min, ImVec2 p_max);
	void DrawGraphs(ImVec2 p_min, ImVec2 p_max);

	float height_menu_buttons = 40.f;

};




#endif