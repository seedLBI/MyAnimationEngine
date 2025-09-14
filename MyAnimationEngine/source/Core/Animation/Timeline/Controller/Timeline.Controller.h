#pragma once

#include "Core/Animation/Timeline/ProviderData/Timeline.ProviderData.h"
#include "Utils/Timer/Timer.h"

enum TimelineRunState {
	PAUSE,
	RUNNING,
	RUNNING_CYCLE,
};

class TimelineController {
public:
	TimelineController(TimelineProviderData* data, TimelineRunState state = TimelineRunState::PAUSE) : data(data), state(state){}
	~TimelineController(){};

	void SetCursor(const uint64_t& frame);
	void Pause();
	void Play();
	void PlayCycle();

	void FirstFrame();
	void PrevFrame();
	void NextFrame();
	void LastFrame();


	void Update();

private:
	TimelineProviderData* data;

	Timer timer;

	TimelineRunState state;
};