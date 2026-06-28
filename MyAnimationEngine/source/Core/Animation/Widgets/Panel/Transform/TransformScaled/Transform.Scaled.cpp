#include "Transform.Scaled.h"

#include <algorithm>

TransformScaled::TransformScaled() : offset(0.f), scale(1.f) {

}

TransformScaled::~TransformScaled(){

}

void TransformScaled::update(float deltaTime) {
	offset.Update(deltaTime);
	scale.Update(deltaTime);
	reactionOverScale.Update(deltaTime);
}

void TransformScaled::updateAvailableScreenValue(float value) {
	this->availableScreenValue = value;
}

float TransformScaled::getAvailableScreenValue() {
	return this->availableScreenValue;
}

void TransformScaled::setBaseStep(float value) {
	this->base_step = value;
}

float TransformScaled::getMinOffset() {
	return bounds_offset.bmin;
}
float TransformScaled::getMaxOffset() {
	return bounds_offset.bmax;
}

void TransformScaled::setBoundsOffset(float omin, float omax) {
	bounds_offset.bmin = omin;
	bounds_offset.bmax = omax;
}
void TransformScaled::setBoundsScale(float smin, float smax) {
	bounds_scale.bmin = smin;
	bounds_scale.bmax = smax;
}

LerpedValue<float>& TransformScaled::getOffset() {
	return offset;
}
LerpedValue<float>& TransformScaled::getScale() {
	return scale;
}

float TransformScaled::getLengthTick_real() {
	return this->base_step * getScale().GetReal();
}
float TransformScaled::getLengthTick_lerped() {
	return this->base_step * getScale().GetLerped();
}


float TransformScaled::getPosGlobal_real(float screenPos) {
	float lengthTick = getLengthTick_real();
	if (lengthTick == 0.f) return 0.f;
	return (screenPos - getOffset().GetReal()) / lengthTick;
}
float TransformScaled::getPosGlobal_lerped(float screenPos) {
	float lengthTick = getLengthTick_lerped();
	if (lengthTick == 0.f) return 0.f;
	return (screenPos - getOffset().GetLerped()) / lengthTick;
}

float TransformScaled::getPosScreen_real(float globalPos) {
	return getOffset().GetReal() + getLengthTick_real() * globalPos;
}
float TransformScaled::getPosScreen_lerped(float globalPos) {
	return getOffset().GetLerped() + getLengthTick_lerped() * globalPos;
}


void TransformScaled::centerize(float relative_point, bool isScreen) {

	float screenCenter = this->availableScreenValue / 2.f;
	float screePoint = isScreen ? relative_point : getPosScreen_real(relative_point);

	real_offset -= screePoint - screenCenter;
	offsetDelta = 0.0;
	real_offset = clampOffset(real_offset);

	getOffset().Set(real_offset);

}

void TransformScaled::fitToRanges(float global_min, float global_max) {
	float screen_min = getPosScreen_real(global_min);
	float screen_max = getPosScreen_real(global_max);

	float length = std::abs(screen_min - screen_max);

	float scaleKoef = this->availableScreenValue / length;

	getScale().Set(getScale().GetReal() * scaleKoef);

	getOffset().Set(
		getOffset().GetReal() - getPosScreen_real(global_min)
	);
}

float TransformScaled::getMinBoundView_real() {
	return getPosGlobal_real(0.f);
}
float TransformScaled::getMinBoundView_lerped() {
	return getPosGlobal_lerped(0.f);
}

float TransformScaled::getMaxBoundView_real() {
	return getPosGlobal_real(this->availableScreenValue);
}
float TransformScaled::getMaxBoundView_lerped() {
	return getPosGlobal_lerped(this->availableScreenValue);
}

float TransformScaled::getReactionOverScale() {
	return reactionOverScale.GetLerped();
}


void TransformScaled::zoomToPointByValue(float relative_point, float value, bool isScreen) {
	float prev, next;

	if (isScreen)
		prev = getPosGlobal_real(relative_point);
	else
		prev = getPosScreen_real(relative_point);

	getScale().Set(getScale().GetReal() * value);
	clampScale();

	if (isScreen)
		next = getPosGlobal_real(relative_point);
	else
		next = getPosScreen_real(relative_point);

	float target_offset = real_offset;

	if (isScreen)
		target_offset += getPosScreen_real(prev - next);
	else
		target_offset += prev - next;

	setOffsetWithClamp(target_offset);
}
void TransformScaled::zoomRestore() {
	getScale().Set(1.f);
	setOffsetWithClamp(0.f);
}
void TransformScaled::scaleZoomBy(float value) {
	zoomToPointByValue(this->availableScreenValue / 2.f, value, true);
}
void TransformScaled::addZoom(float value) {
	getScale().Set(getScale().GetReal() + value);
	clampScale();
}

void TransformScaled::setOffsetWithClamp(float value) {
	real_offset = clampOffset(value);
	getOffset().Set(clampOffset(real_offset + offsetDelta));
}
void TransformScaled::addOffset(float value) {
	setOffsetWithClamp(real_offset + value);
}

void TransformScaled::setOffsetDelta(float delta) {
	offsetDelta = delta;
	getOffset().Set(clampOffset(real_offset + offsetDelta));
}
float TransformScaled::getOffsetDelta() {
	return offsetDelta;
}
void TransformScaled::applyOffsetDelta() {
	real_offset += offsetDelta;
	offsetDelta = 0.f;
	setOffsetWithClamp(real_offset);
}


void TransformScaled::clampScale() {
	float current_scale = getScale().GetReal();
	float clamped_scale = std::clamp(current_scale, bounds_scale.bmin, bounds_scale.bmax);

	if (clamped_scale != current_scale) {
		getScale().Set(clamped_scale);

		reactionOverScale.SetAll(1.f);
		reactionOverScale.Set(0.f);
	}
}

float TransformScaled::clampOffset(float target_offset) {
	return std::clamp(target_offset, bounds_offset.bmin, bounds_offset.bmax);
}