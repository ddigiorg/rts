// Shader.hpp
#pragma once

#include <GL/glew.h>
#include <SDL.h>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

namespace GFX {

class Shader {
public:
    Shader(const char* vertFilepath, const char* fragFilepath);
    ~Shader();
    void use();
    void setUniform1f(const GLchar* name, GLfloat v0);
    void setUniform2f(const GLchar* name, GLfloat v0, GLfloat v1);
    void setUniform3f(const GLchar* name, GLfloat v0, GLfloat v1, GLfloat v2);
    void setUniform4f(const GLchar* name, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
    void setUniformMatrix4fv(const GLchar* name, GLsizei count, glm::mat4& value);
    void setUniformBlockBinding(const GLchar* name, GLuint binding);

private:
    void _loadShaderFromFile(GLuint& shader, const char* filepath, GLuint type);

    GLuint program;
};

Shader::Shader(const char* vertFilepath, const char* fragFilepath) {
    GLuint vertShader;
    GLuint fragShader;

    _loadShaderFromFile(vertShader, vertFilepath, GL_VERTEX_SHADER);
    _loadShaderFromFile(fragShader, fragFilepath, GL_FRAGMENT_SHADER);

    program = glCreateProgram();
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
        std::cout << "Error: Program linking failed." << std::endl
                  << "infoLog: " << infoLog << std::endl;
        delete[] infoLog;
        glDeleteProgram(program);
        exit(1);
    }

    glValidateProgram(program);
    glDeleteShader(vertShader);
    glDeleteShader(fragShader);
}

Shader::~Shader() {
    glDeleteProgram(program);
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

void Shader::setUniformMatrix4fv(const GLchar* name, GLsizei count, glm::mat4& value) {
    GLuint location = glGetUniformLocation(program, name);
    glUniformMatrix4fv(location, count, GL_FALSE, glm::value_ptr(value));
}

void Shader::setUniformBlockBinding(const GLchar* name, GLuint binding) {
    GLuint location = glGetUniformBlockIndex(program, name);
    glUniformBlockBinding(program, location, binding);
}

void Shader::_loadShaderFromFile(GLuint& shaderId, const char* filepath, GLuint type) {

    // read shader source from file
    std::string line = "";
    std::string source = "";
    std::ifstream file(filepath);
    if(file.is_open()) {
        while(std::getline(file, line)) {
            source += line + "\n";
        }
        file.close();
    }

    // load and compile shader
    shaderId = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shaderId, 1, &src, nullptr);
    glCompileShader(shaderId);

    // check for shader compile errors 
    GLint success;
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
    if(!success) {
        int length;
        glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &length);
        char* infoLog = new char[length];
        glGetShaderInfoLog(shaderId, length, &length, infoLog);
        std::cout << "Error: Shader compilation failed." << std::endl
                  << "filepath: " << filepath << std::endl
                  << "infoLog: " << infoLog << std::endl;
        delete[] infoLog;
        glDeleteShader(shaderId);
        exit(1);
    }
}

} // namespace GFX