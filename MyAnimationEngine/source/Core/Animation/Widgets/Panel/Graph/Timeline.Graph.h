#ifndef TIMELINE_GRAPH_H
#define TIMELINE_GRAPH_H

#include "Core/Animation/Widgets/Panel/HorizontalTickRenderer/Horizontal.Tick.Renderer.h"

#include "Core/Animation/Widgets/Panel/Transform/Horizontal/Timeline.Transform.Horizontal.h"
#include "Core/Animation/Layer/ProviderData/Layers.Data.Provider.h"
#include "Core/Animation/Timeline/ProviderData/Timeline.ProviderData.h"
#include "Core/Animation/Timeline/Controller/Timeline.Controller.h"
#include "VerticalTransform/Timeline.Graph.Transform.Vertical.h"

class Timeline_Graph{
public:
	Timeline_Graph(	LayersDataProvider* layersData,
					TimelineProviderData* timelineData,
					TimelineController* timelineController,
					TimelineTransformHorizontal* horizontalTransform,
					HorizontalTickRenderer* horizontalTickRenderer);
	~Timeline_Graph();

	void Draw();
	void Update();

private:
	LayersDataProvider* layersData;
	TimelineProviderData* timelineData;
	TimelineController* timelineController;
	TimelineTransformHorizontal* horizontalTransform;
	HorizontalTickRenderer* horizontalTickRenderer;
	Graph_VerticalTransform* verticalTransform;
};




#endif