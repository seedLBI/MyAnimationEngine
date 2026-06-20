#include "Layer.Object.Solid.h"


LayerObject_Solid::~LayerObject_Solid()
{

}

void LayerObject_Solid::Draw(float time) {
	quad.Render();
}

void LayerObject_Solid::Update() {
	quad.Update();
}

LayerObject_Solid::LayerObject_Solid(const LayerInfo& info) : LayerObject(info)
{
	properties.set("color",		TPColor());
	properties.set("scale",		TP2Float());
	properties.set("pos",		TP2Float());
	properties.set("anchor",	TP2Float());
	properties.set("rotation",	TP3Float());

	auto color = properties.get<TPColor>("color").kr.getDefaultValue();

	color[0] = float(rand()) / float(RAND_MAX);
	color[1] = float(rand()) / float(RAND_MAX);
	color[2] = float(rand()) / float(RAND_MAX);
	color[3] = 1.f;

	properties.get<TPColor>("color").kr.setDefaultValue(color);
}
