#ifndef LAYER_OBJECT_H
#define LAYER_OBJECT_H

#include "Core/Animation/Component/Component.h"

#include <vector>


class LayerObject {
public:
	virtual ~LayerObject() = default;
	LayerObject(const LayerInfo& info);

	virtual void LayerList_DrawIconType() = 0;
	virtual void Draw() = 0;
	virtual void DrawHitBoxViewport() = 0;
	virtual bool isHit() = 0;
	virtual void Update() = 0;

	LayerInfo& GetLayerInfo();

protected:
	LayerInfo layer_info;
	std::vector<Component*> components;
	// base component for all layers
	Component_2DTransform transform;
};

#endif