#ifndef TIMELINE_RANGE_H
#define TIMELINE_RANGE_H

#include "Core/Animation/Timeline/Transform/Horizontal/Timeline.Transform.Horizontal.h"
#include "Core/Animation/Timeline/ProviderData/Timeline.ProviderData.h"

class Timeline_Range {
public:
	Timeline_Range(
		TimelineProviderData* timelineData, 
		TimelineTransformHorizontal* horizontalTransform
	);
	~Timeline_Range();

	void Draw();
	void Update();

private:
	TimelineProviderData* timelineData;
	TimelineTransformHorizontal* horizontalTransform;
};



#endif