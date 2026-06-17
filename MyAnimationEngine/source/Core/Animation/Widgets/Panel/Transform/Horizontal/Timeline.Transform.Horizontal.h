#ifndef TIMELINE_TRANSFORM_HORIZONTAL_H
#define TIMELINE_TRANSFORM_HORIZONTAL_H

#include "Core/Animation/Timeline/ProviderData/Timeline.ProviderData.h"
#include "Utils/LerpedValue/LerpedValue.h"


class TimelineTransformHorizontal {
public:
	TimelineTransformHorizontal(TimelineProviderData* providerData);
	~TimelineTransformHorizontal();


	float getAvailableWidth();
	void updateAvailableWidth(const double& availableWidth);
	void update(const float& dt);


	float getPostionScreen_real(const float& globalPos);
	float getPostionGlobal_real(const float& screenPos);

	float getPostionScreen_lerped(const float& globalPos);
	float getPostionGlobal_lerped(const float& screenPos);

	void zoomToPointByValue(const double& relative_point, const double& value, const bool& isScreen = true);
	void zoomRestore();

	double getLengthTick_real();
	double getLengthTick_lerped();



	float getMinScale();
	float getMaxScale();

	float getMinOffset();
	float getMaxOffset();

	float getLeftBoundView_global();
	float getRightBoundView_global();


	void centerize(const double& relative_point, const bool& isScreen = false);

	void scaleZoomBy(const double& value);

	void addZoom(const double& value);
	void addOffset(const double& value);

	void setOffsetDelta(const float& delta);
	float getOffsetDelta();
	void applyOffsetDelta();




	float getOffset_OutOfBounds();


	void fitToRanges(const double& global_left, const double& global_right);


	LerpedValue_float& getOffset();
	LerpedValue_float& getScale();
	const float& getReactionToOverScale();

private:
	TimelineProviderData* providerData;

	LerpedValue_float scale, offset;
	LerpedValue_float reaction_to_over_scale;

	float real_scale = 1.f;
	float real_offset = 0.f;
	float real_offsetDelta = 0.f;

	float base_step = 100.f;


	float width_available = 0.f;



	float Out_Bound_zoom_percent = 0.45f;

	float clampOffsetByHalfWidth_screen(const float& screen_value);
	bool clampZoomByTimeline();
};



#endif