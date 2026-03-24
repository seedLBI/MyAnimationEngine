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

	void DrawObject(std::shared_ptr<LayerObject>& object);

	bool DrawVisible(std::shared_ptr<LayerObject>& object);
	bool DrawToggle(std::shared_ptr<LayerObject>& object);
	void DrawLayerColor(std::shared_ptr<LayerObject>& object);
	void DrawLayerIndex(std::shared_ptr<LayerObject>& object);
	bool DrawLayerIconType(std::shared_ptr<LayerObject>& object);
	bool DrawLayerName(std::shared_ptr<LayerObject>& object);
};


#endif