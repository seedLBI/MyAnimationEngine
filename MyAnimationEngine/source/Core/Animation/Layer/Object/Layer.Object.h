#ifndef LAYER_OBJECT_H
#define LAYER_OBJECT_H

#include "Core/Animation/ObjectProperties/ObjectProperties.h"
#include "Core/Animation/Layer/LayerInfo.h"
#include <vector>

class LayerObject {
public:
	virtual ~LayerObject() = default;
	LayerObject(const LayerInfo& info);

	virtual void Draw(float time) = 0;
	virtual void Update() = 0;

	LayerInfo& GetLayerInfo();
	ObjectProperties& GetProperties();

protected:
	LayerInfo layer_info;
	ObjectProperties properties;
};

#endif