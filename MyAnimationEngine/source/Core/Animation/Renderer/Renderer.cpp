#include "Renderer.h"

RendererAnimationScene::RendererAnimationScene(LayersDataProvider* layersData) : layersData(layersData) {

	TextureSetting ts;
	ts.Min = TextureFilter::LINEAR_MIPMAP_LINEAR;
	ts.Max = TextureFilter::LINEAR_MIPMAP_LINEAR;
	ts.WrapX = TextureWrap::CLAMP_TO_EDGE;
	ts.WrapY = TextureWrap::CLAMP_TO_EDGE;

	output_texture = new Texture(ts, 0, TextureInternalPixelFormat::RGBA8, 1920, 1080, TexturePixelFormat::RGBA, TextureDataType::_BYTE, nullptr);

	output_framebuffer = new Framebuffer;
	output_framebuffer->AttachTexture(*output_texture, Buffer_Attachment::COLOR_ATTACHMENT0);
	output_framebuffer->SetCountDrawBuffers(1);



}

RendererAnimationScene::~RendererAnimationScene() {
	output_texture->~Texture();
	delete output_texture;

	output_framebuffer->~Framebuffer();
	delete output_framebuffer;
}

void RendererAnimationScene::Draw(float time) {
	output_framebuffer->Bind();
	glClearColor(0.f, 0.f, 0.f, 1.f);

	auto& layers = layersData->GetData().layers;
	for (size_t i = 0; i < layers.size(); i++) {
		auto& layer = layers[i];
		bool is_visible = layer.get()->GetLayerInfo().isVisible;
		if (is_visible == false)
			continue;

		layer->Draw(time);
	}

	output_framebuffer->UnBind();

}

void RendererAnimationScene::Update() {

}

const Texture& RendererAnimationScene::getTexture() {
	return *output_texture;
}