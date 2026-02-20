#ifndef TIMELINE_LAYERS_LIST_TIMELINE_H
#define TIMELINE_LAYERS_LIST_TIMELINE_H

#include "Core/Animation/Timeline/Transform/Horizontal/Timeline.Transform.Horizontal.h"
#include "Core/Animation/Timeline/Transform/Vertical/Timeline.Transform.Vertical.h"
#include "Core/Animation/Layer/ProviderData/Layers.Data.Provider.h"
#include "Core/Animation/Timeline/ProviderData/Timeline.ProviderData.h"


class Timeline_Layers_Viewer
{
public:
	Timeline_Layers_Viewer(
		LayersDataProvider* layersData, 
		TimelineProviderData* timelineData, 
		TimelineTransformVertical* verticalTransform, 
		TimelineTransformHorizontal* horizontalTransform
	);

	~Timeline_Layers_Viewer();

	void Draw();
	void Update();

private:
	LayersDataProvider*		layersData;
	TimelineProviderData*	timelineData;

	TimelineTransformVertical*	 verticalTransform;
	TimelineTransformHorizontal* horizontalTransform;

};





#endif