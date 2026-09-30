#include "PixelReadback.h"
#include "Utils/OpenGL/Framebuffer/Framebuffer.h"

#include <cstring>

PixelReadback::~PixelReadback() {
    if (pbo[0])
        glDeleteBuffers(2, pbo);
}

void PixelReadback::ensurePBO() {
    if (pbo[0]) return;
    glGenBuffers(2, pbo);
    for (int i = 0; i < 2; ++i) {
        glBindBuffer(GL_PIXEL_PACK_BUFFER, pbo[i]);
        glBufferData(GL_PIXEL_PACK_BUFFER, sizeof(pixel), nullptr, GL_STREAM_READ);
    }
    glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
}

void PixelReadback::Pick(Framebuffer& fbo, Buffer_Attachment attachment, int x, int y) {
    ensurePBO();

    // сохраняем состояния, чтобы не ломать внешний рендер
    GLint prevFBO = 0, prevPBO = 0;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prevFBO);
    glGetIntegerv(GL_PIXEL_PACK_BUFFER_BINDING, &prevPBO);

    fbo.Bind();
    glReadBuffer(attachment);          // для FBO это состояние самого FBO
    glPixelStorei(GL_PACK_ALIGNMENT, 1);

    // nullptr => пиксель пишется в PBO через DMA, вызов возвращается сразу
    glBindBuffer(GL_PIXEL_PACK_BUFFER, pbo[curr]);
    glReadPixels(x, y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    glBindFramebuffer(GL_FRAMEBUFFER, prevFBO);
    glBindBuffer(GL_PIXEL_PACK_BUFFER, prevPBO);
}

void PixelReadback::Fetch() {
    if (!pbo[0]) return;

    GLint prevPBO = 0;
    glGetIntegerv(GL_PIXEL_PACK_BUFFER_BINDING, &prevPBO);

    // мапим PBO, записанный кадр назад — DMA давно завершён, ожидания нет
    glBindBuffer(GL_PIXEL_PACK_BUFFER, pbo[1 - curr]);
    void* ptr = glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY);
    if (ptr) {
        memcpy(pixel, ptr, sizeof(pixel));
        glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
    }
    glBindBuffer(GL_PIXEL_PACK_BUFFER, prevPBO);

    curr = 1 - curr;
}

void PixelReadback::PickSync(Framebuffer& fbo, Buffer_Attachment attachment, int x, int y) {
    GLint prevFBO = 0;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prevFBO);

    fbo.Bind();
    glReadBuffer(attachment);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadPixels(x, y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixel); // сразу в RAM

    glBindFramebuffer(GL_FRAMEBUFFER, prevFBO);
}