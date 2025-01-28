// CameraManager.hpp
#pragma once

#include "core/Types.hpp"

#include <glm/vec3.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define CAMERA_DEFAULT_NEAR 0.0f
#define CAMERA_DEFAULT_FAR 2.0f
#define CAMERA_DEFAULT_FRONT glm::vec3(0.0f, 0.0f, -1.0f)
#define CAMERA_DEFAULT_UP glm::vec3(0.0f, 1.0f, 0.0f)
#define CAMERA_DEFAULT_POSITION glm::vec3(0.0f, 0.0f, 1.0f)

namespace GFX {

// TODO: NEEDS UPDATING
class CameraManager {
public:
    CameraManager();

    // void resize();
    void update();

    const glm::mat4& getViewMatrix() const { return vMat; };
    const glm::mat4& getProjMatrix() const { return pMat; };
    const glm::mat4& getViewProjMatrix() const { return vpMat; };

private:
    glm::vec3 position;
    glm::vec2 size;
    glm::mat4 vMat;  // view matrix
    glm::mat4 pMat;  // projection matrix
    glm::mat4 vpMat; // view projection matrix
};

CameraManager::CameraManager() {

    // setup orthographic camera
    position = CAMERA_DEFAULT_POSITION;
    size = glm::vec2(Core::DEFAULT_WINDOW_WIDTH, Core::DEFAULT_WINDOW_HEIGHT);

    vMat = glm::lookAt(
        CAMERA_DEFAULT_POSITION,
        CAMERA_DEFAULT_POSITION + CAMERA_DEFAULT_FRONT,
        CAMERA_DEFAULT_UP
    );

    float boundX = (float)Core::DEFAULT_WINDOW_WIDTH / 2.0f;
    float boundY = (float)Core::DEFAULT_WINDOW_HEIGHT / 2.0f;

    pMat = glm::ortho(
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

void CameraManager::update() {
    vpMat = pMat * vMat;
}

} // namespace GFX