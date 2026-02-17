#include "Timeline.Transform.Horizontal.h"

TimelineTransformHorizontal::TimelineTransformHorizontal(TimelineProviderData* providerData) 
	: providerData(providerData) 
{
	scale.SetAll(1.f);
	offset.SetAll(0.f);
}

TimelineTransformHorizontal::~TimelineTransformHorizontal() {

}

void TimelineTransformHorizontal::updateAvailableWidth(const double& availableWidth) {
	this->width_available = availableWidth;
}

void TimelineTransformHorizontal::update(const float& dt) {
	scale.Update(dt * 20.f);
	offset.Update(dt * 20.f);
}

float TimelineTransformHorizontal::getPostionScreen_real(const float& globalPos) {
	return getOffset().GetReal() +  getLengthTick_real() * globalPos;
}
float TimelineTransformHorizontal::getPostionGlobal_real(const float& screenPos) {
	return (screenPos - getOffset().GetReal()) / (getLengthTick_real());
}

float TimelineTransformHorizontal::getPostionScreen_lerped(const float& globalPos) {
	return getOffset().GetLerped() + getLengthTick_lerped() * globalPos;
}
float TimelineTransformHorizontal::getPostionGlobal_lerped(const float& screenPos) {
	return (screenPos - getOffset().GetLerped()) / (getLengthTick_lerped());
}

void TimelineTransformHorizontal::zoomToPointByValue(const double& relative_point, const double& value, const bool& isScreen) {
	float prev, next;


	if (isScreen)
		prev = getPostionGlobal_real(relative_point);
	else
		prev = getPostionScreen_real(relative_point);

	real_scale *= value;
	getScale().Set(real_scale);

	if (isScreen)
		next = getPostionGlobal_real(relative_point);
	else
		next = getPostionScreen_real(relative_point);

	if (isScreen)
		real_offset += getPostionScreen_real(prev - next);
	else
		real_offset += prev - next;

	getOffset().Set(real_offset + real_offsetDelta);

}
void TimelineTransformHorizontal::zoomRestore() {
	getScale().Set(1.f);
	getOffset().Set(0.f);
}

double TimelineTransformHorizontal::getLengthTick_real() {
	return 200.0 * getScale().GetReal();
}

double TimelineTransformHorizontal::getLengthTick_lerped() {
	return 200.0 * getScale().GetLerped();
}

void TimelineTransformHorizontal::centerize(const double& relative_point, const bool& isScreen) {

	float screen_center = width_available / 2.f;
	float screen_point = isScreen == true? relative_point : getPostionScreen_real(relative_point);

	real_offset -= screen_point - screen_center;
	real_offsetDelta = 0.0;

	getOffset().Set(real_offset + real_offsetDelta);


}

void TimelineTransformHorizontal::addZoom(const double& value) {
	real_scale += value;
	getScale().Set(real_scale);
}
void TimelineTransformHorizontal::scaleZoomBy(const double& value) {
	zoomToPointByValue(width_available / 2.f, value);
}

void TimelineTransformHorizontal::addOffset(const double& value) {
	real_offset += value;
	getOffset().Set(real_offset + real_offsetDelta);
}
void TimelineTransformHorizontal::setOffsetDelta(const double& delta) {
	real_offsetDelta = delta;
	getOffset().Set(real_offset + real_offsetDelta);
}
void TimelineTransformHorizontal::applyOffsetDelta() {
	real_offset += real_offsetDelta;
	real_offsetDelta = 0;

	getOffset().Set(real_offset + real_offsetDelta);
}

LerpedValue_float& TimelineTransformHorizontal::getOffset() {
	return offset;
}
LerpedValue_float& TimelineTransformHorizontal::getScale() {
	return scale;
}
