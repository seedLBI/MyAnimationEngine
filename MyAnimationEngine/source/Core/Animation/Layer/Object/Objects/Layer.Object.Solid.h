#pragma once

#include "Core/Animation/Layer/Object/Layer.Object.h"
#include "RenderObjects/Quad/Quad.h"

class LayerObject_Solid : public LayerObject {
public:
	LayerObject_Solid(const LayerInfo& info);
	~LayerObject_Solid();

	void LayerList_DrawIconType() override;

	void Draw() override;
	void DrawHitBoxViewport() override;
	bool isHit() override;
	void Update() override;

private:
	Quad quad;
};

