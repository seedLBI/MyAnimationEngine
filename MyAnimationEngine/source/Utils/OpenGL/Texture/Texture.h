#ifndef TEXTURE_H
#define TEXTURE_H


#ifndef _DEBUG
#define GLEW_STATIC
#endif
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
	float Anisotropy = 0.f;
};

struct TextureInfo {
	int width = 0;
	int height = 0;
	int channels = 0; 
};


class Texture {
public:
	Texture(Texture&& other) noexcept;
	Texture& operator=(Texture&& other) noexcept;
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
	const TextureSetting& GetSetting() const;

	void LoadTexture(const std::string& Path2Image,const int& desiredCount_channels = 0);

	void Resize(int width, int height);

	void Init();

	int GetWidth();
	int GetHeight();
	float GetAspectRatio();

	GLuint GetID();
private:
	GLuint ID;
	TextureSetting setting;
	TextureInfo info;

	TextureInternalPixelFormat internalFormat = TextureInternalPixelFormat::RGBA8;
	TexturePixelFormat         pixelFormat = TexturePixelFormat::RGBA;
	TextureDataType            dataType = TextureDataType::Unsigned_byte;

	uint8_t* dataImage = nullptr;

	TextureInternalPixelFormat GetImageInternalPixelFormat(const int& channels);
	TexturePixelFormat GetImageTexturePixelFormat(const int& channels);

	void ApplySetting();
	bool IsMipmapFilter(TextureFilter f);
	bool NeedMipmaps();
};

#endif