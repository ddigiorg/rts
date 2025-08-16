#pragma once

#include "common/types.hpp"
#include "graphics/camera.hpp"
#include "graphics/render_context.hpp"
#include "input/input_manager.hpp"
#include "ui/ui_manager.hpp"

class Application {
public:
    Application();

    void handleInput();
    void handleUpdate();
    void handleRenderPre();
    void handleRender();
    void handleRenderPost();
    void handleQuit();

    bool isRunning() const { return running; };
    const Camera& getPlayerCamera() const { return uiManager.getPlayerCamera(); };

private:
    bool running = true;

    FrameState frame;

    RenderContext renderContext;
    InputManager inputManager;
    UIManager uiManager;
};

Application::Application() {
    uiManager.toggleDebugOverlay();
}

void Application::handleInput() {
    frame.input = inputManager.processEvents();
}

void Application::handleUpdate() {
    uiManager.update(frame);
}

void Application::handleRenderPre() {
    renderContext.clearWindow();
}

void Application::handleRender() {
    uiManager.render();
}

void Application::handleRenderPost() {
    renderContext.swapWindowBuffers();
}

void Application::handleQuit() {
    running = !frame.input.quit;
    if(frame.input.keyboard.buttons[SDL_SCANCODE_ESCAPE])
        running = false;
}