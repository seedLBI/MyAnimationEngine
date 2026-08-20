#ifndef RENDERER_H
#define RENDERER_H

#include "Core/Animation/Layer/ProviderData/Layers.Data.Provider.h"

#include "Utils/OpenGL/Framebuffer/Framebuffer.h"
#include "Utils/OpenGL/Texture/Texture.h"


class RendererAnimationScene {
public:
	RendererAnimationScene(LayersDataProvider* layersData);
	~RendererAnimationScene();

	void Draw(float time);
	void Update();

	const Texture& getTexture();

private:
	LayersDataProvider* layersData;
	Framebuffer* output_framebuffer;
	Texture*	 output_texture;
};




#endif