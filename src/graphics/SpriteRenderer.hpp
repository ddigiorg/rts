// SpriteRenderer.hpp
#pragma once

#include "graphics/CameraManager.hpp"
#include "graphics/Shader.hpp"

#include <GL/glew.h>

namespace GFX {

// TODO: need to dynamically grow or shrink buffers depending on 2x the count

class SpriteRenderer {
public:
    SpriteRenderer() {};
    SpriteRenderer(size_t capacity);
    ~SpriteRenderer();
    void append(size_t count, const void* data);
    void render(Shader& shader, CameraManager& camera);

private:
    static float vertices[];
    static unsigned int indices[];

    unsigned int VAO = 0;
    unsigned int VBO = 0;
    unsigned int EBO = 0;
    unsigned int positionVBO = 0;

    size_t count = 0;
    size_t capacity = 0; // maximum number of sprites
};

// NOTE: if vertices changes then glVertexAttribPointer parameter 5 will need to change
GLfloat SpriteRenderer::vertices[] = {
    // positions    texcoords
    -10.5f, -10.5f,   //0.0f, 0.0f, // bottom left
     10.5f, -10.5f,   //1.0f, 0.0f, // bottom right
     10.5f,  10.5f,   //1.0f, 1.0f, // top right
    -10.5f,  10.5f,   //0.0f, 1.0f  // top left
};

GLuint SpriteRenderer::indices[] = {
    0, 1, 2, // first triangle
    2, 3, 0  // second triangle
};

SpriteRenderer::SpriteRenderer(size_t capacity) {
    this->capacity = capacity;

    // generate the opengl objects
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenBuffers(1, &positionVBO);

    // bind the vertex array object
    glBindVertexArray(VAO);

    // setup sprite vertices
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // setup sprite indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // setup sprite positions
    glBindBuffer(GL_ARRAY_BUFFER, positionVBO);
    glBufferData(GL_ARRAY_BUFFER, capacity * 2 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribDivisor(1, 1);

    // unbind vertex array object
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

SpriteRenderer::~SpriteRenderer() {
    if (VAO) glDeleteVertexArrays(1, &VAO);
    if (VBO) glDeleteBuffers(1, &VBO);
    if (EBO) glDeleteBuffers(1, &EBO);
    if (positionVBO) glDeleteBuffers(1, &positionVBO);
}

void SpriteRenderer::append(size_t count, const void* data) {
    // assert(this->count + count <= capacity);

    size_t offset = this->count * 2 * sizeof(float);
    size_t size = count * 2 * sizeof(float);

    glBindBuffer(GL_ARRAY_BUFFER, positionVBO);
    glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    this->count += count;
}

void SpriteRenderer::render(Shader& shader, CameraManager& camera) {
    if (count == 0)
        return;

    glm::mat4 vp =  camera.getViewProjMatrix();

    shader.use();
    shader.setUniformMatrix4fv("uVP", 1, vp);

    glBindVertexArray(VAO);
    glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, (GLsizei)count);
    glBindVertexArray(0);

    count = 0;
}

} // namespace GFX