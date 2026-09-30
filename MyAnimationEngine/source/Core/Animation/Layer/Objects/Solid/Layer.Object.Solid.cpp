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
	properties.set("scale",		TP3Float());
	properties.set("pos",		TP3Float());
	properties.set("anchor",	TP3Float());
	properties.set("rotation",	TP3Float());

	auto color = properties.get<TPColor>("color").kr.getDefaultValue();

	color[0] = float(rand()) / float(RAND_MAX);
	color[1] = float(rand()) / float(RAND_MAX);
	color[2] = float(rand()) / float(RAND_MAX);
	color[3] = 1.f;

	properties.get<TPColor>("color").kr.setDefaultValue(color);

	std::array<float, 3> scale{ 1.f, 1.f, 1.f };
	properties.get<TP3Float>("scale").kr.setDefaultValue(scale);

	std::array<float, 3> pos{ 0.f, 0.f, 0.f };
	properties.get<TP3Float>("pos").kr.setDefaultValue(pos);

	std::array<float, 3> anchor{ 0.f, 0.f, 0.f };
	properties.get<TP3Float>("anchor").kr.setDefaultValue(anchor);

	std::array<float, 3> rotation{ 0.f, 0.f, 0.f };
	properties.get<TP3Float>("rotation").kr.setDefaultValue(rotation);

}
