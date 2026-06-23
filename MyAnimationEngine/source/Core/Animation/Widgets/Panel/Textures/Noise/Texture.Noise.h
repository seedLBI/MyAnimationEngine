#ifndef TEXTURE_NOISE_H
#define TEXTURE_NOISE_H

#include "Utils/OpenGL/Framebuffer/Framebuffer.h"
#include "Utils/OpenGL/Texture/Texture.h"
#include "Utils/OpenGL/Shaders/Shader.h"
#include "RenderObjects/Quad/Quad.h"


class NoiseTexture {
public:
	NoiseTexture(const NoiseTexture&) = delete;
	NoiseTexture& operator=(const NoiseTexture&) = delete;

	static NoiseTexture& getInstance();

	GLuint getID();

private:
	NoiseTexture();
	~NoiseTexture();

	Framebuffer* framebuffer;
	Texture* texture;
	Quad* quad;
	Shader* shader;


	TextureSetting getSetting();
	void Init();
};

#endif