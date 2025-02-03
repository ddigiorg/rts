#pragma once

#include "core/types.hpp"
#include "graphics/debug_message_callback.hpp"

#define SDL_MAIN_HANDLED  // Prevent SDL from redefining main()
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
    const FrameInput& SDLManager::processEvents();

private:
    SDL_Window* window = nullptr;
    SDL_GLContext context = nullptr;

    FrameInput input;
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
        // SDL_GL_DeleteContext(context);
        SDL_GL_DestroyContext(context);

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

const FrameInput& SDLManager::processEvents() {
    SDL_Event event;

    // // reset the mouse button states
    // for (auto& pair : input.mouse.buttons)
    //     pair.second = {false, false};

    // // reset the keyboard button states
    // for (auto& pair : input.keyboard.buttons)
    //     pair.second = {false, false};

    // poll for SDL events
    while(SDL_PollEvent(&event)) {

        // // handle mouse motion event
        // if (event.type == SDL_MOUSEMOTION) {
        //     input.mouse.moved = true;
        //     input.mouse.x = event.motion.x;
        //     input.mouse.y = event.motion.y;
        // }

        // // handle mouse down event
        // if (event.type == SDL_MOUSEBUTTONDOWN) {
        //     Uint8 b = event.button.button;
        //     input.mouse.buttons[b].pressed = true;
        // }

        // // handle mouse mouse up event
        // if (event.type == SDL_MOUSEBUTTONUP) {
        //     Uint8 b = event.button.button;
        //     input.mouse.buttons[b].pressed = false;
        //     input.mouse.buttons[b].released = true;
        // }

        // // key down events
        // if (event.type == SDL_KEYDOWN && event.key.repeat == 0) {
        //     SDL_Keycode b = event.key.keysym.sym;
        //     input.keyboard.buttons[b].pressed = true;
        // }

        // // key up events
        // if (event.type == SDL_KEYUP) {
        //     SDL_Keycode b = event.key.keysym.sym;
        //     input.keyboard.buttons[b].pressed = false;
        //     input.keyboard.buttons[b].released = true;
        // }

        // // window events
        // if(event.type == SDL_WINDOWEVENT) {
        //     if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
        //         SDL_GetWindowSize(window, &input.window.width, &input.window.height);
        //         glViewport(0, 0, input.window.width, input.window.height);
        //         input.window.resized = true;
        //     }
        // }

        // quit event
        // if (event.type == SDL_QUIT) {
        if (event.type == SDL_EVENT_QUIT) {
            input.quit = true;
        }
    }

    // int numkeys = 0;
    // const Uint8* currentKeyStates = SDL_GetKeyboardState(&numkeys);
    // // std::cout << numkeys << std::endl;

    // if(currentKeyStates[SDL_SCANCODE_LSHIFT])
    //     std::cout << "shift" << std::endl;

    // if(currentKeyStates[SDL_SCANCODE_A])
    //     std::cout << "a" << std::endl;
    


    return input;
}

} // namespace Core