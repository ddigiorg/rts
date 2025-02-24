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

class UnitManager {
public:
    UnitManager();
    void create(const UnitType type, const Position& pos);
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

    // renderers
    QuadRenderer selectRenderer;
    SpriteRenderer spriteRenderer;

    // unit manager
    ObjectManager units;

    // unit data
    uint32_t count;
    uint32_t capacity;
    std::vector<ObjectID> ids;
    std::vector<UnitType> types;
    std::vector<Position> positions;
    std::vector<Velocity> velocities;
    std::vector<Position> targets;
    std::vector<bool> isMovings;
    std::vector<Scale> hitboxScales;
    std::vector<Scale> spriteScales;
    std::vector<TexCoord> texcoords;

    // selected unit data
    const Color selectColor = Color{1.0f, 1.0f, 1.0f, 1.0f};
    const float selectLineWidth = 2.0f;
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
    types.resize(capacity);
    positions.resize(capacity);
    velocities.resize(capacity);
    targets.resize(capacity);
    isMovings.resize(capacity);
    hitboxScales.resize(capacity);
    spriteScales.resize(capacity);
    texcoords.resize(capacity);

    spriteRenderer.setCapacity(capacity);

    // setup selected unit data
    selectCount = 0;
    selectCapacity = UNIT_DEFAULT_SELECT_CAPACITY;

    selectRenderer.setCapacity(selectCapacity);

    for (uint16_t i = 0; i < selectCapacity; i++)
        selectRenderer.updateColorData(i, 1, &selectColor);
}

void UnitManager::create(const UnitType type, const Position& pos) {
    if (count >= capacity)
        _growCapacity();

    uint32_t idx = count++;
    ObjectID id = units.create(idx);
    const UnitConfig& config = UNIT_CONFIGS[type];

    ids[idx] = id;
    types[idx] = type;
    positions[idx] = Position{pos.x, pos.y, 0.5f}; // TODO: update z buffer
    velocities[idx] = Velocity{0.0f, 0.0f};
    targets[idx] = Position{0.0f, 0.0f, 0.0f};
    isMovings[idx] = false;
    hitboxScales[idx] = config.hitboxScale;
    spriteScales[idx] = config.spriteScale;
    texcoords[idx] = UNIT_CONFIGS[type].idleSprites[0][0];
}

void UnitManager::remove(const ObjectID& removeID) {
    uint32_t removeIdx = units.get(removeID);
    uint32_t lastIdx = count - 1;
    ObjectID lastID = ids[lastIdx];

    // swap last data into removed slot
    if (removeIdx != lastIdx) {
        units.set(lastID, removeIdx);
        
        ids[removeIdx] = ids[lastIdx];
        types[removeIdx] = types[lastIdx];
        positions[removeIdx] = positions[lastIdx];
        velocities[removeIdx] = velocities[lastIdx];
        targets[removeIdx] = targets[lastIdx];
        isMovings[removeIdx] = isMovings[lastIdx];
        hitboxScales[removeIdx] = hitboxScales[lastIdx];
        spriteScales[removeIdx] = spriteScales[lastIdx];
        texcoords[removeIdx] = texcoords[lastIdx];
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
    types.resize(newCapacity);
    positions.resize(newCapacity);
    velocities.resize(newCapacity);
    targets.resize(newCapacity);
    isMovings.resize(newCapacity);
    hitboxScales.resize(newCapacity);
    spriteScales.resize(newCapacity);
    texcoords.resize(newCapacity);

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
        if (detectAABBCollision(boxPos, boxScale, positions[i], spriteScales[i])) {
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
    }
}

void UnitManager::_updatePositions(FrameState& frame) {
    const float dt = frame.dt;

    // integrate velocity into position
    for (uint32_t i = 0; i < count; i++) {
        const auto& velocity = velocities[i];
        auto& position = positions[i];

        position.x += velocity.x * dt;
        position.y += velocity.y * dt;
    }
}

void UnitManager::_updateCollisions() {

    // TODO: implement spatial partitioning
    // resolve collisions
    for (uint32_t i = 0; i < count; i++) {
        for (uint32_t j = i + 1; j < count; j++) {

            // smaller collision radii when moving
            // float pctI = (isMovings[i]) ? 1.0f : 0.8f;
            // float pctJ = (isMovings[j]) ? 1.0f : 0.8f;

            float radiusI = hitboxScales[i].x * 0.5f;
            float radiusJ = hitboxScales[i].x * 0.5f;
            resolveCircleCollision(positions[i], radiusI, positions[j], radiusJ);
        }
    }
}

void UnitManager::_updateTexCoords() {
    constexpr const float RAD_TO_DEG = 180.0f / 3.14159265358979323846f;
    constexpr const float ANGLE_OFFSET = 22.5f;
    constexpr const float INV_ANGLE_STEP = 8.0f / 360.0f;

    for (uint32_t i = 0; i < count; i++) {
        const auto& isMoving = isMovings[i];
        const auto& type = types[i];
        const auto& velocity = velocities[i];
        auto& texcoord = texcoords[i];

        // if stationary keep the current texcoord
        if (!isMoving) continue;

        // compute angle
        float angle = std::atan2(velocity.y, velocity.x) * RAD_TO_DEG;
        angle = (angle < 0.0f) ? (angle + 360.0f) : angle; // normalize to (0, 360)

        // compute sprite facing in the range [0, 7]
        size_t facing = static_cast<int>((angle + ANGLE_OFFSET) * INV_ANGLE_STEP) & 7;
        size_t frame = 0;

        // directly access the lookup table
        texcoord = UNIT_CONFIGS[type].idleSprites[frame][facing];
    }
}

void UnitManager::_updateRendering() {

    // update select
    for (uint32_t i = 0; i < selectCount; i++) {
        ObjectID& id = selectIDs[i];
        uint32_t idx = units.get(id);
        selectRenderer.updatePositionData(i, 1, &positions[idx]);
        selectRenderer.updateScaleData(i, 1, &spriteScales[idx]);
    }
    selectRenderer.setCount(selectCount);

    // update sprites
    spriteRenderer.updatePositionData(0, count, &positions[0]);
    spriteRenderer.updateScaleData(0, count, &spriteScales[0]);
    spriteRenderer.updateTexCoordData(0, count, &texcoords[0]);
    spriteRenderer.setCount(count);
}

} // namespace Core