#pragma once

#include "core/frame_state.hpp"
#include "graphics/camera.hpp"
#include "input/control/player_camera_controller.hpp"
#include "input/control/screen_camera_controller.hpp"
#include "input/control/debug_overlay_controller.hpp"
#include "ui/cursor.hpp"
#include "ui/debug_overlay.hpp"

using namespace Core;
using namespace Graphics;

namespace UI {

class UIManager {
public:
    UIManager();
    void toggleDebugOverlay() { debugOverlay.toggleActive(); };
    void update(FrameState& frame);
    void render();

    const Camera& getPlayerCamera() const { return playerCamera; };

private:
    Camera playerCamera;
    Camera screenCamera;

    PlayerCameraController playerCameraController;
    ScreenCameraController screenCameraController;
    DebugOverlayController debugOverlayController;

    Cursor cursor;
    DebugOverlay debugOverlay;
};

UIManager::UIManager()
    : playerCamera(Camera::Type::Orthographic, Camera::Origin::Centered),
      screenCamera(Camera::Type::Orthographic, Camera::Origin::BottomLeft)
{}

void UIManager::update(FrameState& frame) {
    playerCameraController.update(playerCamera, frame);
    screenCameraController.update(screenCamera, frame);
    debugOverlayController.update(debugOverlay, frame);
    cursor.update(frame);
    debugOverlay.update(playerCamera, frame);
}

void UIManager::render() {
    cursor.render(screenCamera);
    debugOverlay.render(screenCamera);
}

} // namespace UI