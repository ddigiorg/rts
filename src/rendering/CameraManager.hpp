// CameraManager.hpp
#pragma once

#include "core/Defaults.hpp"

#include <glm/vec3.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define CAMERA_DEFAULT_NEAR 0.0f
#define CAMERA_DEFAULT_FAR 2.0f
#define CAMERA_DEFAULT_FRONT glm::vec3(0.0f, 0.0f, -1.0f)
#define CAMERA_DEFAULT_UP glm::vec3(0.0f, 1.0f, 0.0f)
#define CAMERA_DEFAULT_POSITION glm::vec3(0.0f, 0.0f, 1.0f)

// TODO: NEEDS UPDATING
class CameraManager {
public:
    CameraManager();

    // void resize();

    const glm::mat4& getViewMatrix() const { return viewMatrix; };
    const glm::mat4& getProjMatrix() const { return projMatrix; };

private:
    glm::vec3 position;
    glm::vec2 size;
    glm::mat4 viewMatrix;
    glm::mat4 projMatrix;
};

CameraManager::CameraManager() {

    // setup orthographic camera
    position = CAMERA_DEFAULT_POSITION;
    size = glm::vec2(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT);

    viewMatrix = glm::lookAt(
        CAMERA_DEFAULT_POSITION,
        CAMERA_DEFAULT_POSITION + CAMERA_DEFAULT_FRONT,
        CAMERA_DEFAULT_UP
    );

    float boundX = (float)DEFAULT_WINDOW_WIDTH / 2.0f;
    float boundY = (float)DEFAULT_WINDOW_HEIGHT / 2.0f;

    projMatrix = glm::ortho(
        -boundX, boundX,
        -boundY, boundY,
        CAMERA_DEFAULT_NEAR,
        CAMERA_DEFAULT_FAR
    );
}

// CameraManager::resize() {

//     float boundX = (float)width / 2.0f;
//     float boundY = (float)height / 2.0f;
//     app->cameraProjMat = glm::ortho(
//         -boundX, boundX,
//         -boundY, boundY,
//         APP_DEFAULT_CAMERA_NEAR,
//         APP_DEFAULT_CAMERA_FAR
//     );
// }