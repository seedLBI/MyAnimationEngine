#ifndef TIMELINE_TRANSFORM_VERTICAL_H
#define TIMELINE_TRANSFORM_VERTICAL_H


#include "ThirdParty/ImGui/imgui.h"
#include "ThirdParty/ImGui/imgui_internal.h"


class TimelineTransformVertical {
public:

	TimelineTransformVertical();
	~TimelineTransformVertical();

	void update(const float& deltaTime);

	float getScaledHeightOneObject();

private:

};




#endif