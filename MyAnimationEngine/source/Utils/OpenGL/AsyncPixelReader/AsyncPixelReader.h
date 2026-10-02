#ifndef ASYNC_PIXEL_READER_H
#define ASYNC_PIXEL_READER_H

#include "Utils/OpenGL/Enums/OpenGL.Enums.h"
#include <glm/glm.hpp>

class AsyncPixelReader {
public:
    void init() {
        glGenBuffers(2, pbo_);
        for (int i = 0; i < 2; ++i) {
            glBindBuffer(GL_PIXEL_PACK_BUFFER, pbo_[i]);
            glBufferData(GL_PIXEL_PACK_BUFFER, sizeof(glm::u8vec4), nullptr, GL_STREAM_READ);
        }
        glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
    }

    // в конце кадра, после рендера сцены:
    void request(GLuint fbo, int x, int y) {
        if (fence_[idx_]) glDeleteSync(fence_[idx_]);
        glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
        glReadBuffer(GL_COLOR_ATTACHMENT0);
        glBindBuffer(GL_PIXEL_PACK_BUFFER, pbo_[idx_]);
        glReadPixels(x, y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, nullptr); // в PBO
        glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
        fence_[idx_] = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
        idx_ = 1 - idx_;
    }

    // в начале следующего кадра:
    bool fetch(glm::u8vec4& out) {
        int i = 1 - idx_; // буфер прошлого запроса
        if (!fence_[i]) return false;
        GLenum st = glClientWaitSync(fence_[i], 0, 0);            // неблокирующая проверка
        if (st != GL_ALREADY_SIGNALED && st != GL_CONDITION_SATISFIED) return false;
        glDeleteSync(fence_[i]); fence_[i] = nullptr;
        glBindBuffer(GL_PIXEL_PACK_BUFFER, pbo_[i]);
        auto* d = (glm::u8vec4*)glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY);
        if (!d) { glBindBuffer(GL_PIXEL_PACK_BUFFER, 0); return false; }
        out = *d;
        glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
        glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
        return true;
    }
private:
    GLuint pbo_[2]{};
    GLsync fence_[2]{};
    int idx_ = 0;
};

#endif