#pragma once

#include <GL/glew.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <iostream>

namespace GFX {

class Texture {
public:
    Texture(const char* filepath);
    ~Texture();
    void bind();
    int getW() { return w; };
    int getH() { return h; };

private:
    GLuint texture;
    int w;
    int h;
    int numChannels;
};

Texture::Texture(const char* filepath) {
    // stbi_set_flip_vertically_on_load(1);

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
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, w, h, 0, format, GL_UNSIGNED_BYTE, data);

    stbi_image_free(data);
}

Texture::~Texture() {
    glDeleteTextures(1, &texture);
}

void Texture::bind() {
    glBindTexture(GL_TEXTURE_2D, texture);
}

} // namespace GFX