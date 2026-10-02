#ifndef RENDERBUFFER_H
#define RENDERBUFFER_H

#include "Utils/OpenGL/Enums/OpenGL.Enums.h"

class Renderbuffer
{
public:
	Renderbuffer(const TextureInternalPixelFormat& InternalFormat, const GLuint& width, const GLuint& height);
	~Renderbuffer();

	void Resize(const GLuint& width, const GLuint& height);

	GLuint GetID();

	GLuint GetWidth();
	GLuint GetHeight();

private:
	GLuint ID = 0;
	GLuint width = 0, height = 0;

	TextureInternalPixelFormat internalFormat = TextureInternalPixelFormat::DEPTH_COMPONENT;
};


#endif