#pragma once

#include "ThirdParty/ImGui/imgui.h"
#include "ThirdParty/ImGui/imgui_internal.h"


#include "Core/Animation/Layer/ProviderData/Layers.Data.Provider.h"
#include "Core/Animation/Layer/Factory/Layer.Factory.h"

#include "Core/Animation/Timeline/Widgets/Panel/ListScrollController/Timeline.ListScrollController.h"

class Timeline_LayersList
{
public:
	Timeline_LayersList(LayersDataProvider* layers, LayerFactory* layerFactory, Timeline_ListScrollController* listScrollController);
	~Timeline_LayersList();

	void Draw();
	void Update();

private:
	LayersDataProvider* layers;
	LayerFactory* layerFactory;

	Timeline_ListScrollController* listScrollController;

	ImVec2 DrawInfoLine(const ImVec2& p);
	ImVec2 DrawObject(const ImVec2& p, std::shared_ptr<LayerObject>& object);
};

