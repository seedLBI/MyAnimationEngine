#include "Texture.Noise.h"

TextureSetting NoiseTexture::getSetting() {
	TextureSetting settings;
	settings.Min = TextureFilter::LINEAR;
	settings.Max = TextureFilter::LINEAR;
	settings.WrapX = TextureWrap::MIRRORED_REPEAT;
	settings.WrapY = TextureWrap::MIRRORED_REPEAT;
	return settings;
}


NoiseTexture& NoiseTexture::getInstance() {
	static NoiseTexture instance;
	return instance;
}

GLuint NoiseTexture::getID() {
	return texture->GetID();
}

void NoiseTexture::Init() {

	texture = new Texture(
		NoiseTexture::getSetting(),
		0,
		TextureInternalPixelFormat::RGBA8,
		512,
		512,
		TexturePixelFormat::RGBA,
		TextureDataType::Unsigned_byte,
		nullptr
	);
	framebuffer = new Framebuffer();
	quad = new Quad();
	shader = new Shader(
		"SHADER_FOR_NOISE",

R"VERTEX(
#version 330 core

layout(location = 0) in vec3 localPos;
layout(location = 1) in vec2 localUV;

void main(){
	gl_Position = vec4(localPos,1.0);
}
)VERTEX",



R"FRAG(
#version 330 core


float hash(vec2 p) { return fract(1e4 * sin(17.0 * p.x + p.y * 0.1) * (0.1 + abs(sin(p.y * 13.0 + p.x)))); }

float noise(vec2 x) {
	vec2 i = floor(x);
	vec2 f = fract(x);

	float a = hash(i);
	float b = hash(i + vec2(1.0, 0.0));
	float c = hash(i + vec2(0.0, 1.0));
	float d = hash(i + vec2(1.0, 1.0));

	vec2 u = f * f * (3.0 - 2.0 * f);
	return mix(a, b, u.x) + (c - a) * u.y * (1.0 - u.x) + (d - b) * u.x * u.y;
}

out vec4 fragColor;

void main() {
	float value = noise(gl_FragCoord.xy * 2.f + vec2(475.31) );
	vec3 color = vec3((value));
    fragColor = vec4(color, 1.f - color.x);
}

)FRAG");

	GLint last_viewport[4];
	glGetIntegerv(GL_VIEWPORT, last_viewport);


	framebuffer->AttachTexture(*texture, FramebufferAttachment::COLOR_ATTACHMENT0);
	framebuffer->SetCountDrawBuffers(1);

	framebuffer->Bind();
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "ОШИБКА: Framebuffer incomplete! Status: " << status << std::endl;
	}
	glViewport(0, 0, texture->GetWidth(), texture->GetHeight());

		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		shader->use();
		quad->Render();


	framebuffer->UnBind();
	glViewport(last_viewport[0], last_viewport[1], last_viewport[2], last_viewport[3]);
}

#include <iostream>
NoiseTexture::NoiseTexture() {
	std::cout << "NoiseTexture::NoiseTexture()\n";
	Init();
}
NoiseTexture::~NoiseTexture() {
	std::cout << "NoiseTexture::~NoiseTexture()\n";

	texture->~Texture();
	delete texture;
	texture = nullptr;

	framebuffer->~Framebuffer();
	delete framebuffer;
	framebuffer = nullptr;

	quad->~Quad();
	delete quad;
	quad = nullptr;

	shader->~Shader();
	delete shader;
	shader = nullptr;
}