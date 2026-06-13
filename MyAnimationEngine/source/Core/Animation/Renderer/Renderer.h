#ifndef RENDERER_H
#define RENDERER_H

#include "Core/Animation/Layer/ProviderData/Layers.Data.Provider.h"

class RendererAnimationScene {
public:
	RendererAnimationScene(LayersDataProvider* layersData);
	~RendererAnimationScene();

	void Draw(float time);
	void Update();

private:
	LayersDataProvider* layersData;
};

RendererAnimationScene::RendererAnimationScene(LayersDataProvider* layersData): layersData(layersData) {

}

RendererAnimationScene::~RendererAnimationScene(){

}

void RendererAnimationScene::Draw(float time) {
	auto& layers = layersData->GetData().layers;

	for (size_t i = 0; i < layers.size(); i++) {
		auto& layer = layers[i];
		bool is_visible = layer.get()->GetLayerInfo().isVisible;
		if (is_visible == false)
			continue;

		layer->Draw(time);
	}

}

void RendererAnimationScene::Update() {

}


#endif