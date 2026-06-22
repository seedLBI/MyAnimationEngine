#pragma once
#include "Core/Animation/Timeline/Controller/Timeline.Controller.h"

class Timeline_Player {
public:
	Timeline_Player(TimelineController* controller);
	~Timeline_Player();

	void Draw();
	void Update();

private:
	TimelineController* controller;
	float multiplier = 1.f;
};