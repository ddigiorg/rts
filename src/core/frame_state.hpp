#pragma once

#include "input/user_input.hpp"

namespace Core {

// TODO: should I rename frame_state.hpp to game_states.hpp?
struct GameStates {
    bool isDebugActive = false;
    bool isSelecting = false;
};

// TODO: need to make game events more robust
struct SelectEvent {
    bool isActive = false;
    float boxBegX = 0.0f; // mouse coords
    float boxBegY = 0.0f; // mouse coords
    float boxEndX = 0.0f; // mouse coords
    float boxEndY = 0.0f; // mouse coords
};

// TODO: need to make game events more robust
struct GameEvents {
    SelectEvent select;
};

struct FrameState {
    float dt = 1.0f;
    Input::UserInput input; // TODO: add inputPrev
    GameStates states;
    GameEvents events;
};

} // namespace Core