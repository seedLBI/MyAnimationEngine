#pragma once

#include "Core/Animation/Component/Component.h"
#include "Core/Animation/Layer/ProviderData/Layers.Data.Provider.h"
#include "Core/Animation/Timeline/ProviderData/Timeline.ProviderData.h"



#include <vector>

class LayerFactory {
public:
	LayerFactory(TimelineProviderData* timeline, LayersDataProvider* layers);
	~LayerFactory();

	void CreateSolid();

private:
	LayersDataProvider* layers;
	TimelineProviderData* timeline;
};


