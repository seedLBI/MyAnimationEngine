#include "Layer.Object.h"

LayerInfo& LayerObject::GetLayerInfo() {
	return layer_info;
}

LayerObject::LayerObject(const LayerInfo& info) : layer_info(info) {

}

void LayerObject::Draw()
{

}

void LayerObject::DrawHitBoxViewport() {

}

void LayerObject::Update()
{

}

