#include "Layer.Object.h"

LayerInfo& LayerObject::GetLayerInfo() {
	return layer_info;
}

LayerObject::LayerObject(const LayerInfo& info) : layer_info(info) {

}
ObjectProperties& LayerObject::GetProperties() {
	return properties;
}

