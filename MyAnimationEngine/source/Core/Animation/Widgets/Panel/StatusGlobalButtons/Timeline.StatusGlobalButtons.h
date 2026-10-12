#ifndef TIMELINE_STATUS_GLOBAL_BUTTONS_H
#define TIMELINE_STATUS_GLOBAL_BUTTONS_H


class Timeline_StatusGlobalButtons {
public:
	Timeline_StatusGlobalButtons() = default;
	~Timeline_StatusGlobalButtons() = default;

	void SetFlag_GraphMode(bool status);
	void SetFlag_MotionBlurMode(bool status);

	const bool& isGraphMode();
	const bool& isMotionBlurMode();

private:
	bool GraphMode = true;
	bool MotionBlurMode = false;
};


#endif