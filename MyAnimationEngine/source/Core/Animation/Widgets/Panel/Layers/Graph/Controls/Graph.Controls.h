#ifndef GRAPH_CONTROLS_H
#define GRAPH_CONTROLS_H

#include "Core/Animation/Widgets/Panel/Context/Timeline.Context.h"

class GraphControls {
public:
	GraphControls(TimelineContext* ctx);
	~GraphControls();

	void FitSelectedGraph();
	void FitAllVertical();

	void ChangeTypeKeyframe(TypeKeyframe type);

private:
	TimelineContext* ctx;

	struct SelectedRect {
		float minTime, maxTime;
		float minValue, maxValue;
	};

	SelectedRect getSelectedRect();

};

#endif