#ifndef TIMELINE_SCROLL_HORIZONTAL_H
#define TIMELINE_SCROLL_HORIZONTAL_H


#include "Core/Animation/Timeline/Transform/Horizontal/Timeline.Transform.Horizontal.h"
#include "Core/Animation/Timeline/ProviderData/Timeline.ProviderData.h"

class Timeline_Scroll_Horizontal
{
public:
	Timeline_Scroll_Horizontal(
		TimelineProviderData* timelineData,
		TimelineTransformHorizontal* horizontalTransform);

	~Timeline_Scroll_Horizontal();

	void Draw();
	void Update();

private:
	TimelineProviderData* timelineData;
	TimelineTransformHorizontal* horizontalTransform;
};




#endif