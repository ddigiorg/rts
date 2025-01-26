// Application.hpp
#pragma once

#include "core/WindowManager.hpp"
#include "ecs/Manager.hpp"
#include "gameplay/components/Position.hpp"
#include "gameplay/components/Velocity.hpp"
#include "gameplay/systems/Movement.hpp"
#include "gameplay/events/Events.hpp"
#include "rendering/CameraManager.hpp"
#include "rendering/Shader.hpp"
#include "utilities/Timer.hpp"

#include <SDL.h>

#include <iostream>

class Application {
public:
    Application();
    ~Application();
    void printGLInfo();
    void loop();

private:
    void _initializeSDL();
    void _setupComponents();
    void _setupSystems();
    void _setupEvents();
    void _setupEntities();
    void _processInput();

    bool quit = false;

    WindowManager windowManager;
    CameraManager cameraManager;

    // TEMPORARY!!!!!!!!!!!!!!!!!!
    void _setupQuad();
    void _renderQuad();
    GLuint vao;
    GLuint vbo;
    GFX::Shader* shader; // TODO: change gfx to rendering or keep?

    ECS::Manager ecs;

    Timer timer;
};

Application::Application() {
    _initializeSDL();

    _setupQuad();

    _setupComponents();
    _setupSystems();
    _setupEvents();
    _setupEntities();

    ecs.print();

    ecs.triggerEvent<OnCreate>();
}

Application::~Application() {
    ecs.triggerEvent<OnDestroy>();
    SDL_Quit();
}

void Application::printGLInfo() {
    std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "GL Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
}

void Application::loop() {
    std::cout << "looping..." << std::endl;

    
    size_t counter = 0;
    timer.reset();

    while (!quit) {
        _processInput();
        ecs.triggerEvent<OnUpdate>();
        windowManager.clear();
        ecs.triggerEvent<OnRender>();
        _renderQuad();
        windowManager.swap();

        if (counter == 100)
            std::cout << timer.elapsed() << std::endl;
        counter++;
    }
}

void Application::_initializeSDL() {
    int error = SDL_Init(SDL_INIT_VIDEO);
    if(error) {
        // TODO: need better error handling and logging
        std::cout << "Error: SDL_Init() failed:" << std::endl;
        std::cout << SDL_GetError() << std::endl;
        exit(1);
    }
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
}

void Application::_setupComponents() {
    ecs.registerComponent<Position>();
    ecs.registerComponent<Velocity>();
}

void Application::_setupSystems() {
    ecs.registerSystem<Movement>();
}

void Application::_setupEvents() {
    ecs.registerEvent<OnCreate>();
    ecs.registerEvent<OnDestroy>();
    ecs.registerEvent<OnUpdate>();
    ecs.registerEvent<OnRender>();

    ecs.subscribeToEvent<OnUpdate, Movement>();
}

void Application::_setupEntities() {
    for (size_t i = 0; i < 10; i++) {
        ecs.createEntity({ECS::typeof(Position), ECS::typeof(Velocity)});
    }
}

void Application::_setupQuad() {
    shader = new GFX::Shader(
        "data/shaders/quad_vert.glsl",
        "data/shaders/quad_frag.glsl"
    );

    // GLfloat quadVertexPositions[12] = {
    //      0.5f,  0.5f,
    //      0.5f, -0.5f,
    //     -0.5f,  0.5f,

    //      0.5f, -0.5f,
    //     -0.5f, -0.5f,
    //     -0.5f,  0.5f,
    // };

    GLfloat quadVertexPositions[12] = {
         50.0f,  50.0f,
         50.0f, -50.0f,
        -50.0f,  50.0f,

         50.0f, -50.0f,
        -50.0f, -50.0f,
        -50.0f,  50.0f,
    };

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertexPositions), quadVertexPositions, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Application::_renderQuad() {
    glm::mat4 mvp =  cameraManager.getProjMatrix() * cameraManager.getViewMatrix(); // TODO: model matrix?

    shader->use();
    shader->setUniformMatrix4fv("mvp", 1, mvp);

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Application::_processInput() {
    SDL_Event e;
    while(SDL_PollEvent(&e)) {
        if(e.type == SDL_WINDOWEVENT) {
            switch(e.window.event) {
                case SDL_WINDOWEVENT_SIZE_CHANGED: {
                    windowManager.resize();
                    // cameraManager.resize();
                    // world.sendEvent(evAppResize);
                    break;
                }
            }
        }
        if (e.type == SDL_KEYDOWN) {
            if (e.key.keysym.sym == SDLK_ESCAPE)
                quit = true;
        }
        if (e.type == SDL_QUIT) {
            quit = true;
        }
    }
}