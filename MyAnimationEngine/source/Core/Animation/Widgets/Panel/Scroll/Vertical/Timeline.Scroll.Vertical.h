#ifndef TIMELINE_SCROLL_VERTICAL_H
#define TIMELINE_SCROLL_VERTICAL_H

#include "Core/Animation/Widgets/Panel/Context/Timeline.Context.h"

class Timeline_Scroll_Vertical {
public:
	Timeline_Scroll_Vertical(TimelineContext* ctx);
	~Timeline_Scroll_Vertical();

	void Draw();
	void Update();

private:
	TimelineContext* ctx;
};

#endif