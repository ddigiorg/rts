// SDLManager.hpp
#pragma once

#include "core/Types.hpp"
#include "graphics/DebugMessageCallback.hpp"

#include <SDL.h>
#include <GL/glew.h>

#include <iostream>

namespace Core {

// TODO: move OpenGL functionality to graphics since that's where it belongs

class SDLManager {
public:
    SDLManager();
    ~SDLManager();
    void resize();
    void clear();
    void swap();
    GameEvents processEvents();

    SDL_Window* getWindow() const { return window; };

private:
    SDL_Window* window = nullptr;
    SDL_GLContext context = nullptr;
};

SDLManager::SDLManager() {

    // initialize SDL
    int error = SDL_Init(SDL_INIT_VIDEO);
    if(error) {
        // TODO: need better error handling and logging
        std::cout << "Error: SDL_Init() failed:" << std::endl;
        std::cout << SDL_GetError() << std::endl;
        exit(1);
    }

    // setup opengl attributes before window creation
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

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

    // // enable OpenGL debug mode
    // glEnable(GL_DEBUG_OUTPUT);
    // glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    // glDebugMessageCallback(debugMessageCallback, nullptr);

    // setup OpenGL states
    glViewport(0, 0, DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

SDLManager::~SDLManager() {
    if (window)
        SDL_DestroyWindow(window);

    if (context)
        SDL_GL_DeleteContext(context);

    if(SDL_WasInit(SDL_INIT_VIDEO) != 0)
        SDL_Quit();
}

// void SDLManager::printGLInfo() {
//     std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
//     std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
//     std::cout << "GL Version: " << glGetString(GL_VERSION) << std::endl;
//     std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
// }

void SDLManager::resize() {
    int width = NULL;
    int height = NULL;
    SDL_GetWindowSize(window, &width, &height);
    glViewport(0, 0, width, height);
}

void SDLManager::clear() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void SDLManager::swap() {
    SDL_GL_SwapWindow(window);
}

GameEvents SDLManager::processEvents() {
    GameEvents result;
    SDL_Event e;
    while(SDL_PollEvent(&e)) {

        if(e.type == SDL_WINDOWEVENT) {
            switch(e.window.event) {
                case SDL_WINDOWEVENT_SIZE_CHANGED: {
                    resize();
                    result.windowResize = true;
                    break;
                }
            }
        }

        if (e.type == SDL_KEYDOWN) {
            if (e.key.keysym.sym == SDLK_ESCAPE)
                result.quit = true;
        }

        if (e.type == SDL_QUIT) {
            result.quit = true;
        }
    }

    return result;
}

} // namespace Core