#include "Layer.Factory.h"

#include "Core/Animation/Layer/Object/Layer.Object.h"
#include "Core/Animation/Layer/Object/Objects/Layer.Object.Solid.h"

LayerFactory::~LayerFactory()
{

}

void LayerFactory::CreateSolid()
{
	LayerInfo newLayerInfo;
	newLayerInfo.name = u8"Сплошной";

	int count = 0;
	for (size_t i = 0; i < layers->GetData().layers.size(); i++) {

		if (typeid(*layers->GetData().layers[i].get()) == typeid(LayerObject_Solid))
			count++;

	}
	newLayerInfo.name += " " + std::to_string(count);


	newLayerInfo.time.Begin = 0;
	newLayerInfo.time.End = timeline->GetData().totalTime;
	newLayerInfo.isVisible = true;

	newLayerInfo.color.r = 1.f;
	newLayerInfo.color.g = 0.f;
	newLayerInfo.color.b = 0.2f;

	newLayerInfo.index = layers->GetData().layers.size();


	layers->GetData().layers.emplace_back(std::make_shared<LayerObject_Solid>(newLayerInfo));
}

void LayerFactory::CreateSolid(const float& begin, const float& time) {
	LayerInfo newLayerInfo;
	newLayerInfo.name = u8"Сплошной";

	int count = 0;
	for (size_t i = 0; i < layers->GetData().layers.size(); i++) {

		if (typeid(*layers->GetData().layers[i].get()) == typeid(LayerObject_Solid))
			count++;

	}

	newLayerInfo.name += " " + std::to_string(count);


	newLayerInfo.time.Begin = begin;
	newLayerInfo.time.End = begin + time;
	
	if (newLayerInfo.time.End > timeline->GetData().totalTime)
		newLayerInfo.time.End = timeline->GetData().totalTime;


	newLayerInfo.isVisible = true;

	newLayerInfo.color.r = 1.f;
	newLayerInfo.color.g = 0.f;
	newLayerInfo.color.b = 0.2f;

	newLayerInfo.index = layers->GetData().layers.size();


	layers->GetData().layers.emplace_back(std::make_shared<LayerObject_Solid>(newLayerInfo));
}


LayerFactory::LayerFactory(TimelineProviderData* timeline, LayersDataProvider* layers) : timeline(timeline), layers(layers) {

}
