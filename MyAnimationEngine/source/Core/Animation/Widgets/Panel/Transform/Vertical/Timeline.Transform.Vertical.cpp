#include "Timeline.Transform.Vertical.h"

TimelineTransformVertical::TimelineTransformVertical() {
	offset.SetAll(0.f);
}
TimelineTransformVertical::~TimelineTransformVertical() {

}

void TimelineTransformVertical::update(const float& deltaTime) {
	offset.Update(deltaTime * 20.f);
	updateBounds();
}

void TimelineTransformVertical::updateAvailableHeight(const double& height_available) {
	this->height_available = height_available;
}
void TimelineTransformVertical::updateContentHeight(const double& height_content) {
	this->height_content = height_content + this->height_available/3.f;
}

const float& TimelineTransformVertical::getAvailableHeight() {
	return this->height_available;
}
const float& TimelineTransformVertical::getContentHeight() {
	return this->height_content;
}



float TimelineTransformVertical::getScaledHeightOneObject() {
	return ImGui::GetTextLineHeight()*0.9f;
}

void TimelineTransformVertical::addOffset(const float& value) {
	offset.Set(offset.GetReal() + value);
	updateBounds();
}

float TimelineTransformVertical::getOffset_lerped() {
	return offset.GetLerped();
}

void TimelineTransformVertical::updateBounds() {
	if (height_content - height_available < 0.f) {
		offset.Set(0.f);
	}
	else if (-offset.GetReal() >= height_content - height_available) {
		offset.Set(-( height_content  - height_available));
	}
	else if (offset.GetReal() >= 0.f) {
		offset.Set(0.f);
	}
}