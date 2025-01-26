// WindowManager.hpp
#pragma once

#include "core/Defaults.hpp"

#include <SDL.h>
#include <GL/glew.h>

#include <iostream>

class WindowManager {
public:
    WindowManager();
    ~WindowManager();
    void resize();
    void clear();
    void swap();

    SDL_Window* getWindow() const { return window; };

private:
    SDL_Window* window = nullptr;
    SDL_GLContext context = nullptr;
};

WindowManager::WindowManager() {

    // setup SDL window
    window = SDL_CreateWindow(
        DEFAULT_WINDOW_TITLE,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        DEFAULT_WINDOW_WIDTH,
        DEFAULT_WINDOW_HEIGHT,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
    );
    if(window == nullptr) {
        std::cout << "Error: SDL_CreateWindow() failed:" << std::endl;
        std::cout << SDL_GetError() << std::endl;
        exit(1);
    }

    // set window minimum size
    SDL_SetWindowMinimumSize(
        window,
        DEFAULT_MIN_WINDOW_WIDTH,
        DEFAULT_MIN_WINDOW_HEIGHT
    );

    // setup OpenGL graphics context
    context = SDL_GL_CreateContext(window);
    if (context == nullptr) {
        std::cout << "Error: SDL_GL_CreateContext() failed:" << std::endl;
        std::cout << SDL_GetError() << std::endl;
        exit(1);
    }

    // setup GLEW
    GLenum err = glewInit();
    if (err != GLEW_OK){
        std::cout << "Error: glewInit() failed:" << std::endl;
        std::cout << glewGetErrorString(err) << std::endl;
        exit(1);
    }

    // setup OpenGL states
    glViewport(0, 0, DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

WindowManager::~WindowManager() {
    if (window) SDL_DestroyWindow(window);
    if (context) SDL_GL_DeleteContext(context);
}

void WindowManager::resize() {
    int width = NULL;
    int height = NULL;
    SDL_GetWindowSize(window, &width, &height);
    glViewport(0, 0, width, height);
}

void WindowManager::clear() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void WindowManager::swap() {
    SDL_GL_SwapWindow(window);
}