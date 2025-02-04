#pragma once

#include "core/types.hpp"
#include "core/input_state.hpp"
#include "graphics/opengl_debug.hpp"

#include <SDL3/SDL.h>
#include <GL/glew.h>

#include <iostream>

namespace Core {

// TODO: move any OpenGL functionality to graphics since that's where it belongs?

class SDLManager {
public:
    SDLManager();
    ~SDLManager();
    void clear();
    void swap();
    const InputState& SDLManager::processEvents();

    int getWindowWidth() const { return input.window.width; };
    int getWindowHeight() const { return input.window.height; };

private:
    SDL_Window* window = nullptr;
    SDL_GLContext context = nullptr;

    InputState input;
};

SDLManager::SDLManager() {
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
        DEFAULT_WINDOW_TITLE,
        DEFAULT_WINDOW_WIDTH,
        DEFAULT_WINDOW_HEIGHT,
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
        DEFAULT_MIN_WINDOW_WIDTH,
        DEFAULT_MIN_WINDOW_HEIGHT
    );
    if(!success) {
        std::cout << "SDL_SetWindowMinimumSize: SDL_Init() failed:" << std::endl;
        std::cout << SDL_GetError() << std::endl;
        exit(1); // TODO: should probably return or throw an error instead
    }

    // setup OpenGL graphics context
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
    glViewport(0, 0, DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // setup input
    input.window.width = DEFAULT_WINDOW_WIDTH;
    input.window.height = DEFAULT_WINDOW_HEIGHT;
    input.keyboard.buttons = SDL_GetKeyboardState(&input.keyboard.numkeys);
}

SDLManager::~SDLManager() {
    if (window)
        SDL_DestroyWindow(window);
    if (context)
        SDL_GL_DestroyContext(context);
    if (SDL_WasInit(SDL_INIT_VIDEO) != 0)
        SDL_Quit();
}

// void SDLManager::printOpenGLInfo() {
//     std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
//     std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
//     std::cout << "GL Version: " << glGetString(GL_VERSION) << std::endl;
//     std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
// }

void SDLManager::clear() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void SDLManager::swap() {
    SDL_GL_SwapWindow(window);
}

const InputState& SDLManager::processEvents() {
    SDL_Event event;

    // reset flags
    input.window.resized = false;
    input.mouse.moved = false;
    input.mouse.wheel.moved = false;

    // poll for SDL events
    while(SDL_PollEvent(&event)) {

        // mouse motion
        if (event.type == SDL_EVENT_MOUSE_MOTION) {
            input.mouse.moved = true;
            input.mouse.x = event.motion.x;
            input.mouse.y = event.motion.y;
        }

        // mouse wheel
        if (event.type == SDL_EVENT_MOUSE_WHEEL) {
            input.mouse.wheel.moved = true;
            input.mouse.wheel.x = event.wheel.x;
            input.mouse.wheel.y = event.wheel.y;
        }

        // window resized
        if(event.type == SDL_EVENT_WINDOW_RESIZED) {
            input.window.resized = true;
            input.window.width = event.window.data1;
            input.window.height = event.window.data2;
            glViewport(0, 0, input.window.width, input.window.height);
        }

        // quit
        if (event.type == SDL_EVENT_QUIT) {
            input.quit = true;
        }
    }

    // update mouse states after polling events
    input.mouse.buttons = SDL_GetMouseState(nullptr, nullptr);

    return input;
}

} // namespace Core