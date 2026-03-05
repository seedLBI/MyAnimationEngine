#ifndef LAYER_OBJECT_H
#define LAYER_OBJECT_H

#include "Core/Animation/Component/Component.h"

#include <vector>


class LayerObject {
public:
	virtual ~LayerObject() = default;
	LayerObject(const LayerInfo& info);

	virtual void Draw();
	virtual void DrawHitBoxViewport();
	virtual bool isHit();
	virtual void Update();

	LayerInfo& GetLayerInfo();

protected:
	LayerInfo layer_info;

	Component_2DTransform transform;
	Component_Color color;
};

#endif