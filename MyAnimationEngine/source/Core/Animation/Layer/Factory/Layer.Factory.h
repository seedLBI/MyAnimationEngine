#ifndef LAYER_FACTORY_H
#define LAYER_FACTORY_H

#include "Core/Animation/Layer/ProviderData/Layers.Data.Provider.h"
#include "Core/Animation/Timeline/ProviderData/Timeline.ProviderData.h"


#include <vector>

class LayerFactory {
public:
	LayerFactory(TimelineProviderData* timeline, LayersDataProvider* layers);
	~LayerFactory();

	void CreateSolid();
	void CreateSolid(const float& begin,const float& time);

private:
	LayersDataProvider* layers;
	TimelineProviderData* timeline;
};

#endif