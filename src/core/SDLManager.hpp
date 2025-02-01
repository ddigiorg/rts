// SDLManager.hpp
#pragma once

#include "core/Types.hpp"
#include "graphics/DebugMessageCallback.hpp"

#define SDL_MAIN_HANDLED  // Prevent SDL from redefining main()
#include <SDL.h>
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
    void SDLManager::processEvents(EventState& events);

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

void SDLManager::clear() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void SDLManager::swap() {
    SDL_GL_SwapWindow(window);
}

void SDLManager::processEvents(EventState& events) {
    SDL_Event event;

    // reset per-frame flags
    events.window.resized = false;
    events.mouse.moved = false;
    events.mouse.leftReleased = false;
    events.mouse.rightReleased = false;
    events.mouse.middleReleased = false;
    events.keyboard.tildeReleased = false;

    // poll for SDL events
    while(SDL_PollEvent(&event)) {

        // handle mouse motion event
        if (event.type == SDL_MOUSEMOTION) {
            events.mouse.posScreenX = event.motion.x;
            events.mouse.posScreenY = event.motion.y;
            events.mouse.moved = true;
        }

        // handle mouse down event
        if (event.type == SDL_MOUSEBUTTONDOWN) {
            if (event.button.button == SDL_BUTTON_LEFT) {
                events.mouse.leftPressed = true;
            }
            if (event.button.button == SDL_BUTTON_RIGHT) {
                events.mouse.rightPressed = true;
            }
            if (event.button.button == SDL_BUTTON_MIDDLE) {
                events.mouse.middlePressed = true;
            }       
        }

        // handle mouse up event
        if (event.type == SDL_MOUSEBUTTONUP) {
            if (event.button.button == SDL_BUTTON_LEFT) {
                events.mouse.leftPressed = false;
                events.mouse.leftReleased = true;
            }
            if (event.button.button == SDL_BUTTON_RIGHT) {
                events.mouse.rightPressed = false;
                events.mouse.rightReleased = true;
            }
            if (event.button.button == SDL_BUTTON_MIDDLE) {
                events.mouse.middlePressed = false;
                events.mouse.middleReleased = true;
            }
        }

        // key down events
        if (event.type == SDL_KEYDOWN) {
            if (event.key.keysym.sym == SDLK_ESCAPE)
                events.quit = true;
            if (event.key.keysym.sym == SDLK_BACKQUOTE)
                events.keyboard.tildePressed = true;
        }

        // key up events
        if (event.type == SDL_KEYUP) {
            if (event.key.keysym.sym == SDLK_BACKQUOTE)
                events.keyboard.tildePressed = false;
                events.keyboard.tildeReleased = true;
        }

        // window events
        if(event.type == SDL_WINDOWEVENT) {
            if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                SDL_GetWindowSize(window, &events.window.width, &events.window.height);
                glViewport(0, 0, events.window.width, events.window.height);
                events.window.resized = true;
            }
        }

        // quit event
        if (event.type == SDL_QUIT) {
            events.quit = true;
        }
    }
}

} // namespace Core