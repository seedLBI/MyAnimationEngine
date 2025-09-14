#pragma once
#include "Core/Animation/Timeline/ProviderData/Timeline.ProviderData.h"
#include "Core/Animation/Layer/ProviderData/Layers.Data.Provider.h"
#include "Core/Animation/Layer/Factory/Layer.Factory.h"


#include "Core/Animation//Timeline/Widgets/Panel/UpperControls/Timeline.UpperControls.h"
#include "Core/Animation//Timeline/Widgets/Panel/LayersList/Timeline.LayersList.h"
#include "Core/Animation//Timeline/Widgets/Panel/TimelineEditor/Timeline.Editor.h"
#include "Core/Animation//Timeline/Widgets/Panel/ListScrollController/Timeline.ListScrollController.h"


class TimelinePanel {
public:
	TimelinePanel(TimelineProviderData* timelineData, LayersDataProvider* layersData, LayerFactory* layerFactory);
	~TimelinePanel();

	void Draw();
	void Update();

private:
	TimelineProviderData* timelineData = nullptr;
	LayersDataProvider* layersData = nullptr;
	LayerFactory* layerFactory = nullptr;


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



