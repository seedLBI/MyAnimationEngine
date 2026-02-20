#ifndef TIMELINE_LAYERS_LIST_H
#define TIMELINE_LAYERS_LIST_H

#include "Core/Animation/Timeline/Transform/Vertical/Timeline.Transform.Vertical.h"
#include "Core/Animation/Layer/ProviderData/Layers.Data.Provider.h"

class Timeline_Layers_List {
public:
	Timeline_Layers_List(LayersDataProvider* layersData, TimelineTransformVertical* verticalTransform);
	~Timeline_Layers_List();

	void Draw();
	void Update();

private:
	LayersDataProvider* layersData;
	TimelineTransformVertical* verticalTransform;

	ImVec2 DrawInfoLine(const ImVec2& p);
	ImVec2 DrawObject(const ImVec2& p, std::shared_ptr<LayerObject>& object);
};


#endif