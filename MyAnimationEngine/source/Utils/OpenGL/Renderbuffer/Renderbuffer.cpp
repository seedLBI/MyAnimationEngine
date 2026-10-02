#include "Renderbuffer.h"


Renderbuffer::Renderbuffer(const TextureInternalPixelFormat& InternalFormat, const GLuint& width, const GLuint& height)
	: internalFormat(InternalFormat), width(width), height(height) {
	glGenRenderbuffers(1, &ID);
	glBindRenderbuffer(GL_RENDERBUFFER, ID);
	glRenderbufferStorage(GL_RENDERBUFFER, (GLenum)InternalFormat, width, height);
}

Renderbuffer::~Renderbuffer() {
	glDeleteRenderbuffers(1, &ID);
}

void Renderbuffer::Resize(const GLuint& width, const GLuint& height) {
	if (width == 0 || height == 0) return;
	if (width == this->width && height == this->height) return;

	this->width = width;
	this->height = height;

	glBindRenderbuffer(GL_RENDERBUFFER, ID);
	glRenderbufferStorage(GL_RENDERBUFFER, (GLenum)internalFormat, width, height);
}

GLuint Renderbuffer::GetWidth() {
	return width;
}
GLuint Renderbuffer::GetHeight() {
	return height;
}
GLuint Renderbuffer::GetID() {
	return ID;
}