#pragma once

#include "core/types.hpp"
#include "core/input_state.hpp"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace GFX {

constexpr float CAMERA_DEFAULT_SPEED = 10.0f;
constexpr float CAMERA_DEFAULT_NEAR = -1.0f;
constexpr float CAMERA_DEFAULT_FAR = 1.0f;
constexpr glm::vec3 CAMERA_DEFAULT_FRONT = glm::vec3(0.0f, 0.0f, -1.0f);
constexpr glm::vec3 CAMERA_DEFAULT_UP = glm::vec3(0.0f, 1.0f, 0.0f);
constexpr glm::vec3 CAMERA_DEFAULT_POSITION = glm::vec3(0.0f, 0.0f, 1.0f);

struct CameraInput {
    glm::vec2 movement;
    bool resize;
    int resizeWidth;
    int resizeHeight;
};

class Camera {
public:
    Camera();
    void update(const Core::InputState& input);

    glm::vec2 screenToWorld(
        int screenX,
        int screenY,
        int screenWidth,
        int screenHeight
    );

    // void raycast(int mouseX, int mouseY, int screenWidth, int screenHeight);

    const glm::vec3 getPosition() const { return position; };
    const glm::vec2 getAspect() const { return aspect; };
    const glm::mat4x4& getViewMatrix() const { return vMat; };
    const glm::mat4x4& getProjMatrix() const { return pMat; };
    const glm::mat4x4& getViewProjMatrix() const { return vpMat; };

private:
    void _resize(int width, int height);

    float speed;
    glm::vec3 position;
    glm::vec2 aspect;
    glm::mat4x4 vMat;  // view matrix
    glm::mat4x4 pMat;  // projection matrix
    glm::mat4x4 vpMat; // view projection matrix
};

Camera::Camera() {
    position = CAMERA_DEFAULT_POSITION;
    speed = CAMERA_DEFAULT_SPEED;

    // setup view matrix
    vMat = glm::lookAt(position, position + CAMERA_DEFAULT_FRONT, CAMERA_DEFAULT_UP);

    // setup orthographic projection matrix
    _resize(Core::DEFAULT_WINDOW_WIDTH, Core::DEFAULT_WINDOW_HEIGHT);

    // setup view projection matrix
    vpMat = pMat * vMat;
}

void Camera::update(const Core::InputState& input) {
    bool resized = false;
    bool moved = false;
    glm::vec3 direction = {0.0f, 0.0f, 0.0f};

    // resize camera
    if (input.window.resized) {
        _resize(input.window.width, input.window.height);
        resized = true;
    }

    // move camera left
    if (input.keyboard.buttons[SDL_SCANCODE_LEFT]) {
        direction.x += -1.0f;
        moved = true;
    }

    // move camera right
    if (input.keyboard.buttons[SDL_SCANCODE_RIGHT]) {
        direction.x += 1.0f;
        moved = true;
    }

    // move camera up
    if (input.keyboard.buttons[SDL_SCANCODE_UP]) {
        direction.y += 1.0f;
        moved = true;
    }

    // move camera down
    if (input.keyboard.buttons[SDL_SCANCODE_DOWN]) {
        direction.y += -1.0f;
        moved = true;
    }

    // if moved update position and view matrix
    if (moved) {
        position += direction * speed;
        vMat = glm::lookAt(position, position + CAMERA_DEFAULT_FRONT, CAMERA_DEFAULT_UP);
    }

    // update view projection matrix
    if (resized || moved)
        vpMat = pMat * vMat;
}

glm::vec2 Camera::screenToWorld(
        int screenX,
        int screenY,
        int screenWidth,
        int screenHeight
) {

    // convert to Normalized Device Coordinates (NDC)
    float ndcX = (2.0f * (float)screenX) / (float)screenWidth - 1.0f;
    float ndcY = 1.0f - (2.0f * (float)screenY) / (float)screenHeight;

    // convert to clip space
    glm::vec4 clipCoords(ndcX, ndcY, -1.0f, 1.0f);

    // convert to world space
    glm::vec4 worldCoords = glm::inverse(vMat) * glm::inverse(pMat) * clipCoords;

    // convert from homogeneus coordinates
    glm::vec2 worldPos = glm::vec3(worldCoords) / worldCoords.w;
    // worldPos.z = 0.0f;

    return worldPos;
}

// void Camera::raycast(int mouseX, int mouseY, int screenWidth, int screenHeight) {
//     // https://antongerdelan.net/opengl/raycasting.html

//     // convert to Normalized Device Coordinates (NDC)
//     float ndcX = (2.0f * (float)mouseX) / (float)screenWidth - 1.0f;
//     float ndcY = 1.0f - (2.0f * (float)mouseY) / (float)screenHeight;

//     // get clip coordinates
//     glm::vec4 rayClip(ndcX, ndcY, -1.0f, 1.0f);

//     // convert to eye (or camera) coordinates
//     glm::vec4 rayEye = glm::inverse(pMat) * rayClip;
//     rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);

//     // convert to world space
//     glm::vec3 rayWorld = glm::vec3(glm::inverse(vMat) * rayEye);
//     rayWorld = glm::normalize(rayWorld);

//     std::cout << rayWorld.x << ", " 
//               << rayWorld.y << ", "
//               << rayWorld.z << std::endl;
// }

void Camera::_resize(int width, int height) {
    aspect = {(float)width * 0.5f, (float)height * 0.5f};
    float left   = -aspect.x;
    float right  =  aspect.x;
    float bottom = -aspect.y;
    float top    =  aspect.y;
    float near   = CAMERA_DEFAULT_NEAR;
    float far    = CAMERA_DEFAULT_FAR;
    pMat = glm::ortho(left, right, bottom, top, near, far);
}

} // namespace GFX