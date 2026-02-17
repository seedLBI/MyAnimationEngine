#ifndef TIMELINE_PANEL_H
#define TIMELINE_PANEL_H

#include "Core/Animation/Timeline/ProviderData/Timeline.ProviderData.h"
#include "Core/Animation/Timeline/Transform/Horizontal/Timeline.Transform.Horizontal.h"
#include "Core/Animation/Layer/ProviderData/Layers.Data.Provider.h"
#include "Core/Animation/Layer/Factory/Layer.Factory.h"



//elements ruler
#include "UpperControls/Timeline.UpperControls.h"
#include "TimelineEditor/Timeline.Editor.h"
#include "Ruler/Timeline.Ruler.h"

#include "Core/Animation//Timeline/Widgets/Panel/LayersList/Timeline.LayersList.h"
#include "Core/Animation//Timeline/Widgets/Panel/ListScrollController/Timeline.ListScrollController.h"


#include "Core/DebugOverlay/DebugOverlay.h"

#include "Utils/Timer/Timer.Framerate.h"


class TimelinePanel {
public:
	TimelinePanel(FPS_Timer* deltaTime, TimelineProviderData* timelineData, LayersDataProvider* layersData, LayerFactory* layerFactory);
	~TimelinePanel();

	void Draw();
	void Update();

private:
	FPS_Timer* deltaTime = nullptr;
	TimelineProviderData* timelineData = nullptr;
	TimelineTransformHorizontal* timelineTransformHorizontal = nullptr;

	LayersDataProvider* layersData = nullptr;
	LayerFactory* layerFactory = nullptr;

	Timeline_Ruler* timelineRuler;
	Timeline_Editor* timelineEditor;
	Timeline_LayersList* layersList;
	Timeline_UpperControls* upperControls;
	Timeline_ListScrollController* listScrollController;

	void DrawLeftSide();
	void DrawRightSide();

	void DrawEditors();
	void DrawUpperControls();
	void DrawBottomControls();
	void DrawRuler();
	void DrawLayerList();


};

#endif

