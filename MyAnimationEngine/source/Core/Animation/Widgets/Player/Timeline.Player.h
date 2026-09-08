#ifndef TIMELINE_PLAYER_H
#define TIMELINE_PLAYER_H

#include "Core/Animation/Timeline/Controller/Timeline.Controller.h"
#include "Core/Animation/Widgets/Colors/Widgets.Colors.h"
#include "Utils/ImGui/Utils.ImGui.h"

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

#endif