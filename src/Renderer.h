#pragma once

#define GL_SILENCE_DEPRECATION
#include <GL/glew.h>
#include <signal.h>

#include "IndexBuffer.h"
#include "Shader.h"
#include "VertexArray.h"

#define ASSERT(x) \
    if (!(x)) raise(SIGTRAP)
#define GLCall(x)   \
    GLClearError(); \
    x;              \
    ASSERT(GLLogCall(#x, __FILE__, __LINE__))

void GLClearError();
bool GLLogCall(const char* function, const char* file, int line);

class Renderer {
   public:
    void Clear() const;
    void Draw(const VertexArray& va, const IndexBuffer& ib,
              const Shader& shader) const;
};