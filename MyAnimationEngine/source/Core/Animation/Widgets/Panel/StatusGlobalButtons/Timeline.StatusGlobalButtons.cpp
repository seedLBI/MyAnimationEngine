#include "Timeline.StatusGlobalButtons.h"

void Timeline_StatusGlobalButtons::SetFlag_GraphMode(bool status) {
	this->GraphMode = status;
}
void Timeline_StatusGlobalButtons::SetFlag_MotionBlurMode(bool status) {
	this->MotionBlurMode = status;
}

const bool& Timeline_StatusGlobalButtons::isGraphMode() {
	return this->GraphMode;
}
const bool& Timeline_StatusGlobalButtons::isMotionBlurMode() {
	return this->MotionBlurMode;
}
