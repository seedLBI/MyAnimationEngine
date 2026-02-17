#include "Timeline.Transform.h"

TimelineTransform::TimelineTransform(TimelineProviderData* providerData) 
	: providerData(providerData) 
{
	scale.SetAll(1.f);
	offset.SetAll(0.f);
}
TimelineTransform::~TimelineTransform() {

}

void TimelineTransform::updateAvailableWidth(const double& availableWidth) {
	this->width_available = availableWidth;
}

void TimelineTransform::update(const float& dt) {
	scale.Update(dt);
	offset.Update(dt);
}

float TimelineTransform::getPostionScreen_real(const float& globalPos) {
	return getOffset().GetReal() +  getLengthTick_real() * globalPos;
}
float TimelineTransform::getPostionGlobal_real(const float& screenPos) {
	return (screenPos - getOffset().GetReal()) / (getLengthTick_real());
}

float TimelineTransform::getPostionScreen_lerped(const float& globalPos) {
	return getOffset().GetLerped() + getLengthTick_lerped() * globalPos;
}
float TimelineTransform::getPostionGlobal_lerped(const float& screenPos) {
	return (screenPos - getOffset().GetLerped()) / (getLengthTick_lerped());
}

void TimelineTransform::zoomToPointByValue(const double& relative_point, const double& value, const bool& isScreen) {
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
void TimelineTransform::zoomRestore() {
	getScale().Set(1.f);
	getOffset().Set(0.f);
}

double TimelineTransform::getLengthTick_real() {
	return 200.0 * getScale().GetReal();
}

double TimelineTransform::getLengthTick_lerped() {
	return 200.0 * getScale().GetLerped();
}

void TimelineTransform::centerize(const double& relative_point, const bool& isScreen) {

	float screen_center = width_available / 2.f;
	float screen_point = isScreen == true? relative_point : getPostionScreen_real(relative_point);

	real_offset -= screen_point - screen_center;
	real_offsetDelta = 0.0;

	getOffset().Set(real_offset + real_offsetDelta);


}

void TimelineTransform::addZoom(const double& value) {
	real_scale += value;
	getScale().Set(real_scale);
}
void TimelineTransform::scaleZoomBy(const double& value) {
	zoomToPointByValue(width_available / 2.f, value);
}

void TimelineTransform::addOffset(const double& value) {
	real_offset += value;
	getOffset().Set(real_offset + real_offsetDelta);
}
void TimelineTransform::setOffsetDelta(const double& delta) {
	real_offsetDelta = delta;
	getOffset().Set(real_offset + real_offsetDelta);
}
void TimelineTransform::applyOffsetDelta() {
	real_offset += real_offsetDelta;
	real_offsetDelta = 0;

	getOffset().Set(real_offset + real_offsetDelta);
}

LerpedValue_float& TimelineTransform::getOffset() {
	return offset;
}
LerpedValue_float& TimelineTransform::getScale() {
	return scale;
}
