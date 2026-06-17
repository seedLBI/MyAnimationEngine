#ifndef TIMELINE_TRANSFORM_VERTICAL_H
#define TIMELINE_TRANSFORM_VERTICAL_H


#include "ThirdParty/ImGui/imgui.h"
#include "ThirdParty/ImGui/imgui_internal.h"

#include "Utils/LerpedValue/LerpedValue.h"

class TimelineTransformVertical {
public:

	TimelineTransformVertical();
	~TimelineTransformVertical();

	void updateAvailableHeight(const double& height_available);
	void update(const float& deltaTime);

	float getScaledHeightOneObject();
	float getOffset_lerped();

	void addOffset(const float& value);

private:
	LerpedValue_float offset;
	float height_available = 0.f;


};




#endif