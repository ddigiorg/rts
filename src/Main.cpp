// Main.cpp
#include "core/Application.hpp"

int main(int argc, char* argv[]) {
    Application app;
    // app.printGLInfo();
    app.loop();
    std::cout << "success" << std::endl;
    return 0;
}