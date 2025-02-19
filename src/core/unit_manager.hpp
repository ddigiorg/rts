#pragma once

#include "core/types.hpp"
#include "core/frame_state.hpp"
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
constexpr const uint32_t UNIT_DEFAULT_SELECT_CAPACITY = 16;

class UnitManager {
public:
    UnitManager();

    void createUnit(const Position& pos);
    void removeUnit(const uint32_t& idx);

    void update(FrameState& frame, const Camera& camera); // TODO: figure out how to handle player camera better
    void render(const Camera& camera);

private:
    void _growUnitCapacity();
    // void _shrinkUnitCapacity();

    void _handleDelete(FrameState& frame);
    void _handleSelect(FrameState& frame, const Camera& camera);
    void _handleMovement(FrameState& frame, const Camera& camera);

    // renderers
    MarkersRenderer markersRenderer;
    SpritesRenderer spritesRenderer;

    // unit data
    uint32_t unitCount;
    uint32_t unitCapacity;
    std::vector<bool> unitIsMovings;
    std::vector<Transform> unitTransforms;
    std::vector<TexCoord> unitTexCoords;
    std::vector<Position> unitTargets;
    std::vector<Velocity> unitVelocities; // TODO: implement this

    // selected unit data
    uint32_t selectedCount;
    uint32_t selectedCapacity;
    std::array<uint32_t, UNIT_DEFAULT_SELECT_CAPACITY> selectedIndices;

    bool mouseRightPrev;
    bool keyDeletePrev;
};

UnitManager::UnitManager() {
    mouseRightPrev = false;
    keyDeletePrev = false;

    // setup unit data
    unitCount = 0;
    unitCapacity = UNIT_DEFAULT_CAPACITY;
    unitIsMovings.resize(unitCapacity);
    unitTransforms.resize(unitCapacity);
    unitTexCoords.resize(unitCapacity);
    unitTargets.resize(unitCapacity);
    unitVelocities.resize(unitCapacity);
    spritesRenderer.resize(unitCapacity);

    // setup selected unit data
    selectedCount = 0;
    selectedCapacity = UNIT_DEFAULT_SELECT_CAPACITY;
    markersRenderer.resize(selectedCapacity);
}

void UnitManager::_growUnitCapacity() {
    uint32_t newCapacity = unitCapacity * 2;
    unitIsMovings.resize(newCapacity);
    unitTransforms.resize(newCapacity);
    unitTexCoords.resize(newCapacity);
    unitTargets.resize(newCapacity);
    unitVelocities.resize(unitCapacity);
    spritesRenderer.resize(newCapacity);
    unitCapacity = newCapacity;
}

void UnitManager::createUnit(const Position& pos) {
    if (unitCount >= unitCapacity)
        _growUnitCapacity();

    uint32_t idx = unitCount++;
    unitIsMovings[idx] = false;
    unitTransforms[idx] = Transform{pos.x, pos.y, 32.0f, 32.0f};
    unitTexCoords[idx] = TexCoord{0.0f, 0.0f, 0.25f, 1.0f};
    unitTargets[idx] = Position{0.0f, 0.0f};
}

void UnitManager::removeUnit(const uint32_t& idx) {
    if (idx >= unitCount) return;

    uint32_t lastIdx = unitCount - 1;

    if (idx != lastIdx) {
        unitIsMovings[idx] = unitIsMovings[lastIdx];
        unitTransforms[idx] = unitTransforms[lastIdx];
        unitTexCoords[idx] = unitTexCoords[lastIdx];
        unitTargets[idx] = unitTargets[lastIdx];
    }

    unitCount--;
}

void UnitManager::_handleDelete(FrameState& frame) {
    KeyboardInput& keyboard = frame.input.keyboard;
    bool keyDelete = keyboard.buttons[SDL_SCANCODE_DELETE];

    // std::cout << selectedCount << std::endl;

    // TODO: why isnt this working?
    //       basically I have to update the indices in selectedIndices after 
    //       removeUnit() is called because it swaps the removed index with
    //       the last index
    if (keyDelete && !keyDeletePrev) {
        for (unsigned int i = 0; i < selectedCount; i++) {
            removeUnit(selectedIndices[i]);
        }
        selectedCount = 0;
    }

    keyDeletePrev = keyDelete;
}

void UnitManager::_handleSelect(FrameState& frame, const Camera& camera) {
    SelectEvent& select = frame.events.select;

    if (select.isActive) {
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

        // compute collisions
        for (unsigned int i = 0; i < unitCount && selectedCount < selectedCapacity; i++) {
            if (checkAABBCollision(boxTransform, unitTransforms[i])) {
                selectedIndices[selectedCount++] = i;
            }
        }

        select.isActive = false;
    }
}

void UnitManager::_handleMovement(FrameState& frame, const Camera& camera) {
    const float dt = frame.dt;
    const MouseInput& mouse = frame.input.mouse;
    const WindowInput& window = frame.input.window;
    bool mouseRight = (mouse.buttons & SDL_BUTTON_RMASK) != 0;

    // update selected units' target position
    if(mouseRight && !mouseRightPrev) {
        glm::vec2 target = camera.screenToWorld(mouse.x, mouse.y, window.width, window.height);
        for (unsigned int i = 0; i < selectedCount; i++) {
            uint32_t& index = selectedIndices[i];
            unitIsMovings[index] = true;
            unitTargets[index] = Position{target.x, target.y};
        }
    }

    // TODO: put these somewhere
    const float speed = 2.0f;
    const float tolerance = 0.1f;
    float distance = speed * dt;

    // move units
    for (unsigned int i = 0; i < unitCount; i++) {
        if (unitIsMovings[i]) {
            Transform& unitTransform = unitTransforms[i];
            const Position& unitTarget = unitTargets[i];

            // move towards target position
            if (unitTransform.x < unitTarget.x) 
                unitTransform.x += distance;
            if (unitTransform.x > unitTarget.x) 
                unitTransform.x -= distance;
            if (unitTransform.y < unitTarget.y) 
                unitTransform.y += distance;
            if (unitTransform.y > unitTarget.y) 
                unitTransform.y -= distance;

            // check if the unit has reached the target and stop unit movement
            if (std::abs(unitTransform.x - unitTarget.x) < tolerance &&
                std::abs(unitTransform.y - unitTarget.y) < tolerance) {
                unitIsMovings[i] = false;
            }
        }
    }

    mouseRightPrev = mouseRight;
}

void UnitManager::update(FrameState& frame, const Camera& camera) {
    _handleDelete(frame);
    _handleSelect(frame, camera);
    _handleMovement(frame, camera);

    // update markers
    markersRenderer.resetCount();
    for (unsigned int i = 0; i < selectedCount; i++) {
        uint32_t& index = selectedIndices[i];
        markersRenderer.appendSubset(1, &unitTransforms[index]);
    }

    // update sprites
    spritesRenderer.resetCount();
    spritesRenderer.appendSubset(unitCount, &unitTransforms[0], &unitTexCoords[0]);
}

void UnitManager::render(const Camera& camera) {
    markersRenderer.render(camera);
    spritesRenderer.render(camera);
}

} // namespace Core