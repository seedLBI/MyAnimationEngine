#ifndef TIMELINE_SCROLL_HORIZONTAL_H
#define TIMELINE_SCROLL_HORIZONTAL_H

#include "Core/Animation/Widgets/Panel/Context/Timeline.Context.h"

class Timeline_Scroll_Horizontal {
public:
	Timeline_Scroll_Horizontal(TimelineContext* ctx);
	~Timeline_Scroll_Horizontal();

	void Draw();
	void Update();

private:
	TimelineContext* ctx;
};

#endif