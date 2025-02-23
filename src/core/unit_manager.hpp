#pragma once

#include "core/types.hpp"
#include "core/frame_state.hpp"
#include "core/object_manager.hpp"
#include "graphics/camera.hpp"
#include "graphics/render/quad_renderer.hpp"
#include "graphics/render/sprite_renderer.hpp"
#include "input/user_input.hpp"

#include <array>
#include <vector>
#include <cstdint> // for uint32_t
#include <algorithm> // for std::max

using namespace Graphics;
using namespace Input;

namespace Core {

constexpr const uint32_t UNIT_DEFAULT_CAPACITY = 32;
constexpr const uint32_t UNIT_DEFAULT_SELECT_CAPACITY = 64;
constexpr const uint32_t UNIT_MAX_CAPACITY = 1024 * 20; // 20k units

struct UnitConfig {
    SpriteType type = UNIT_CYAN;
    Position position = {0.0f, 0.0f};
    Scale scale = {32.0f, 32.0f};
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
    void _updateTexCoords();
    void _updateRendering();

    // constants
    const Color selectColor = Color{1.0f, 1.0f, 1.0f, 1.0f};
    const float selectLineWidth = 2.0f;

    // renderers
    QuadRenderer selectRenderer;
    SpriteRenderer spriteRenderer;

    // unit manager
    ObjectManager units;

    // unit data
    uint32_t count;
    uint32_t capacity;
    std::vector<ObjectID> ids;
    std::vector<size_t> spriteOffsets;
    std::vector<bool> isMovings;
    std::vector<Position> positions;
    std::vector<Scale> scales;
    std::vector<TexCoord> texcoords;
    std::vector<Position> targets;
    std::vector<Velocity> velocities;
    std::vector<CircleCollider> colliders;

    // selected unit data
    uint32_t selectCount;
    uint32_t selectCapacity;
    std::array<ObjectID, UNIT_DEFAULT_SELECT_CAPACITY> selectIDs;

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
    spriteOffsets.resize(capacity);
    isMovings.resize(capacity);
    positions.resize(capacity);
    scales.resize(capacity);
    texcoords.resize(capacity);
    targets.resize(capacity);
    velocities.resize(capacity);
    colliders.resize(capacity);
    spriteRenderer.setCapacity(capacity);

    // setup selected unit data
    selectCount = 0;
    selectCapacity = UNIT_DEFAULT_SELECT_CAPACITY;
    selectRenderer.setCapacity(selectCapacity);

    for (uint16_t i = 0; i < selectCapacity; i++)
        selectRenderer.updateColorData(i, 1, &selectColor);
}

void UnitManager::create(const UnitConfig& config) {
    if (count >= capacity)
        _growCapacity();

    uint32_t idx = count++;
    ObjectID id = units.create(idx);

    // initialize unit data
    const size_t offset = SPRITE_OFFSETS[config.type];

    ids[idx] = id;
    spriteOffsets[idx] = offset;
    isMovings[idx] = false;
    positions[idx] = Position{config.position.x, config.position.y, 0.5f}; // TODO: update z buffer
    scales[idx] = config.scale;
    texcoords[idx] = SPRITE_TEXCOORDS[offset];
    targets[idx] = Position{0.0f, 0.0f, 0.0f};
    velocities[idx] = Velocity{0.0f, 0.0f};
    colliders[idx] = CircleCollider{config.position.x, config.position.y, config.scale.x * 0.5f};
}

void UnitManager::remove(const ObjectID& removeID) {
    uint32_t removeIdx = units.get(removeID);
    uint32_t lastIdx = count - 1;
    ObjectID lastID = ids[lastIdx];

    // swap last data into removed slot
    if (removeIdx != lastIdx) {
        units.set(lastID, removeIdx);
        
        ids[removeIdx] = ids[lastIdx];
        spriteOffsets[removeIdx] = spriteOffsets[lastIdx];
        isMovings[removeIdx] = isMovings[lastIdx];
        positions[removeIdx] = positions[lastIdx];
        scales[removeIdx] = scales[lastIdx];
        texcoords[removeIdx] = texcoords[lastIdx];
        targets[removeIdx] = targets[lastIdx];
        velocities[removeIdx] = velocities[lastIdx];
        colliders[removeIdx] = colliders[lastIdx];
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
    _updateTexCoords();
    _updateRendering();
}

void UnitManager::render(const Camera& camera) {
    selectRenderer.renderOutline(camera, selectLineWidth);
    spriteRenderer.render(camera);
}

void UnitManager::_growCapacity() {
    uint32_t newCapacity = std::min(capacity * 2, UNIT_MAX_CAPACITY);

    ids.resize(newCapacity);
    spriteOffsets.resize(newCapacity);
    isMovings.resize(newCapacity);
    positions.resize(newCapacity);
    scales.resize(newCapacity);
    texcoords.resize(newCapacity);
    targets.resize(newCapacity);
    velocities.resize(newCapacity);
    colliders.resize(newCapacity);
    spriteRenderer.setCapacity(newCapacity);

    capacity = newCapacity;
}

void UnitManager::_handleDeletion(FrameState& frame) {
    KeyboardInput& keyboard = frame.input.keyboard;
    bool keyDelete = keyboard.buttons[SDL_SCANCODE_DELETE];

    if (keyDelete && !keyDeletePrev) {
        for (uint32_t i = 0; i < selectCount; i++) {
            remove(selectIDs[i]);
        }
        selectCount = 0;
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
    glm::vec2 pos = glm::min(begWorldPos, endWorldPos);
    glm::vec2 scale = glm::abs(endWorldPos - begWorldPos);
    Position boxPos{pos.x, pos.y};
    Scale boxScale{scale.x, scale.y};

    // reset selected count
    selectCount = 0;

    // compute unit collisions with selection box
    for (uint32_t i = 0; i < count; i++) {
        if (detectAABBCollision(boxPos, boxScale, positions[i], scales[i])) {
            if (selectCount < selectCapacity)
                selectIDs[selectCount++] = ids[i];
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

    if((mouseRight && !mouseRightPrev) || selectCount == 0) {
        glm::vec2 target = camera.screenToWorld(mouse.x, mouse.y, window.width, window.height);

        for (uint32_t i = 0; i < selectCount; i++) {
            ObjectID& id = selectIDs[i];
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
    const float speed = 2.5f;
    const float tolerance = 1.0f; // TODO: need to tune this

    // update unit velocities
    for (uint32_t i = 0; i < count; i++) {
        if (!isMovings[i]) continue;

        const auto& position = positions[i];
        const auto& target = targets[i];
        auto& velocity = velocities[i];

        float dx = target.x - position.x;
        float dy = target.y - position.y;
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

        // TODO: implement smaller collision radii when moving
        // if (isMovings[i])
        //     collider.radius = 14.0f;
        // else
        //     collider.radius = 16.0f;
    }
}

void UnitManager::_updatePositions(FrameState& frame) {
    const float dt = frame.dt;

    // integrate velocity into position
    for (uint32_t i = 0; i < count; i++) {
        const auto& velocity = velocities[i];
        auto& position = positions[i];
        auto& collider = colliders[i];

        position.x += velocity.x * dt;
        position.y += velocity.y * dt;

        // TODO: figure out better way without repeating positions in collider
        collider.x = position.x;
        collider.y = position.y;
    }
}

void UnitManager::_updateCollisions() {

    // TODO: implement spatial partitioning
    // resolve collisions
    for (uint32_t i = 0; i < count; i++) {
        for (uint32_t j = i + 1; j < count; j++) {
            resolveCircleCollision(colliders[i], colliders[j]);

            // TODO: figure out better way without repeating positions
            positions[i].x = colliders[i].x;
            positions[i].y = colliders[i].y;
            positions[j].x = colliders[j].x;
            positions[j].y = colliders[j].y;
        }
    }
}

void UnitManager::_updateTexCoords() {
    constexpr float RAD_TO_DEG = 180.0f / 3.14159265358979323846f;
    constexpr float ANGLE_OFFSET = 22.5f;
    constexpr float INV_ANGLE_STEP = 8.0f / 360.0f;

    for (uint32_t i = 0; i < count; i++) {
        const auto& offset = spriteOffsets[i];
        const auto& velocity = velocities[i];
        auto& texcoord = texcoords[i];

        // if stationary keep the current texcoord
        if (velocity.x == 0.0f && velocity.y == 0.0f) {
            continue;
        }

        // compute angle
        float angle = std::atan2(velocity.y, velocity.x) * RAD_TO_DEG;
        angle = (angle < 0.0f) ? (angle + 360.0f) : angle; // normalize to (0, 360)

        // compute index in the range [0, 7] without branching
        int index = static_cast<int>((angle + ANGLE_OFFSET) * INV_ANGLE_STEP) & 7;

        // directly access the lookup table
        texcoord = SPRITE_TEXCOORDS[offset + index];
    }
}

void UnitManager::_updateRendering() {

    // update select
    for (uint32_t i = 0; i < selectCount; i++) {
        ObjectID& id = selectIDs[i];
        uint32_t idx = units.get(id);
        selectRenderer.updatePositionData(i, 1, &positions[idx]);
        selectRenderer.updateScaleData(i, 1, &scales[idx]);
    }
    selectRenderer.setCount(selectCount);

    // update sprites
    spriteRenderer.updatePositionData(0, count, &positions[0]);
    spriteRenderer.updateScaleData(0, count, &scales[0]);
    spriteRenderer.updateTexCoordData(0, count, &texcoords[0]);
    spriteRenderer.setCount(count);
}

} // namespace Core