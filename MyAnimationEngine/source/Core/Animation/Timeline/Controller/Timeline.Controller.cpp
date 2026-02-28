#include "Timeline.Controller.h"

void TimelineController::SetCursor(const uint64_t& frame) {
	state = TimelineRunState::PAUSE;
	data->GetData().SetTime((uint64_t)frame);
}


void TimelineController::SetSpeedMultiplier(const float& multiplier) {
	speed_multiplier = multiplier;
}


void TimelineController::Pause() {
	state = TimelineRunState::PAUSE;
}

void TimelineController::Play() {


	if (data->GetData().currentTime == data->GetData().totalTime) {
		data->GetData().SetTime(0.0l);
		state = TimelineRunState::RUNNING;
		timer.start();
	}
	else if (state == TimelineRunState::RUNNING) {
		state = TimelineRunState::PAUSE;
		data->GetData().SetTime((uint64_t)data->GetData().currentFrame);
	}
	else {
		state = TimelineRunState::RUNNING;
		timer.start();
	}
}

void TimelineController::PlayCycle() {
	state = TimelineRunState::RUNNING_CYCLE;
	timer.start();
}

void TimelineController::FirstFrame()
{
	state = TimelineRunState::PAUSE;
	data->GetData().SetTime((uint64_t)0);
}

void TimelineController::PrevFrame()
{
	state = TimelineRunState::PAUSE;
	data->GetData().SetTime((uint64_t)data->GetData().currentFrame - 1);
}

void TimelineController::NextFrame()
{
	state = TimelineRunState::PAUSE;
	data->GetData().SetTime((uint64_t)data->GetData().currentFrame + 1);
}

void TimelineController::LastFrame()
{
	state = TimelineRunState::PAUSE;
	data->GetData().SetTime((uint64_t)data->GetData().totalFrames);
}

const TimelineRunState& TimelineController::GetState() {
	return state;
}

void TimelineController::Update() {
	if (state == TimelineRunState::RUNNING || state == TimelineRunState::RUNNING_CYCLE) {
		timer.stop();


		float elapsed_seconds = timer.elapsedSeconds() * speed_multiplier;

		switch (state)
		{
		case RUNNING: {

			data->GetData().SetTime(
				data->GetData().currentTime + elapsed_seconds
			);

			break;
		}
		case RUNNING_CYCLE: {


			if (data->GetData().currentTime < data->GetData().left_range_repeat) {
				data->GetData().SetTime(data->GetData().left_range_repeat);
			}

			const float range_length = data->GetData().right_range_repeat - data->GetData().left_range_repeat;

			float length_currentTime = data->GetData().currentTime - data->GetData().left_range_repeat;

			length_currentTime = std::fmod(length_currentTime + elapsed_seconds, range_length);

			data->GetData().SetTime(data->GetData().left_range_repeat + length_currentTime);


			break;
		}
		default:
			break;
		}


		timer.start();
	}
	
}
