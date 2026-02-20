#ifndef TIMELINE_SCROLL_VERTICAL_H
#define TIMELINE_SCROLL_VERTICAL_H


#include "Core/Animation/Timeline/Transform/Vertical/Timeline.Transform.Vertical.h"
#include "Core/Animation/Layer/ProviderData/Layers.Data.Provider.h"

class Timeline_Scroll_Vertical {
public:
	Timeline_Scroll_Vertical(
		LayersDataProvider* layersData, 
		TimelineTransformVertical * verticalTransform
	);
	~Timeline_Scroll_Vertical();

	void Draw();
	void Update();

private:
	LayersDataProvider* layersData;

	TimelineTransformVertical* verticalTransform;
};





#endif