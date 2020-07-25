#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"

/*
LocalBuffer is a pointer to RAM where texture is stored
BPP - bits per pixel?

*/
Texture::Texture(const std::string& path)
    : m_RendererID(0),
      m_FilePath(path),
      m_LocalBuffer(nullptr),
      m_Width(0),
      m_Height(0),
      m_BPP(0) {
    // Flip the texture because OpenGL expects pixels to
    // start at the bottom left, not the top left
    stbi_set_flip_vertically_on_load(1);

    // Last parameter is 4 because we want rgba
    m_LocalBuffer = stbi_load(path.c_str(), &m_Width, &m_Height, &m_BPP, 4);

    GLCall(glGenTextures(1, &m_RendererID));
    GLCall(glBindTexture(GL_TEXTURE_2D, m_RendererID));

    // Minification filter is used when texture needs to be sampled down
    // to be rendered on screen
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    // Sometimes we might need to render the texture on area which is larger
    // than the texture itself
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    // Parameters for texture wrapping
    // GL_CLAMP - don't extend the area
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

    // Send the data to OpenGL
    // internalFormat - how OpenGL will store the texture data
    // format - format of data you provide (m_LocalBuffer)
    GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0,
                        GL_RGBA, GL_UNSIGNED_BYTE, m_LocalBuffer));

    Unbind();

    if (m_LocalBuffer) stbi_image_free(m_LocalBuffer);
}

Texture::~Texture() { GLCall(glDeleteTextures(1, &m_RendererID)); }

void Texture::Bind(unsigned int slot) const {
    GLCall(glActiveTexture(GL_TEXTURE0 + slot));
    GLCall(glBindTexture(GL_TEXTURE_2D, m_RendererID));
}

void Texture::Unbind() const { GLCall(glBindTexture(GL_TEXTURE_2D, 0)); }
