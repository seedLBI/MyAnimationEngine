#include "Texture.h"


#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#ifdef _DEBUG
#include <iostream>
#endif

#include "Utils/File/File.h"

Texture::Texture() {
	glGenTextures(1, &ID);
#ifdef _DEBUG
	std::cout << "Texture::Texture()\n";
#endif
}

Texture::~Texture() {
	if (dataImage)
		stbi_image_free(dataImage);
	glDeleteTextures(1, &ID);
#ifdef _DEBUG
	std::cout << "Texture::~Texture()\n";
#endif
}


Texture& Texture::operator=(Texture&& other) noexcept {
	if (this != &other) {
		if (dataImage) stbi_image_free(dataImage);
		glDeleteTextures(1, &ID);

		ID = other.ID;
		info = other.info;
		setting = other.setting;

		other.ID = 0;
		other.dataImage = nullptr;
	}
	return *this;
}


Texture::Texture(const TextureSetting& setting, const std::string& Path2Image, const int& desiredCount_channels) : Texture() {
	SetSetting(setting);
	LoadTexture(Path2Image, desiredCount_channels);
	Init();
}

Texture::Texture(
	TextureSetting setting,
	const int& MipMapLevel,
	TextureInternalPixelFormat internalFormat,
	int width,
	int height,
	TexturePixelFormat format,
	TextureDataType type,
	const void* data) : Texture()
{
	SetSetting(setting);
	info.width = width;
	info.height = height;

	glBindTexture(GL_TEXTURE_2D, ID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,		setting.WrapX);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,		setting.WrapY);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,	setting.Min);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,	setting.Max);

	glTexImage2D(GL_TEXTURE_2D, MipMapLevel, internalFormat, info.width, info.height, 0, format, type, data);
}


int Texture::GetWidth() {
	return info.width;
}
int Texture::GetHeight() {
	return info.height;
}

GLuint Texture::GetID() {
	return ID;
}

void Texture::SetSetting(const TextureSetting& setting) {
#ifdef _DEBUG
	std::cout << "void Texture::SetSetting(const TextureSetting& setting)\n";
#endif
	this->setting = setting;
}

int Texture::GetTypeImageData(const int& channels) {
	switch (channels) {
	case 4:
		return GL_RGBA;
		break;
	case 3:
		return GL_RGB;
		break;
	case 2:
		return GL_RG;
		break;
	case 1:
		return GL_R;
		break;
	case 0:
		exit(100);
		break;
	default:
		break;
	}
}

void Texture::LoadTexture(const std::string& Path2Image, const int& desiredCount_channels) {
	if (!isFileExist(Path2Image)) {
		throw std::runtime_error("Incorrect path to image: [" + Path2Image + "]");
	}

	stbi_set_flip_vertically_on_load(true);
	dataImage = stbi_load(Path2Image.c_str(), &info.width, &info.height, &info.channels, desiredCount_channels);

#ifdef _DEBUG
	std::cout << "[Info Texture] [" << Path2Image << "]\n";
	std::cout << "\t\tWidth:   [" << info.width << "]\n";
	std::cout << "\t\tHeight:  [" << info.height << "]\n";
	std::cout << "\t\tChannels:[" << info.channels << "]\n";
#endif


}


void Texture::Init() {
#ifdef _DEBUG
	std::cout << "Texture::Init()\n";
#endif
	glBindTexture(GL_TEXTURE_2D, ID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, setting.WrapX);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, setting.WrapY);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, setting.Min);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, setting.Max);

	GLfloat maxAnisotropy;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotropy);

	const int mipmapFilters[4] = {
		GL_NEAREST_MIPMAP_NEAREST,
		GL_NEAREST_MIPMAP_LINEAR,
		GL_LINEAR_MIPMAP_NEAREST,
		GL_LINEAR_MIPMAP_LINEAR };
	bool haveMipmapFilter = false;
	for (int i = 0; i < 4; i++)
	{
		if (setting.Min == mipmapFilters[i]
			|| setting.Max == mipmapFilters[i])
		{
			haveMipmapFilter = true;
			break;
		}
	}

	if (dataImage) {
		int TypeData = GetTypeImageData(info.channels);

		glTexImage2D(GL_TEXTURE_2D, 0, TypeData, info.width, info.height, 0, TypeData, GL_UNSIGNED_BYTE, dataImage);

		if (haveMipmapFilter) {
			glGenerateMipmap(GL_TEXTURE_2D);
		}

		stbi_image_free(dataImage);
		dataImage = nullptr;
	}
	else
		throw std::runtime_error("Load image before initialization!!!");






}