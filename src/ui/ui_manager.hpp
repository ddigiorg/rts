#pragma once

#include "common/types.hpp"
#include "graphics/camera.hpp"
#include "input/control/player_camera_controller.hpp"
#include "input/control/screen_camera_controller.hpp"
#include "input/control/debug_overlay_controller.hpp"
#include "ui/cursor.hpp"
#include "ui/debug_overlay.hpp"

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
    Camera mouseCamera; // TODO: Do I really need 3 cameras?  the sdl mouse camera is making things annoying

    PlayerCameraController playerCameraController;
    ScreenCameraController screenCameraController;
    ScreenCameraController mouseCameraController; // TODO:
    DebugOverlayController debugOverlayController;

    Cursor cursor;
    DebugOverlay debugOverlay;
};

UIManager::UIManager()
    : playerCamera(Camera::Type::Orthographic, Camera::Origin::Centered),
      screenCamera(Camera::Type::Orthographic, Camera::Origin::BottomLeft),
      mouseCamera(Camera::Type::Orthographic, Camera::Origin::TopLeft)
{}

void UIManager::update(FrameState& frame) {
    playerCameraController.update(playerCamera, frame);
    screenCameraController.update(screenCamera, frame);
    mouseCameraController.update(mouseCamera, frame);
    debugOverlayController.update(debugOverlay, frame);
    cursor.update(frame);
    debugOverlay.update(playerCamera, frame);
}

void UIManager::render() {
    cursor.render(mouseCamera);
    debugOverlay.render(screenCamera);
}
