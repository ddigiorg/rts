#include "application.hpp"

int main(int argc, char* argv[]) {
    Application app;
    app.loop();
    std::cout << "success" << std::endl;
    return 0;
}