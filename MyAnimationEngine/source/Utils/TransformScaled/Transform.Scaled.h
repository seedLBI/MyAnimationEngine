#ifndef TRANSFORM_SCALED_H
#define TRANSFORM_SCALED_H

#include "Utils/LerpedValue/LerpedValue.h"


struct Bounds {
	float bmin, bmax;
};

class TransformScaled {
public:
	TransformScaled();
	~TransformScaled();

	void update(float deltaTime);
	void updateAvailableScreenValue(float value);
	float getAvailableScreenValue();

	void setBoundsOffset(float omin, float omax);
	void setBoundsScale(float smin, float smax);

	void setBaseStep(float value);

	float getMinOffset();
	float getMaxOffset();

	float getLengthTick_real();
	float getLengthTick_lerped();

	float getPosGlobal_real   (float screenPos);
	float getPosGlobal_lerped (float screenPos);

	float getPosScreen_real   (float globalPos);
	float getPosScreen_lerped (float globalPos);

	float getMinBoundView_real();
	float getMinBoundView_lerped();

	float getMaxBoundView_real();
	float getMaxBoundView_lerped();

	void centerize(float relative_point, bool isScreen = false);
	void centerizeForce(float relative_point, bool isScreen = false);
	void fitToRanges(float global_min, float global_max);

	void zoomToPointByValue(float relative_point, float value, bool isScreen = true);
	void zoomRestore();
	void scaleZoomBy(float value);
	void addZoom(float value);

	float getReactionOverScale();

	void setOffsetWithClamp(float value);
	void addOffset(float value);

	void setOffsetDelta(float delta);
	float getOffsetDelta();
	void applyOffsetDelta();

	LerpedValue<float>& getOffset();
	LerpedValue<float>& getScale();

private:
	LerpedValue<float> offset, scale;
	LerpedValue<float> reactionOverScale;

	float real_offset = 0.f;
	float offsetDelta = 0.f;


	float availableScreenValue;

	float base_step = 100.f;

	Bounds bounds_offset = { -1000000.f, 1000000.f };
	Bounds bounds_scale = { 0.001f, 1000.f };

	void clampScale();
	float clampOffset(float target_offset);
};






#endif