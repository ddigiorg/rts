#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

class VertexBuffer {
public:
    VertexBuffer(
            GLuint  attributeIndex,
            GLint   componentCount,
            GLenum  componentType,
            GLsizei stride,
            GLuint  divisor = 0,
            GLenum  usage = GL_STATIC_DRAW,
            GLboolean normalized = GL_FALSE
    ) : attributeIndex(attributeIndex),
        componentCount(componentCount),
        componentType(componentType),
        stride(stride),
        divisor(divisor),
        usage(usage),
        normalized(normalized),
        vbo(0), capacity(0) {
            glGenBuffers(1, &vbo);
        }

    ~VertexBuffer();
    void bind()   const { glBindBuffer(GL_ARRAY_BUFFER, vbo); }
    void unbind() const { glBindBuffer(GL_ARRAY_BUFFER, 0);   }
    void allocate(size_t newCapacity);
    void resize(size_t newCapacity);
    void update(size_t offset, size_t count, const void* data);

private:
    void setupAttribute();

    GLuint  attributeIndex; // Shader attribute location
    GLint   componentCount; // Components per vertex attribute (1-4)
    GLenum  componentType;  // GL_FLOAT, GL_UNSIGNED_BYTE, etc.
    GLsizei stride;         // Byte spacing between attributes
    GLuint  divisor;        // Instancing divisor
    GLenum  usage;          // GL_STATIC_DRAW or GL_DYNAMIC_DRAW
    GLboolean normalized;   // Normalize integer values

    GLuint vbo;
    size_t capacity;
};

VertexBuffer::~VertexBuffer() {
    if (vbo) {
        glDeleteBuffers(1, &vbo);
        vbo = 0;
    }
}

void VertexBuffer::allocate(size_t newCapacity) {
    bind();
    glBufferData(GL_ARRAY_BUFFER, newCapacity * stride, nullptr, usage);
    setupAttribute();
    unbind();
    capacity = newCapacity;
}

void VertexBuffer::resize(size_t newCapacity) {
    GLuint newVBO;
    glGenBuffers(1, &newVBO);

    // Create bigger buffer
    glBindBuffer(GL_COPY_WRITE_BUFFER, newVBO);
    glBufferData(GL_COPY_WRITE_BUFFER, newCapacity * stride, nullptr, usage);

    // Copy old data
    glBindBuffer(GL_COPY_READ_BUFFER, vbo);
    glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, capacity * stride);

    // Delete old buffer and update handle
    glDeleteBuffers(1, &vbo);
    vbo = newVBO;

    // Rebind attributes
    bind();
    setupAttribute();
    unbind();

    glBindBuffer(GL_COPY_READ_BUFFER, 0);
    glBindBuffer(GL_COPY_WRITE_BUFFER, 0);

    capacity = newCapacity;
}

void VertexBuffer::update(size_t offset, size_t count, const void* data) {
    bind();
    glBufferSubData(GL_ARRAY_BUFFER, offset * stride, count * stride, data);
    unbind();
}

void VertexBuffer::setupAttribute() {
    glVertexAttribPointer(attributeIndex, componentCount, componentType, normalized, stride, (void*)0);
    glEnableVertexAttribArray(attributeIndex);
    if (divisor != 0) {
        glVertexAttribDivisor(attributeIndex, divisor);
    }
}