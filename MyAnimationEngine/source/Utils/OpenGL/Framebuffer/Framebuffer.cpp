#include "Framebuffer.h"

Framebuffer::Framebuffer() {
	Init();
}

Framebuffer::~Framebuffer() {
	glDeleteFramebuffers(1, &ID);
}


void Framebuffer::SetCountDrawBuffers(const int& count) {
	Bind();

	std::vector<GLenum> attachments;

	for (int i = 0; i < count; i++) {
		attachments.push_back((GLenum)FramebufferAttachment::COLOR_ATTACHMENT0 + i);
	}

	glDrawBuffers(count, attachments.data());
	
	UnBind();
}

void Framebuffer::AttachTexture(Texture& texture, const FramebufferAttachment& attachment) {
	Bind();
		glViewport(0, 0, texture.GetWidth(), texture.GetHeight());
		glFramebufferTexture2D(GL_FRAMEBUFFER, (GLenum)attachment, GL_TEXTURE_2D, texture.GetID(), 0);
	UnBind();
	attachments.push_back({ &texture, nullptr });
}
void Framebuffer::AttachRenderBuffer(Renderbuffer& buffer, const FramebufferAttachment& attachment) {
	Bind();
		glViewport(0, 0, buffer.GetWidth(), buffer.GetHeight());
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, (GLenum)attachment, GL_RENDERBUFFER, buffer.GetID());
	UnBind();
	attachments.push_back({ nullptr, &buffer });
}


void Framebuffer::Resize(int width, int height) {
	if (width <= 0 || height <= 0) return;

	for (Attachment& a : attachments) {
		if (a.texture)          a.texture->Resize(width, height);
		else if (a.renderbuffer) a.renderbuffer->Resize(width, height);
	}

	Bind();
	glViewport(0, 0, width, height);
	UnBind();
}

GLuint Framebuffer::GetID() {
	return this->ID;
}

void Framebuffer::Bind() {
	glBindFramebuffer(GL_FRAMEBUFFER, ID);
}
void Framebuffer::UnBind() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

bool Framebuffer::CheckStatus() {
	Bind();
	bool result = glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
	UnBind();
	return result;
}

void Framebuffer::Init() {
	glGenFramebuffers(1, &ID);
}