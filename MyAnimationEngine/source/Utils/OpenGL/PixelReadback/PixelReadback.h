#ifndef PIXEL_READBACK_H
#define PIXEL_READBACK_H

#ifndef _DEBUG
#define GLEW_STATIC
#endif
#include <GL/glew.h>

#include "Utils/OpenGL/Enums/OpenGL.Enums.h"

class Framebuffer;

class PixelReadback
{
public:
    PixelReadback() = default;
    ~PixelReadback();

    PixelReadback(const PixelReadback&) = delete;
    PixelReadback& operator=(const PixelReadback&) = delete;

    // Асинхронно заказать чтение пикселя (x, y) из аттачмента FBO.
    // НЕ останавливает конвейер. Вызывать ПОСЛЕ рендера сцены в этот FBO.
    void Pick(Framebuffer& fbo, Buffer_Attachment attachment, int x, int y);

    // Забрать результат, заказанный в ПРОШЛОМ кадре (латентность 1 кадр).
    void Fetch();

    // Синхронное чтение «здесь и сейчас» (останавливает GPU!).
    // Оправдано только для редких событий — например по клику.
    void PickSync(Framebuffer& fbo, Buffer_Attachment attachment, int x, int y);

    const unsigned char* Data() const { return pixel; }

    float R() const { return pixel[0] * (1.f / 255.f); }
    float G() const { return pixel[1] * (1.f / 255.f); }
    float B() const { return pixel[2] * (1.f / 255.f); }
    float A() const { return pixel[3] * (1.f / 255.f); }

private:
    GLuint pbo[2] = { 0, 0 };
    int curr = 0; // PBO, в который пишем в этом кадре
    unsigned char pixel[4] = { 0, 0, 0, 255 };

    void ensurePBO();
};

#endif