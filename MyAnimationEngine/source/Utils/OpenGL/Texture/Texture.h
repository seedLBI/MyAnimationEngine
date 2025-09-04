#ifndef TEXTURE_H
#define TEXTURE_H


#define GLEW_STATIC
#include <GL/glew.h>

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <string>
#include <stdexcept>



#include "Utils/File/File.h"

#include "Utils/OpenGL/Enums/OpenGL.Enums.h"

struct TextureSetting {
	TextureWrap WrapX = TextureWrap::CLAMP_TO_EDGE;
	TextureWrap WrapY = TextureWrap::CLAMP_TO_EDGE;
	TextureFilter Min = TextureFilter::LINEAR;
	TextureFilter Max = TextureFilter::LINEAR;
};

struct TextureInfo {
	int width = 0;
	int height = 0;
	int channels = 0; 
};


class Texture {
public:
	Texture();
	Texture(TextureSetting setting,
		const int& MipMapLevel,
		TextureInternalPixelFormat internalFormat,
		int width,
		int height,
		TexturePixelFormat format,
		TextureDataType type,
		const void* data);
	Texture(const TextureSetting& setting, const std::string& Path2Image, const int& desiredCount_channels = 0);
	~Texture();

	void SetSetting(const TextureSetting& setting);
	void LoadTexture(const std::string& Path2Image,const int& desiredCount_channels = 0);

	void Init();

	int GetWidth();
	int GetHeight();

	int GetID();
private:
	GLuint ID;
	TextureSetting setting;
	TextureInfo info;

	uint8_t* dataImage = nullptr;

	int GetTypeImageData(const int& channels);
};







#endif