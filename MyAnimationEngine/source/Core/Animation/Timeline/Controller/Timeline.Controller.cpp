#include "Timeline.Controller.h"

void TimelineController::SetCursor(const uint64_t& frame) {
	state = TimelineRunState::PAUSE;
	data->GetData().SetTime((uint64_t)frame);
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

void TimelineController::PlayCycle()
{
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

void TimelineController::Update() {
	if (state == TimelineRunState::RUNNING || state == TimelineRunState::RUNNING_CYCLE) {
		timer.stop();


		switch (state)
		{
		case RUNNING:

			data->GetData().SetTime(
				data->GetData().currentTime + timer.elapsedSeconds()
			);

			break;
		case RUNNING_CYCLE:

			data->GetData().SetTime(
				std::fmod(
					data->GetData().currentTime + timer.elapsedSeconds(), 
					data->GetData().totalTime
				)
			);

			break;
		default:
			break;
		}


		timer.start();
	}
	
}
