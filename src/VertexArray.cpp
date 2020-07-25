#include "VertexArray.h"

#include <iostream>

#include "Renderer.h"
#include "VertexBufferLayout.h"

// Vertex array object is needed to bind vertex buffer to specific
// vertex layout for easy later use.
// Then, when rendering you need to only bind vertex array instead of
// vertex buffer + vertex attrib array
// for example glBindVertexArray(vao);

// GLuint vao;
// GLCall(glGenVertexArrays(1, &vao));
// GLCall(glBindVertexArray(vao));

VertexArray::VertexArray() { GLCall(glGenVertexArrays(1, &m_RendererID)); }
VertexArray::~VertexArray() { GLCall(glDeleteVertexArrays(1, &m_RendererID)); }

void VertexArray::AddBuffer(const VertexBuffer &vb,
                            const VertexBufferLayout &layout) {
    Bind();
    vb.Bind();
    const auto &elements = layout.GetElements();
    unsigned int offset = 0;
    for (unsigned int i = 0; i < elements.size(); ++i) {
        const auto &element = elements[i];
        GLCall(glEnableVertexAttribArray(i));
        GLCall(glVertexAttribPointer(i, element.count, element.type,
                                     element.normalized, layout.GetStride(),
                                     reinterpret_cast<const void *>(offset)));
        offset += element.count * VertexAttribute::GetSizeOfType(element.type);
    }
}

void VertexArray::Bind() const { GLCall(glBindVertexArray(m_RendererID)); }

void VertexArray::Unbind() const { GLCall(glBindVertexArray(0)); }
