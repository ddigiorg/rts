#include "engine/core/sdl_manager.hpp"
#include "engine/utilities/timer.hpp"

using namespace Core;

int main() {

    // TODO: REVIEW THIS!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // - https://gameprogrammingpatterns.com/game-loop.html
    // - https://gafferongames.com/post/fix_your_timestep/
    // - https://docs.unity3d.com/Manual/ExecutionOrder.html
    // - https://www.gameludere.com/2019/11/01/game-loop/#4_Fixed_update_time_and_variable_rendering
    //
    // // Note: game time is in milliseconds
    // uint64_t tSim = 0; // ms
    // uint64_t tPrevious = SDL_GetTicks64();
    // uint64_t tLag = 0;
    // const uint64_t MS_PER_UPDATE = 16;
    // while (!quit) {
    //     uint64_t tCurrent = SDL_GetTicks64(); 
    //     uint64_t tFrame = tCurrent - tPrevious;
    //     tPrevious = tCurrent;
    //     tLag = tFrame;
    //     _processInput();
    //     // game simulates at constant rate
    //     while (tLag >= MS_PER_UPDATE) {
    //         _update(tSim, MS_PER_UPDATE);
    //         tLag -= MS_PER_UPDATE;
    //         tSim += MS_PER_UPDATE;
    //     }
    //     _render(); 
    //     fps = (int)(1.0 / ((float)tFrame / 1000.0f));
    // }

    // setup sdl
    SDLManager sdl;

    // setup loop variables
    bool quit = false;
    float dt = 1.0f;

    // loop
    while (!quit) {

        // handle inputs
        UserInput input = sdl.processEvents();

        // handle updates

        // handle renders
        sdl.clear();
        sdl.swap();

        // handle quit
        quit = input.quit;
        if(input.keyboard.buttons[SDL_SCANCODE_ESCAPE])
            quit = true;
    }

    return 0;
}