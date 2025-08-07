#pragma once

#include "config/defaults.hpp"
#include "graphics/opengl_debug.hpp"

#include <SDL3/SDL.h>
#include <GL/glew.h>

#include <iostream>

class RenderContext {
public:
    RenderContext();
    ~RenderContext();
    void clearWindow();
    void swapWindowBuffers();

private:
    SDL_Window* window = nullptr;
    SDL_GLContext context = nullptr;
};

RenderContext::RenderContext() {
    bool success = false;

    // initialize SDL
    success = SDL_Init(SDL_INIT_VIDEO);
    if(!success) {
        // TODO: need better error handling and logging
        std::cout << "Error: SDL_Init() failed:" << std::endl;
        std::cout << SDL_GetError() << std::endl;
        exit(1); // TODO: should probably return or throw an error instead
    }

    // setup opengl attributes before window creation
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    // setup SDL window
    window = SDL_CreateWindow(
        Defaults::Window::TITLE,
        Defaults::Window::WIDTH,
        Defaults::Window::HEIGHT,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
    );
    if(window == nullptr) {
        std::cout << "Error: SDL_CreateWindow() failed:" << std::endl;
        std::cout << SDL_GetError() << std::endl;
        exit(1); // TODO: should probably return or throw an error instead
    }

    // set window minimum size
    success = SDL_SetWindowMinimumSize(
        window,
        Defaults::Window::MIN_WIDTH,
        Defaults::Window::MIN_HEIGHT
    );
    if(!success) {
        std::cout << "SDL_SetWindowMinimumSize: SDL_Init() failed:" << std::endl;
        std::cout << SDL_GetError() << std::endl;
        exit(1); // TODO: should probably return or throw an error instead
    }

    // setup other sdl options
    SDL_SetWindowMouseGrab(window, true);
    // SDL_HideCursor();
    SDL_WarpMouseInWindow(window, Defaults::Window::WIDTH / 2.0f, Defaults::Window::HEIGHT / 2.0f);

    // setup OpenGL gfx context
    context = SDL_GL_CreateContext(window);
    if (context == nullptr) {
        std::cout << "Error: SDL_GL_CreateContext() failed:" << std::endl;
        std::cout << SDL_GetError() << std::endl;
        exit(1); // TODO: should probably return or throw an error instead
    }

    // setup GLEW
    GLenum err = glewInit();
    if (err != GLEW_OK){
        std::cout << "Error: glewInit() failed:" << std::endl;
        std::cout << glewGetErrorString(err) << std::endl;
        exit(1);
    }

    // // enable OpenGL debug mode
    // glEnable(GL_DEBUG_OUTPUT);
    // glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    // glDebugMessageCallback(openglDebugMessageCallback, nullptr);

    // setup OpenGL states
    glViewport(0, 0, Defaults::Window::WIDTH, Defaults::Window::HEIGHT);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // TODO: make this an option
    // SDL_GL_SetSwapInterval(0); // Disable vsync
}

RenderContext::~RenderContext() {
    if (window)
        SDL_DestroyWindow(window);
    if (context)
        SDL_GL_DestroyContext(context);
    if (SDL_WasInit(SDL_INIT_VIDEO) != 0)
        SDL_Quit();
}

// void RenderContext::printOpenGLInfo() {
//     std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
//     std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
//     std::cout << "GL Version: " << glGetString(GL_VERSION) << std::endl;
//     std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
// }

void RenderContext::clearWindow() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void RenderContext::swapWindowBuffers() {
    SDL_GL_SwapWindow(window);
}