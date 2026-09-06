#include "Layer.Factory.h"

#include "Core/Animation/Layer/Object/Layer.Object.h"
#include "Core/Animation/Layer/Objects/Solid/Layer.Object.Solid.h"


uint64_t LayerFactory::layers_id_counter = 0;

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

	newLayerInfo.id = layers_id_counter;
	newLayerInfo.name += " " + std::to_string(count);


	newLayerInfo.time.Begin = 0;
	newLayerInfo.time.End = timeline->GetData().totalTime;

	newLayerInfo.isVisible = true;
	newLayerInfo.isExpanded = false;
	newLayerInfo.isLocked = false;
	newLayerInfo.isOnly = false;

	newLayerInfo.color.r = 1.f;
	newLayerInfo.color.g = 0.f;
	newLayerInfo.color.b = 0.2f;

	layers->AddLayer(std::make_shared<LayerObject_Solid>(newLayerInfo));
	layers_id_counter++;
}

void LayerFactory::CreateSolid(const float& begin, const float& time) {
	LayerInfo newLayerInfo;
	newLayerInfo.name = u8"Сплошной";

	int count = 0;
	for (size_t i = 0; i < layers->GetData().layers.size(); i++) {

		if (typeid(*layers->GetData().layers[i].get()) == typeid(LayerObject_Solid))
			count++;

	}

	newLayerInfo.id = layers_id_counter;

	newLayerInfo.name += " " + std::to_string(count);


	newLayerInfo.time.Begin = begin;
	newLayerInfo.time.End = begin + time;
	
	if (newLayerInfo.time.End > timeline->GetData().totalTime)
		newLayerInfo.time.End = timeline->GetData().totalTime;


	newLayerInfo.isVisible = true;
	newLayerInfo.isExpanded = false;
	newLayerInfo.isLocked = false;
	newLayerInfo.isOnly = false;


	newLayerInfo.color.r = 1.f;
	newLayerInfo.color.g = 0.f;
	newLayerInfo.color.b = 0.2f;

	layers->AddLayer(std::make_shared<LayerObject_Solid>(newLayerInfo));
	layers_id_counter++;
}


LayerFactory::LayerFactory(TimelineProviderData* timeline, LayersDataProvider* layers) : timeline(timeline), layers(layers) {
	this->layers_id_counter = 0;
}
