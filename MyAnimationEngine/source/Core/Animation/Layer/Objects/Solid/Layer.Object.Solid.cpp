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

	properties.get<TPColor>("color").kr.getDefaultValue()[0] = float(rand()) / float(RAND_MAX);
	properties.get<TPColor>("color").kr.getDefaultValue()[1] = float(rand()) / float(RAND_MAX);
	properties.get<TPColor>("color").kr.getDefaultValue()[2] = float(rand()) / float(RAND_MAX);
	properties.get<TPColor>("color").kr.getDefaultValue()[3] = 1.f;
}
