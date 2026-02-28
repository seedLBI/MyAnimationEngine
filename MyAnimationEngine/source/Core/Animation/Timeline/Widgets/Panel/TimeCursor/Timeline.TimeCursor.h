#ifndef TIMELINE_TIME_CURSOR_H
#define TIMELINE_TIME_CURSOR_H

#include "Core/Animation/Timeline/Transform/Horizontal/Timeline.Transform.Horizontal.h"
#include "Core/Animation/Timeline/ProviderData/Timeline.ProviderData.h"

class Timeline_TimeCursor
{
public:
	Timeline_TimeCursor(
		TimelineProviderData* timelineData,
		TimelineTransformHorizontal* horizontalTransform
	);
	~Timeline_TimeCursor();

	void Draw();
	void Update();

private:
	TimelineProviderData* timelineData;
	TimelineTransformHorizontal* horizontalTransform;

};





#endif