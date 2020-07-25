#include "VertexBuffer.h"

#include "Renderer.h"

// Create one buffer in VRAM, bind it as GL_ARRAY_BUFFER, load data in it -
// 24 bytes from positions GL_STATIC_DRAW - STATIC means we use it many
// times, change once, and DRAW means we would DRAW it

VertexBuffer::VertexBuffer(const void *data, unsigned int size) {
    GLCall(glGenBuffers(1, &m_RendererID));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));
    GLCall(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));
}

VertexBuffer::~VertexBuffer() { GLCall(glDeleteBuffers(1, &m_RendererID)); }

void VertexBuffer::Bind() const {
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));
}

void VertexBuffer::Unbind() const { GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0)); }