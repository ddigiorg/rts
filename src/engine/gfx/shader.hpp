#pragma once

#include "engine/utilities/file.hpp"

#include <GL/glew.h>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

namespace GFX {

class Shader {
public:
    Shader() {};
    ~Shader();
    void load(const char* vertPath, const char* fragPath);
    void use();
    void setUniform1f(const GLchar* name, GLfloat v0);
    void setUniform2f(const GLchar* name, GLfloat v0, GLfloat v1);
    void setUniform3f(const GLchar* name, GLfloat v0, GLfloat v1, GLfloat v2);
    void setUniform4f(const GLchar* name, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
    void setUniformMatrix4fv(const GLchar* name, const GLsizei count, const glm::mat4x4& value);
    void setUniformBlockBinding(const GLchar* name, const GLuint binding);

private:
    void _loadShaderFromFile(GLuint& shader, const char* filepath, GLuint type);

    GLuint program = 0;
};

Shader::~Shader() {
    if (program) glDeleteProgram(program);
}

void Shader::load(const char* vertPath, const char* fragPath) {
    GLuint vertShader = 0;
    GLuint fragShader = 0;

    // load shaders
    _loadShaderFromFile(vertShader, vertPath, GL_VERTEX_SHADER);
    _loadShaderFromFile(fragShader, fragPath, GL_FRAGMENT_SHADER);

    // setup program
    program = glCreateProgram();
    if (program == 0) {
        std::cerr << "Error: Failed to create shader program." << std::endl;
        exit(1);
    }

    // attach shaders to program
    glAttachShader(program, vertShader);
    glAttachShader(program, fragShader);
    glLinkProgram(program);

    // check for linking errors
    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if(!success) {
        int length;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
        char* infoLog = new char[length];
        glGetProgramInfoLog(program, length, &length, infoLog);
        std::cerr << "Error: Failed to link shader program:" << std::endl
                  << "- infoLog: " << infoLog << std::endl;
        delete[] infoLog;
        glDeleteProgram(program);
        program = 0;
        exit(1);
    }

    // optionally validate the program (GL_VALIDATE_STATUS)
    glValidateProgram(program);

    // release shaders
    if (vertShader) glDeleteShader(vertShader);
    if (fragShader) glDeleteShader(fragShader);
}

void Shader::use() {
    glUseProgram(program);
}

void Shader::setUniform1f(const GLchar* name, GLfloat v0) {
    GLuint location = glGetUniformLocation(program, name);
    glUniform1f(location, v0);
}

void Shader::setUniform2f(const GLchar* name, GLfloat v0, GLfloat v1) {
    GLuint location = glGetUniformLocation(program, name);
    glUniform2f(location, v0, v1);
}

void Shader::setUniform3f(const GLchar* name, GLfloat v0, GLfloat v1, GLfloat v2) {
    GLuint location = glGetUniformLocation(program, name);
    glUniform3f(location, v0, v1, v2);
}

void Shader::setUniform4f(const GLchar* name, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3) {
    GLuint location = glGetUniformLocation(program, name);
    glUniform4f(location, v0, v1, v2, v3);
}

void Shader::setUniformMatrix4fv(const GLchar* name, const GLsizei count, const glm::mat4x4& value) {
    GLuint location = glGetUniformLocation(program, name);
    glUniformMatrix4fv(location, count, GL_FALSE, glm::value_ptr(value));
}

void Shader::setUniformBlockBinding(const GLchar* name, const GLuint binding) {
    GLuint location = glGetUniformBlockIndex(program, name);
    glUniformBlockBinding(program, location, binding);
}

void Shader::_loadShaderFromFile(GLuint& shader, const char* filepath, GLuint type) {
    shader = glCreateShader(type);
    if (shader == 0) {
        std::cerr << "Error: Failed to create shader object for " << filepath << std::endl;
        exit(1);
    }

    // compile the shader file
    std::string content = readFile(filepath);
    const char* source = content.c_str();
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    // check for shader compile errors 
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if(!success) {
        int length;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
        char* infoLog = new char[length];
        glGetShaderInfoLog(shader, length, &length, infoLog);
        std::cerr << "Error: Failed to compile shader:" << std::endl
                  << "- filepath = " << filepath << std::endl
                  << "- infoLog = " << infoLog << std::endl;
        delete[] infoLog;
        glDeleteShader(shader);
        shader = 0;
        exit(1);
    }
}

} // namespace GFX