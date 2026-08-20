#ifndef TIMELINE_GRAPH_H
#define TIMELINE_GRAPH_H

#include "Core/Animation/Widgets/Panel/HorizontalTickRenderer/Horizontal.Tick.Renderer.h"

#include "Core/Animation/Widgets/Panel/Transform/Horizontal/Timeline.Transform.Horizontal.h"
#include "Core/Animation/Layer/ProviderData/Layers.Data.Provider.h"
#include "Core/Animation/Timeline/ProviderData/Timeline.ProviderData.h"
#include "Core/Animation/Timeline/Controller/Timeline.Controller.h"
#include "Core/Animation/Widgets/Panel/Selector/Timeline.Selector.h"

#include "Core/Animation/Widgets/Panel/TimeCursor/Timeline.TimeCursor.h"

#include "Core/Animation/Widgets/Panel/Transform/TransformScaled/Transform.Scaled.h"

#include "Utils/ImGui/Utils.ImGui.h"

class Timeline_Graph{
public:
	Timeline_Graph(	Timeline_TimeCursor* timeCursor,
					LayersDataProvider* layersData,
					TimelineProviderData* timelineData,
					TimelineController* timelineController,
					TimelineTransformHorizontal* horizontalTransform,
					TransformScaled* verticalTransform,
					HorizontalTickRenderer* horizontalTickRenderer,
					Timeline_Selector* selector);
	~Timeline_Graph();

	void Draw();
	void Update(const float& dt);

	void NeedCenterize();

private:
	Timeline_TimeCursor* timeCursor;
	LayersDataProvider* layersData;
	TimelineProviderData* timelineData;
	TimelineController* timelineController;
	TimelineTransformHorizontal* horizontalTransform;
	HorizontalTickRenderer* horizontalTickRenderer;
	Timeline_Selector* selector;
	TransformScaled* verticalTransform;

	bool isHovered = false, isDraging = false, isHeld = false;
	float mouse_global = 0.f, mouse_screen = 0.f;
	float scroll_zoom;


	float min_graph = 0.f, max_graph = 0.f;

	bool NeedCenterized = false;

	ImColor colorLight = ImColor(0.2f, 0.2f, 0.2f, 1.f);
	ImColor colorDark  = ImColor(0.15f, 0.15f, 0.15f, 1.f);

	void DrawTicks(ImVec2 p_min, ImVec2 p_max);
	void DrawGraphs(ImVec2 p_min, ImVec2 p_max);


};




#endif