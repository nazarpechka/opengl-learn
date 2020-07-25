#include "Renderer.h"

#include <iostream>

void GLClearError() {
    while (glGetError() != GL_NO_ERROR)
        ;
}

bool GLLogCall(const char* function, const char* file, int line) {
    while (GLenum error = glGetError()) {
        std::cout << "[OpenGL Error] (0x" << std::hex << error << std::dec
                  << "): " << function << " " << file << ":" << line << '\n';
        return false;
    }
    return true;
}

void Renderer::Clear() const { GLCall(glClear(GL_COLOR_BUFFER_BIT)); }

void Renderer::Draw(const VertexArray& va, const IndexBuffer& ib,
                    const Shader& shader) const {
    shader.Bind();
    va.Bind();

    // Draw six vertices forming two triangles
    // glDrawArrays(GL_TRIANGLES, 0, 6); - if drawing without index buffers
    GLCall(
        glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr));
}