#ifndef LAYER_OBJECT_SOLID_H
#define LAYER_OBJECT_SOLID_H


#include "Core/Animation/Layer/Object/Layer.Object.h"
#include "RenderObjects/Quad/Quad.h"

class LayerObject_Solid : public LayerObject {
public:
	LayerObject_Solid(const LayerInfo& info);
	~LayerObject_Solid();

	void Draw(float time) override;
	void Update() override;

private:
	Quad quad;
};

#endif