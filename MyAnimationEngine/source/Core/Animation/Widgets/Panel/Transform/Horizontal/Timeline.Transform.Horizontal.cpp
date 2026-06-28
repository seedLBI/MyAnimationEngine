#include "Timeline.Transform.Horizontal.h"
#include "Core/DebugOverlay/DebugOverlay.h"
#include <algorithm>

TimelineTransformHorizontal::TimelineTransformHorizontal(TimelineProviderData* providerData) 
	: providerData(providerData) 
{
	scale.SetAll(1.f);
	offset.SetAll(0.f);
	reaction_to_over_scale.SetAll(0.f);
}

TimelineTransformHorizontal::~TimelineTransformHorizontal() {

}

void TimelineTransformHorizontal::updateAvailableWidth(const double& availableWidth) {
	this->width_available = availableWidth;
}

void TimelineTransformHorizontal::update(const float& dt) {
	if (FirstCenterize){
		if (width_available > 100.f) {
			fitToRanges(0. - 0.1, providerData->GetData().totalTime + 0.1);
			getOffset().SetAll(getOffset().GetReal());
			getScale().SetAll(getScale().GetReal());
			FirstCenterize = false;
		}
	}


	clampZoomByTimeline();

	scale.Update(dt * 22.f);
	offset.Update(dt * 22.f);
	reaction_to_over_scale.Update(dt * 8.f);


}



float TimelineTransformHorizontal::getAvailableWidth() {
	return width_available;
}

const float& TimelineTransformHorizontal::getReactionToOverScale() {
	return reaction_to_over_scale.GetLerped();
}

float TimelineTransformHorizontal::getMinScale() {
	const float& screenWidth = width_available;
	const float& totalTime = providerData->GetData().totalTime;

	return (screenWidth * Out_Bound_zoom_percent) / (totalTime * base_step);
}

float TimelineTransformHorizontal::getMaxScale() {
	const float& screenWidth = width_available;
	const float& framerate = (float)providerData->GetData().framerate;

	return (screenWidth * 0.7f * framerate) / base_step;
}


float TimelineTransformHorizontal::getMinOffset() {
	const float left_screen_bound_in_global = getPostionGlobal_lerped(0.f);
	const float right_screen_bound_in_global = getPostionGlobal_lerped(this->width_available / 2.f);
	const float length_for_centered_bound = right_screen_bound_in_global - left_screen_bound_in_global;
	const float left_timeline_bound = 0.f;

	return (length_for_centered_bound -left_timeline_bound ) * getLengthTick_lerped();
}
float TimelineTransformHorizontal::getMaxOffset() {
	const float left_screen_bound_in_global = getPostionGlobal_lerped(0.f);
	const float right_screen_bound_in_global = getPostionGlobal_lerped(this->width_available / 2.f);
	const float length_for_centered_bound = right_screen_bound_in_global - left_screen_bound_in_global;
	const float& right_timeline_bound = providerData->GetData().totalTime;

	return (length_for_centered_bound - right_timeline_bound ) * getLengthTick_lerped();
}


bool TimelineTransformHorizontal::clampZoomByTimeline() {

	const float& totalTime = providerData->GetData().totalTime;
	const float& framerate = (float)providerData->GetData().framerate;

	if (totalTime <= 0.f || framerate <= 0.f || base_step <= 0.f) return false;

	float min_scale = getMinScale();
	float max_scale = getMaxScale();

	if (min_scale > max_scale)
		min_scale = max_scale;

	float current_scale = getScale().GetReal();
	float clamped_scale = std::clamp(current_scale, min_scale, max_scale);

	if (clamped_scale != current_scale) {
		getScale().Set(clamped_scale);
		real_scale = getScale().GetReal();
		return true;
	}

	return false;
}

float TimelineTransformHorizontal::clampOffsetByHalfWidth_screen(const float& screen_value) {

	const float left_screen_bound_in_global  = getPostionGlobal_real(0.f);
	const float right_screen_bound_in_global = getPostionGlobal_real(this->width_available / 2.f);
	const float length_for_centered_bound    = right_screen_bound_in_global - left_screen_bound_in_global;
	
	const float left_timeline_bound   = 0.f;
	const float& right_timeline_bound = providerData->GetData().totalTime;

	const float min_left_pos_transform_screen  = (left_timeline_bound  - length_for_centered_bound) * getLengthTick_real();
	const float max_right_pos_transform_screen =  (right_timeline_bound - length_for_centered_bound) * getLengthTick_real();

	const float offset = -screen_value;

	if (offset < min_left_pos_transform_screen)
		return -min_left_pos_transform_screen;
	else if (offset > max_right_pos_transform_screen)
		return -max_right_pos_transform_screen;
	else
		return screen_value;
}

float TimelineTransformHorizontal::getOffset_OutOfBounds() {
	const float left_screen_bound_in_global = getPostionGlobal_real(0.f);
	const float right_screen_bound_in_global = getPostionGlobal_real(this->width_available / 2.f);
	const float length_for_centered_bound = right_screen_bound_in_global - left_screen_bound_in_global;

	const float left_timeline_bound = 0.f;
	const float& right_timeline_bound = providerData->GetData().totalTime;

	const float min_left_pos_transform_screen  = (left_timeline_bound - length_for_centered_bound) * getLengthTick_real();
	const float max_right_pos_transform_screen = (right_timeline_bound - length_for_centered_bound) * getLengthTick_real();

	float offset = real_offset + real_offsetDelta;


	if (-offset < min_left_pos_transform_screen)
		return offset + min_left_pos_transform_screen;
	else if (-offset > max_right_pos_transform_screen)
		return offset + max_right_pos_transform_screen;
	else
		return 0.f;
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
	if (clampZoomByTimeline()) {
		reaction_to_over_scale.SetAll(1.f);
		reaction_to_over_scale.Set(0.f);
	}

	if (isScreen)
		next = getPostionGlobal_real(relative_point);
	else
		next = getPostionScreen_real(relative_point);

	if (isScreen)
		real_offset += getPostionScreen_real(prev - next);
	else
		real_offset += prev - next;

	real_offset = clampOffsetByHalfWidth_screen(real_offset);

	getOffset().Set(
		(real_offset + real_offsetDelta) 
	);

}
void TimelineTransformHorizontal::zoomRestore() {
	getScale().Set(1.f);
	getOffset().Set(0.f);
}

double TimelineTransformHorizontal::getLengthTick_real() {
	return base_step * getScale().GetReal();
}

double TimelineTransformHorizontal::getLengthTick_lerped() {
	return base_step * getScale().GetLerped();
}

void TimelineTransformHorizontal::centerize(const double& relative_point, const bool& isScreen) {

	float screen_center = width_available / 2.f;
	float screen_point = isScreen == true ? relative_point : getPostionScreen_real(relative_point);

	real_offset -= screen_point - screen_center;
	real_offsetDelta = 0.0;
	real_offset = clampOffsetByHalfWidth_screen(real_offset);

	getOffset().Set(real_offset + real_offsetDelta);

}

void TimelineTransformHorizontal::addZoom(const double& value) {
	real_scale += value;
	getScale().Set(real_scale);
	if (clampZoomByTimeline()) {
		reaction_to_over_scale.SetAll(1.f);
		reaction_to_over_scale.Set(0.f);
	}
}
void TimelineTransformHorizontal::scaleZoomBy(const double& value) {
	zoomToPointByValue(width_available / 2.f, value);
}

void TimelineTransformHorizontal::setOffsetWithClamp(const double& value) {
	real_offset = clampOffsetByHalfWidth_screen(value);
	getOffset().Set(real_offset);
}

void TimelineTransformHorizontal::addOffset(const double& value) {
	real_offset += value;

	real_offset = clampOffsetByHalfWidth_screen(real_offset);

	getOffset().Set(real_offset + real_offsetDelta);
}
void TimelineTransformHorizontal::setOffsetDelta(const float& delta) {
	real_offsetDelta = delta;

	getOffset().Set(
		clampOffsetByHalfWidth_screen(
			real_offset + real_offsetDelta
		)
	);
}
float TimelineTransformHorizontal::getOffsetDelta() {
	return real_offsetDelta;
}
void TimelineTransformHorizontal::applyOffsetDelta() {
	real_offset += real_offsetDelta;
	real_offsetDelta = 0;

	real_offset = clampOffsetByHalfWidth_screen(real_offset);

	getOffset().Set(real_offset);
}

float TimelineTransformHorizontal::getLeftBoundView_global_real() {
	return getPostionGlobal_real(0.f);
}
float TimelineTransformHorizontal::getRightBoundView_global_real() {
	return getPostionGlobal_real(this->width_available);
}

float TimelineTransformHorizontal::getLeftBoundView_global_lerped() {
	return getPostionGlobal_lerped(0.f);
}
float TimelineTransformHorizontal::getRightBoundView_global_lerped() {
	return getPostionGlobal_lerped(this->width_available);
}


void TimelineTransformHorizontal::fitToRanges(const double& global_left, const double& global_right) {

	float screen_left = getPostionScreen_real(global_left);
	float screen_right = getPostionScreen_real(global_right);

	float width = std::abs(screen_left - screen_right);

	float scaleKoef = this->width_available / width;

	getScale().Set(getScale().GetReal() * scaleKoef);

	getOffset().Set(
		clampOffsetByHalfWidth_screen(
			getOffset().GetReal() - getPostionScreen_real(global_left)
		)
	);


	real_scale = getScale().GetReal();
	real_offset = getOffset().GetReal();
}

LerpedValue<float>& TimelineTransformHorizontal::getOffset() {
	return offset;
}
LerpedValue<float>& TimelineTransformHorizontal::getScale() {
	return scale;
}