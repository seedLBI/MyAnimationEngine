#include "Layer.Object.Solid.h"



LayerObject_Solid::~LayerObject_Solid()
{

}

void LayerObject_Solid::Draw()
{
	quad.Render();
}

void LayerObject_Solid::Update()
{
	quad.Update();
}

void LayerObject_Solid::DrawHitBoxViewport()
{
	
}

bool LayerObject_Solid::isHit() {
	return false;
}


LayerObject_Solid::LayerObject_Solid(const LayerInfo& info) : LayerObject(info)
{

}
