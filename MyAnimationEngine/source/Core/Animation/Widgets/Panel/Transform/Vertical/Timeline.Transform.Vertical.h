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
	void updateContentHeight(const double& height_content);
	void update(const float& deltaTime);

	const float& getAvailableHeight();
	const float& getContentHeight();

	float getScaledHeightOneObject();

	float getOffset_lerped();
	void addOffset(const float& value);

private:
	LerpedValue<float> offset;
	float height_available = 0.f;
	float height_content = 0.f;

	void updateBounds();

};




#endif