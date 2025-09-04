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
	void AttachTexture(Texture& texture, const Buffer_Attachment& attachment);
	void AttachRenderBuffer(Renderbuffer& buffer, const Buffer_Attachment& attachment);

	void Bind();
	static void UnBind();

private:
	GLuint ID = 0;

	void Init();
};


#endif