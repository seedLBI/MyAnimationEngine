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


void Texture::Resize(int width, int height) {
	if (width <= 0 || height <= 0)
		throw std::runtime_error("Texture::Resize(): invalid size");
	if (width == info.width && height == info.height)
		return;
	if (dataImage)
		throw std::runtime_error("Texture::Resize(): call Init() first (CPU data pending)");

	info.width = width;
	info.height = height;

	glBindTexture(GL_TEXTURE_2D, ID);
	glTexImage2D(GL_TEXTURE_2D, 0, (GLenum)this->internalFormat, width, height, 0, (GLenum)this->pixelFormat, (GLenum)this->dataType, nullptr);

	if (NeedMipmaps())
		glGenerateMipmap(GL_TEXTURE_2D);
}

void Texture::ApplySetting() {
	glBindTexture(GL_TEXTURE_2D, ID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (GLenum)setting.WrapX);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (GLenum)setting.WrapY);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLenum)setting.Min);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLenum)setting.Max);

	if (setting.Anisotropy > 1.f) {
		GLfloat maxAniso = 1.0f;
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAniso);
		GLfloat a = (setting.Anisotropy < maxAniso) ? setting.Anisotropy : maxAniso;
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, a);
	}
}

bool Texture::IsMipmapFilter(TextureFilter f) {
	switch (f) {
	case TextureFilter::NEAREST_MIPMAP_NEAREST:
	case TextureFilter::NEAREST_MIPMAP_LINEAR:
	case TextureFilter::LINEAR_MIPMAP_NEAREST:
	case TextureFilter::LINEAR_MIPMAP_LINEAR:
		return true;
	default: return false;
	}
}

bool Texture::NeedMipmaps() {
	return IsMipmapFilter(setting.Min) || IsMipmapFilter(setting.Max);
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
	this->internalFormat = internalFormat;
	this->pixelFormat = format;
	this->dataType = type;
	info.width = width;
	info.height = height;

	ApplySetting();

	glTexImage2D(GL_TEXTURE_2D, MipMapLevel, (GLenum)internalFormat, info.width, info.height, 0, (GLenum)format, (GLenum)type, data);

	if (NeedMipmaps() && data != nullptr) {
		glGenerateMipmap(GL_TEXTURE_2D);
	}


}


int Texture::GetWidth() {
	return info.width;
}
int Texture::GetHeight() {
	return info.height;
}

float Texture::GetAspectRatio() {
	return (float)info.width / (float)info.height;
}

GLuint Texture::GetID() {
	return ID;
}

void Texture::SetSetting(const TextureSetting& setting) {
#ifdef _DEBUG
	std::cout << "void Texture::SetSetting(const TextureSetting& setting)\n";
#endif
	this->setting = setting;

	if (this->setting.Max == TextureFilter::NEAREST_MIPMAP_NEAREST || this->setting.Max == TextureFilter::NEAREST_MIPMAP_LINEAR
		|| this->setting.Max == TextureFilter::LINEAR_MIPMAP_NEAREST || this->setting.Max == TextureFilter::LINEAR_MIPMAP_LINEAR){
		this->setting.Max = TextureFilter::LINEAR;

		#ifdef _DEBUG
				std::cout << "Wrong setting for MAG texture filter (setted linear)\n";
		#endif
	}

	ApplySetting();
	if (NeedMipmaps())
		glGenerateMipmap(GL_TEXTURE_2D);

}
TextureSetting Texture::GetSetting() {
	return this->setting;
}



TexturePixelFormat Texture::GetImageTexturePixelFormat(const int& channels) {
	switch (channels) {
	case 4:
		return TexturePixelFormat::RGBA;
		break;
	case 3:
		return TexturePixelFormat::RGB;
		break;
	case 2:
		return TexturePixelFormat::RG;
		break;
	case 1:
		return TexturePixelFormat::RED;
		break;
	case 0:
		exit(100);
		break;
	default:
		break;
	}
}

TextureInternalPixelFormat Texture::GetImageInternalPixelFormat(const int& channels) {
	switch (channels) {
	case 4:
		return TextureInternalPixelFormat::RGBA8;
		break;
	case 3:
		return TextureInternalPixelFormat::RGB8;
		break;
	case 2:
		return TextureInternalPixelFormat::RG8;
		break;
	case 1:
		return TextureInternalPixelFormat::R8;
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
	ApplySetting();

	if (dataImage) {


		this->internalFormat	= GetImageInternalPixelFormat(info.channels);
		this->pixelFormat		= GetImageTexturePixelFormat(info.channels);
		this->dataType			= TextureDataType::Unsigned_byte;

		glTexImage2D(GL_TEXTURE_2D, 0, (GLenum)this->internalFormat, info.width, info.height, 0, (GLenum)this->pixelFormat, (GLenum)this->dataType, dataImage);

		if (NeedMipmaps())
			glGenerateMipmap(GL_TEXTURE_2D);

		stbi_image_free(dataImage);
		dataImage = nullptr;
	}
	else
		throw std::runtime_error("Load image before initialization!!!");

}