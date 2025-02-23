#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <iostream>

namespace Graphics {

class Texture {
public:
    Texture() {};
    ~Texture();
    void load(const char* filepath, bool flip=false);
    void bind();
    glm::ivec2 getSize() const { return glm::ivec2(w, h); };

private:
    GLuint texture;
    int w;
    int h;
    int numChannels;
};

Texture::~Texture() {
    if (texture) glDeleteTextures(1, &texture);
}

void Texture::load(const char* filepath, bool flip) {
    if (flip)
        stbi_set_flip_vertically_on_load(1);

    // load texture from file
    GLubyte* data = stbi_load(filepath, &w, &h, &numChannels, 0);
    if (!data) {
        std::cout << "Error: Failed to load texture:" << filepath << std::endl;
        exit(1);
    }

    // handle texture formatting
    GLint internalFormat;
    GLenum format;
    if (numChannels == 3) {
        internalFormat = GL_RGB;
        format = GL_RGB;
    }
    else if (numChannels == 4) {
        internalFormat = GL_RGBA;
        format = GL_RGBA;
    }

    // setup opengl texture
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, w, h, 0, format, GL_UNSIGNED_BYTE, data);

    // TODO: add generateMipmap flag
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);
}

void Texture::bind() {
    glBindTexture(GL_TEXTURE_2D, texture);
}

} // namespace Graphics