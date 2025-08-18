#pragma once

#include "common/types.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// https://learnopengl.com/Getting-started/Camera

constexpr glm::vec3 CAMERA_WORLD_UP = glm::vec3(0.0f, 1.0f, 0.0f);

struct Frustum {
    float l = -1.0f; // left
    float r =  1.0f; // right
    float b = -1.0f; // bottom
    float t =  1.0f; // top
    float n = -1.0f; // near
    float f =  1.0f; // far
};

class Camera {
public:
    enum class Type {
        Orthographic,
        Perspective
    };

    enum class Origin {
        Centered,   // Coordinates relative to a centered origin.
        BottomLeft, // Coordinates relative to bottom-left corner (e.g. OpenGL-style coordinates).
        TopLeft,    // Coordinates relative to top-left corner (e.g. SDL-style coordinates).
    };

public:
    Camera(
        Type type = Type::Orthographic,
        Origin origin = Origin::BottomLeft,
        float width  = (float)WINDOW_DEFAULT_SIZE_X,
        float height = (float)WINDOW_DEFAULT_SIZE_Y,
        float speed = 10.0f,
        glm::vec3 position = glm::vec3(0.0f, 0.0f, 1.0f),
        glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f)
    );

    // movement functions
    void translate(const glm::vec3& translation);
    void moveForward(float dt);
    void moveBackward(float dt);
    void moveLeft(float dt);
    void moveRight(float dt);
    void moveUp(float dt);
    void moveDown(float dt);

    // rotation functions
    // void rotate(float angle, const glm::vec3& axis);
    // void lookAt(const glm::vec3& target);
    // void yaw(float angle);
    // void pitch(float angle);
    // void roll(float angle);

    // edit frustum functions
    void resize(const glm::vec2& size);
    void zoom(float factor);

    // matrix update functions
    void updateVectors();
    void updateViewMatrix();
    void updateProjMatrix();

    // raycasting
    glm::vec2 screenToWorld(
        float screenX, float screenY,
        float screenW, float screenH
    ) const;

    // getters
    const float getZoom() const { return zoomScale; };
    const glm::vec2 getSize() const { return size; };
    const glm::vec3 getPosition() const { return position; };
    const glm::mat4 getViewMatrix() const { return view; };
    const glm::mat4 getProjMatrix() const { return proj; };
    const glm::mat4 getViewProjMatrix() const { return proj * view; };
    const Frustum getFrustum() const { return frustum; };

private:
    Type type = Type::Orthographic;
    Origin origin = Origin::BottomLeft;
    Frustum frustum = {};

    float speed = 10.0f;
    float zoomSensitivity = 0.1f;

    float zoomScale = 1.0f;

    glm::vec2 size = {2.0f, 2.0f};
    glm::vec3 position = {0.0f, 0.0f, 1.0f};
    glm::vec3 front = {0.0f, 0.0f, -1.0f};
    glm::vec3 right = {1.0f, 0.0f, 0.0f};
    glm::vec3 up = {0.0f, 1.0f, 0.0f};

    glm::mat4 view;
    glm::mat4 proj;
};

Camera::Camera(
        Type type,
        Origin origin,
        float width,
        float height,
        float speed,
        glm::vec3 position,
        glm::vec3 front
) {
    this->type = type;
    this->origin = origin;
    this->size = {width, height};
    this->speed = speed;
    this->position = position;
    this->front = glm::normalize(front);
   
    updateVectors();
    updateViewMatrix();
    resize(size);
}

void Camera::translate(const glm::vec3& translation) {
    position += translation;
    updateViewMatrix();
}

void Camera::moveForward(float dt) {
    position += front * speed * dt;
    updateViewMatrix();
}

void Camera::moveBackward(float dt) {
    position -= front * speed * dt;
    updateViewMatrix();
}

void Camera::moveLeft(float dt) {
    position -= right * speed * dt;
    updateViewMatrix();
}

void Camera::moveRight(float dt) {
    position += right * speed * dt;
    updateViewMatrix();
}

void Camera::moveUp(float dt) {
    position += up * speed * dt;
    updateViewMatrix();
}

void Camera::moveDown(float dt) {
    position -= up * speed * dt;
    updateViewMatrix();
}

void Camera::resize(const glm::vec2& size) {
    this->size = size;

    if (origin == Origin::Centered) {
        float aspectX = size.x * 0.5f;
        float aspectY = size.y * 0.5f;
        frustum.l = -aspectX;
        frustum.r =  aspectX;
        frustum.b = -aspectY;
        frustum.t =  aspectY;
    }
    else if (origin == Origin::BottomLeft) {
        frustum.l = 0.0f;
        frustum.r = size.x;
        frustum.b = 0.0f;
        frustum.t = size.y;
    }
    else if (origin == Origin::TopLeft) {
        frustum.l = 0.0f;
        frustum.r = size.x;
        frustum.b = size.y;
        frustum.t = 0.0f;
    }

    updateProjMatrix();
}

void Camera::zoom(float factor) {
    if (factor == 0.0f)
        return;

    float scale = std::exp(factor * zoomSensitivity);
    zoomScale *= scale;

    if (type == Type::Orthographic) {
        frustum.l *= scale;
        frustum.r *= scale;
        frustum.b *= scale;
        frustum.t *= scale;
        size.x = frustum.r - frustum.l;
        size.y = frustum.t - frustum.b;
    }
    else if (type == Type::Perspective) {
        // TODO: implement
    }

    updateProjMatrix();
}

void Camera::updateVectors() {
    right = glm::normalize(glm::cross(front, CAMERA_WORLD_UP));
    up = glm::normalize(glm::cross(right, front));
}

void Camera::updateViewMatrix() {
    view = glm::lookAt(position, position + front, up);
}

void Camera::updateProjMatrix() {
    if (type == Type::Orthographic) {
        proj = glm::ortho(
            frustum.l, frustum.r,
            frustum.b, frustum.t,
            frustum.n, frustum.f
        );
    }
    else if (type == Type::Perspective) {
        // TODO: implement
    }
}

glm::vec2 Camera::screenToWorld(
        float screenX, float screenY,
        float screenW, float screenH
) const {

    // convert to Normalized Device Coordinates (NDC)
    float ndcX = (2.0f * screenX) / screenW - 1.0f;
    float ndcY = 1.0f - (2.0f * screenY) / screenH;

    // convert to clip space
    glm::vec4 clip(ndcX, ndcY, -1.0f, 1.0f);

    // convert to world space
    glm::vec4 world = glm::inverse(view) * glm::inverse(proj) * clip;

    // convert from homogeneus coordinates
    glm::vec2 worldPos = glm::vec3(world) / world.w;
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
