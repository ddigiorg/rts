#include "core/application.hpp"

int main() {
    Application app;

    while (app.isRunning()) {
        app.handleInput();
        app.handleUpdate();
        app.handleRenderPre();
        app.handleRender();
        app.handleRenderPost();
        app.handleQuit();
    }

    return 0;
}