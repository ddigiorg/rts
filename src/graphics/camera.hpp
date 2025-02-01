#pragma once

#include "core/types.hpp"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace GFX {

class Camera {
public:
    Camera();
    void handleEvents(const Core::EventState& events);
    void resize(int width, int height);
    void update();

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
    static const float DEFAULT_NEAR;
    static const float DEFAULT_FAR;
    static const glm::vec3 DEFAULT_FRONT;
    static const glm::vec3 DEFAULT_UP;
    static const glm::vec3 DEFAULT_POSITION;
    glm::vec3 position;
    glm::vec2 aspect;
    glm::mat4x4 vMat;  // view matrix
    glm::mat4x4 pMat;  // projection matrix
    glm::mat4x4 vpMat; // view projection matrix
};

const float Camera::DEFAULT_NEAR = -1.0f;
const float Camera::DEFAULT_FAR = 1.0f;
const glm::vec3 Camera::DEFAULT_FRONT = glm::vec3(0.0f, 0.0f, -1.0f);
const glm::vec3 Camera::DEFAULT_UP = glm::vec3(0.0f, 1.0f, 0.0f);
const glm::vec3 Camera::DEFAULT_POSITION = glm::vec3(0.0f, 0.0f, 1.0f);

Camera::Camera() {
    position = DEFAULT_POSITION;

    // setup view matrix
    vMat = glm::lookAt(position, position + DEFAULT_FRONT, DEFAULT_UP);

    // setup orthographic projection matrix
    resize(Core::DEFAULT_WINDOW_WIDTH, Core::DEFAULT_WINDOW_HEIGHT);
}

void Camera::handleEvents(const Core::EventState& events) {
    if (events.window.resized) {
        resize(events.window.width, events.window.height);
    }
}

void Camera::resize(int width, int height) {
    aspect = {(float)width * 0.5f, (float)height * 0.5f};
    float left   = -aspect.x;
    float right  =  aspect.x;
    float bottom = -aspect.y;
    float top    =  aspect.y;
    float near   = DEFAULT_NEAR;
    float far    = DEFAULT_FAR;
    pMat = glm::ortho(left, right, bottom, top, near, far);
}

void Camera::update() {
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

} // namespace GFX