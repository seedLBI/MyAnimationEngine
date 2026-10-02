#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "Utils/OpenGL/Texture/Texture.h"
#include "Utils/OpenGL/Renderbuffer/Renderbuffer.h"


class Framebuffer
{
public:
	Framebuffer();
	~Framebuffer();

	void SetCountDrawBuffers(const int& count);
	void AttachTexture(Texture& texture, const FramebufferAttachment& attachment);
	void AttachRenderBuffer(Renderbuffer& buffer, const FramebufferAttachment& attachment);

	bool CheckStatus();

	GLuint GetID();

	void Resize(int width, int height);

	void Bind();
	static void UnBind();

private:
	GLuint ID = 0;

	struct Attachment {
		Texture* texture = nullptr;
		Renderbuffer* renderbuffer = nullptr;
	};
	std::vector<Attachment> attachments;

	void Init();
};


#endif