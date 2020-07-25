#pragma once

#include <vector>

#include "Renderer.h"

// Let OpenGL know what type our attributes are
// Attribute id, amount of values in attribute (1...4), type itself, should
// it be normalized or not, size of vertex, offset to attribute from vertex
// start

struct VertexAttribute {
    unsigned int type;
    unsigned int count;
    unsigned char normalized;

    static unsigned int GetSizeOfType(unsigned int type) {
        switch (type) {
            case GL_FLOAT:
                return 4;
            case GL_UNSIGNED_INT:
                return 4;
            case GL_UNSIGNED_BYTE:
                return 1;
        }
        ASSERT(false);
        return 0;
    }
};

class VertexBufferLayout {
   private:
    std::vector<VertexAttribute> m_Elements;
    unsigned int m_Stride;

   public:
    VertexBufferLayout() : m_Stride(0) {}

    template <typename T>
    void Push(unsigned int count);

    inline const std::vector<VertexAttribute>& GetElements() const {
        return m_Elements;
    }
    inline unsigned int GetStride() const { return m_Stride; }
};

template <typename T>
inline void VertexBufferLayout::Push(unsigned int count) {
    // static_assert(false);
    count++;
}

template <>
inline void VertexBufferLayout::Push<float>(unsigned int count) {
    m_Elements.push_back({GL_FLOAT, count, GL_FALSE});
    m_Stride += VertexAttribute::GetSizeOfType(GL_FLOAT) * count;
}

template <>
inline void VertexBufferLayout::Push<unsigned int>(unsigned int count) {
    m_Elements.push_back({GL_UNSIGNED_INT, count, GL_FALSE});
    m_Stride += VertexAttribute::GetSizeOfType(GL_UNSIGNED_INT) * count;
}

template <>
inline void VertexBufferLayout::Push<unsigned char>(unsigned int count) {
    m_Elements.push_back({GL_UNSIGNED_BYTE, count, GL_TRUE});
    m_Stride += VertexAttribute::GetSizeOfType(GL_UNSIGNED_BYTE) * count;
}