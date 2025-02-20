#pragma once

#include "core/types.hpp"
#include "core/frame_state.hpp"
#include "core/object_manager.hpp"
#include "graphics/camera.hpp"
#include "graphics/render/markers_renderer.hpp"
#include "graphics/render/sprites_renderer.hpp"
#include "input/user_input.hpp"

#include <array>
#include <vector>
#include <cstdint> // for uint32_t
#include <algorithm> // for std::max

using namespace Graphics;
using namespace Input;

namespace Core {

constexpr const uint32_t UNIT_DEFAULT_CAPACITY = 32;
constexpr const uint32_t UNIT_DEFAULT_SELECT_CAPACITY = 32;
constexpr const uint32_t UNIT_MAX_CAPACITY = 1024 * 20; // 20k units

struct UnitConfig {
    Position position = {0.0f, 0.0f};
    float scale = 32.0f;
};

class UnitManager {
public:
    UnitManager();
    void create(const UnitConfig& config);
    void remove(const ObjectID& id);
    void update(FrameState& frame, const Camera& camera); // TODO: figure out how to handle player camera better
    void render(const Camera& camera);

private:
    void _growCapacity();
    void _handleDeletion(FrameState& frame);
    void _updateSelected(FrameState& frame, const Camera& camera);
    void _updateTargets(FrameState& frame, const Camera& camera);
    void _updateVelocities(FrameState& frame);
    void _updatePositions(FrameState& frame);
    void _updateCollisions();
    void _updateRendering();

    // renderers
    MarkersRenderer markersRenderer;
    SpritesRenderer spritesRenderer;

    // unit manager
    ObjectManager units;

    // unit data
    uint32_t count;
    uint32_t capacity;
    std::vector<ObjectID> ids;
    std::vector<bool> isMovings;
    std::vector<Transform> transforms;
    std::vector<TexCoord> texcoords;
    std::vector<Position> targets;
    std::vector<Velocity> velocities;
    std::vector<CircleCollider> colliders;

    // selected unit data
    uint32_t selectedCount;
    uint32_t selectedCapacity;
    std::array<ObjectID, UNIT_DEFAULT_SELECT_CAPACITY> selectedIDs;

    // previous frame key presses
    bool mouseRightPrev;
    bool keyDeletePrev;
};

UnitManager::UnitManager() {
    mouseRightPrev = false;
    keyDeletePrev = false;

    // setup unit data
    count = 0;
    capacity = UNIT_DEFAULT_CAPACITY;
    ids.resize(capacity);
    isMovings.resize(capacity);
    transforms.resize(capacity);
    texcoords.resize(capacity);
    targets.resize(capacity);
    velocities.resize(capacity);
    colliders.resize(capacity);
    spritesRenderer.resize(capacity);

    // setup selected unit data
    selectedCount = 0;
    selectedCapacity = UNIT_DEFAULT_SELECT_CAPACITY;
    markersRenderer.resize(selectedCapacity);
}

void UnitManager::create(const UnitConfig& config) {
    if (count >= capacity)
        _growCapacity();

    uint32_t idx = count++;
    ObjectID id = units.create(idx);

    // initialize unit data
    ids[idx]        = id;
    isMovings[idx]  = false;
    transforms[idx] = Transform{config.position.x, config.position.y, config.scale, config.scale};
    texcoords[idx]  = TexCoord{0.0f, 0.5f, (64.0f / 320.0f), (64.0f / 128.0f)};
    targets[idx]    = Position{0.0f, 0.0f};
    velocities[idx] = Velocity{0.0f, 0.0f};
    colliders[idx]  = CircleCollider{config.position.x, config.position.y, config.scale * 0.5f};
}

void UnitManager::remove(const ObjectID& removeID) {
    uint32_t removeIdx = units.get(removeID);
    uint32_t lastIdx = count - 1;
    ObjectID lastID = ids[lastIdx];

    // swap last data into removed slot
    if (removeIdx != lastIdx) {
        units.set(lastID, removeIdx);
        
        ids[removeIdx]        = ids[lastIdx];
        isMovings[removeIdx]  = isMovings[lastIdx];
        transforms[removeIdx] = transforms[lastIdx];
        texcoords[removeIdx]  = texcoords[lastIdx];
        targets[removeIdx]    = targets[lastIdx];
        velocities[removeIdx] = velocities[lastIdx];
        colliders[removeIdx]  = colliders[lastIdx];
    }

    count--;
    units.remove(removeID);
}

void UnitManager::update(FrameState& frame, const Camera& camera) {
    _handleDeletion(frame);
    _updateSelected(frame, camera);
    _updateTargets(frame, camera);
    _updateVelocities(frame);
    _updatePositions(frame);
    _updateCollisions();
    _updateRendering();
}

void UnitManager::render(const Camera& camera) {
    markersRenderer.render(camera);
    spritesRenderer.render(camera);
}

void UnitManager::_growCapacity() {
    uint32_t newCapacity = std::min(capacity * 2, UNIT_MAX_CAPACITY);

    ids.resize(newCapacity);
    isMovings.resize(newCapacity);
    transforms.resize(newCapacity);
    texcoords.resize(newCapacity);
    targets.resize(newCapacity);
    velocities.resize(newCapacity);
    colliders.resize(newCapacity);
    spritesRenderer.resize(newCapacity);

    capacity = newCapacity;
}

void UnitManager::_handleDeletion(FrameState& frame) {
    KeyboardInput& keyboard = frame.input.keyboard;
    bool keyDelete = keyboard.buttons[SDL_SCANCODE_DELETE];

    if (keyDelete && !keyDeletePrev) {
        for (uint32_t i = 0; i < selectedCount; i++) {
            remove(selectedIDs[i]);
        }
        selectedCount = 0;
    }

    keyDeletePrev = keyDelete;
}

void UnitManager::_updateSelected(FrameState& frame, const Camera& camera) {
    SelectEvent& select = frame.events.select;
    if (!select.isActive) return;

    const WindowInput& window = frame.input.window;

    // convert mouse coordinates to world coordinates
    glm::vec2 begWorldPos = camera.screenToWorld(select.boxBegX, select.boxBegY, window.width, window.height);
    glm::vec2 endWorldPos = camera.screenToWorld(select.boxEndX, select.boxEndY, window.width, window.height);

    // compute selection box
    glm::vec2 boxPos = glm::min(begWorldPos, endWorldPos);
    glm::vec2 boxSize = glm::abs(endWorldPos - begWorldPos);
    Transform boxTransform = {boxPos.x, boxPos.y, boxSize.x, boxSize.y};

    // reset selected count
    selectedCount = 0;

    // compute unit collisions with selection box
    for (uint32_t i = 0; i < count; i++) {
        if (detectAABBCollision(boxTransform, transforms[i])) {
            if (selectedCount < selectedCapacity)
                selectedIDs[selectedCount++] = ids[i];
            else
                break;
        }
    }

    select.isActive = false;
}

void UnitManager::_updateTargets(FrameState& frame, const Camera& camera) {
    const MouseInput& mouse = frame.input.mouse;
    const WindowInput& window = frame.input.window;
    bool mouseRight = (mouse.buttons & SDL_BUTTON_RMASK) != 0;

    if((mouseRight && !mouseRightPrev) || selectedCount == 0) {
        glm::vec2 target = camera.screenToWorld(mouse.x, mouse.y, window.width, window.height);

        for (uint32_t i = 0; i < selectedCount; i++) {
            ObjectID& id = selectedIDs[i];
            uint32_t idx = units.get(id);
            isMovings[idx] = true;
            targets[idx] = {target.x, target.y};
        }
    }

    mouseRightPrev = mouseRight;
}

void UnitManager::_updateVelocities(FrameState& frame) {
    // TODO: put these somewhere
    const float dt = frame.dt;
    const float speed = 2.0f;
    const float tolerance = 1.0f;

    // update unit velocities
    for (uint32_t i = 0; i < count; i++) {
        if (!isMovings[i]) continue;

        auto& transform = transforms[i];
        auto& target = targets[i];
        auto& velocity = velocities[i];

        float dx = target.x - transform.x;
        float dy = target.y - transform.y;
        float length = std::sqrt(dx * dx + dy * dy);

        // stop moving when close enough to target
        if (length <= tolerance) {
            velocity = {0.0f, 0.0f};
            isMovings[i] = false;
            continue;
        }

        // normalize direction and scale by speed
        float invLength = 1.0f / length;
        velocity.x = dx * invLength * speed;
        velocity.y = dy * invLength * speed;
    }
}

void UnitManager::_updatePositions(FrameState& frame) {
    const float dt = frame.dt;

    // integrate velocity into position
    for (uint32_t i = 0; i < count; i++) {
        auto& transform = transforms[i];
        auto& velocity = velocities[i];
        auto& collider = colliders[i];

        transform.x += velocity.x * dt;
        transform.y += velocity.y * dt;

        // TODO: figure out better way without repeating positions
        collider.x = transform.x;
        collider.y = transform.y;

        // TODO: implement smaller collision radii when moving
        // if (isMovings[i])
        //     collider.radius = 14.0f;
        // else
        //     collider.radius = 16.0f;
    }
}

void UnitManager::_updateCollisions() {

    // TODO: implement spatial partitioning
    // resolve collisions
    for (uint32_t i = 0; i < count; i++) {
        for (uint32_t j = i + 1; j < count; j++) {
            resolveCircleCollision(colliders[i], colliders[j]);

            // TODO: figure out better way without repeating positions
            transforms[i].x = colliders[i].x;
            transforms[i].y = colliders[i].y;
            transforms[j].x = colliders[j].x;
            transforms[j].y = colliders[j].y;
        }
    }
}

void UnitManager::_updateRendering() {

    // update markers
    markersRenderer.resetCount();
    for (uint32_t i = 0; i < selectedCount; i++) {
        ObjectID& id = selectedIDs[i];
        uint32_t idx = units.get(id);
        markersRenderer.appendSubset(1, &transforms[idx]);
    }

    // update sprites
    spritesRenderer.resetCount();
    spritesRenderer.appendSubset(count, &transforms[0], &texcoords[0]);
}

} // namespace Core