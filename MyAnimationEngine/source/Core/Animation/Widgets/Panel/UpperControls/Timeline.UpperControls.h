#ifndef TIMELINE_UPPER_CONTROLS_H
#define TIMELINE_UPPER_CONTROLS_H

#include "Core/Animation/Widgets/Panel/Context/Timeline.Context.h"

#include "ThirdParty/ImGui/imgui.h"
#include "Utils/Color/Utils.Color.h"
#include <string>

class Timeline_UpperControls {
public:
	Timeline_UpperControls(TimelineContext* ctx);
	~Timeline_UpperControls();

	void Draw();
	void Update();

private:
	TimelineContext* ctx;

	float DrawTimer();
	float DrawFinder(float availableWidth);
	float DrawButtons();

	std::string getTimer();
	std::string getFrame();
};

#endif