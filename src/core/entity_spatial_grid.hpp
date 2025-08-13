#pragma once

#include "core/types.hpp"
#include "graphics/render/quad_renderer.hpp"
#include "utils/assert.hpp"

#include <array>
#include <vector>

using GridIndex = uint16_t;

constexpr const GridIndex CELL_NULL_INDEX = std::numeric_limits<GridIndex>::max();

struct GridEntityRecord {
    GridIndex cellIndex;
    GridIndex dataIndex;
};

struct Cell {
    std::vector<EntityID> ids;
    // std::vector<BoundingBox> collisionBoxes;
    // std::vector<BoundingBox> selectionBoxes;
    std::vector<BoundingBox> renderXYBoxes;
    // std::vector<BoundingBox> renderUVBoxes;
    std::vector<Vec4f> renderColors; // NOTE: stand-in for renderUVBox for now
};

class SpatialGrid {
public:
    SpatialGrid();

    void insertEntity(
        const EntityID id,
        const Vec2f& position,
        // const BoundingBox& collisionBox,
        // const BoundingBox& selectionBox,
        const BoundingBox& renderXYBox,
        // const BoundingBox& renderUVBox
        const Vec4f& renderColor
    );

    void removeEntity(const EntityID id);
    void translateEntity(const EntityID id, const Vec2f& newPosition);

    void updateRenderData(QuadRenderer& qr);

private:
    GridIndex _getCellIndex(const Vec2f& position);

    size_t numEntities = 0;
    std::vector<GridEntityRecord> records; // NOTE: index is entity id
    std::array<Cell, GRID_CELL_COUNT> cells;
    std::array<BoundingBox, GRID_CELL_COUNT> cellBoundBoxes;
    std::array<Vec4f, GRID_CELL_COUNT> cellBoundColors;
};

SpatialGrid::SpatialGrid() {
    for (int i = 0; i < cells.size(); i++) {
        int x = i % GRID_CELL_COUNT_X;
        int y = i / GRID_CELL_COUNT_X;
        float minX = x * CELL_PIXELS_X;
        float minY = y * CELL_PIXELS_Y;
        float maxX = minX + CELL_PIXELS_X;
        float maxY = minY + CELL_PIXELS_Y;
        cellBoundBoxes[i] = BoundingBox(minX, minY, maxX, maxY);
        cellBoundColors[i] = Vec4f(0.5f, 0.5f, 0.5f, 1.0f);
    }
}

void SpatialGrid::insertEntity(
        const EntityID id,
        const Vec2f& position,
        // const BoundingBox& collisionBox,
        // const BoundingBox& selectionBox,
        const BoundingBox& renderXYBox,
        // const BoundingBox& renderUVBox
        const Vec4f& renderColor
) {
    ASSERT(id < ENTITY_NULL_ID, "Entity id is null.");
    // TODO: assert entity id already exists in cell

    GridIndex cellIndex = _getCellIndex(position);
    Cell& cell = cells[cellIndex];
    GridIndex dataIndex = cell.ids.size();

    BoundingBox newRenderXYBox = renderXYBox;
    newRenderXYBox.translate(position);

    // Add data to cell
    cell.ids.push_back(id);
    cell.renderXYBoxes.push_back(newRenderXYBox);
    cell.renderColors.push_back(renderColor);

    // Grow entity records if necessary
    if (id >= records.size())
        records.resize(id + 128);

    // Add indices to entity records
    records[id] = { cellIndex, dataIndex };
}

void SpatialGrid::removeEntity(const EntityID id) {
    ASSERT(id < ENTITY_NULL_ID, "Invalid entity id.");
    ASSERT(id < records.size(), "Invalid entity id.");

    GridEntityRecord& record = records[id];
    GridIndex cellIndex = record.cellIndex;
    GridIndex dataIndex = record.dataIndex;
    Cell& cell = cells[cellIndex];
    EntityID backID = cell.ids.back();

    swap_and_pop(cell.ids, dataIndex);
    swap_and_pop(cell.renderXYBoxes, dataIndex);
    swap_and_pop(cell.renderColors, dataIndex);

    records[backID].cellIndex = cellIndex;
    records[backID].dataIndex = dataIndex;
    record.cellIndex = ENTITY_NULL_INDEX;
    record.dataIndex = ENTITY_NULL_INDEX;
}

void SpatialGrid::translateEntity(const EntityID id, const Vec2f& newPosition) {
    ASSERT(id < ENTITY_NULL_ID, "Invalid entity id.");
    ASSERT(id < records.size(), "Invalid entity id.");

    GridEntityRecord& record = records[id];
    GridIndex oldCellIndex = record.cellIndex;
    GridIndex oldDataIndex = record.dataIndex;
    Cell& oldCell = cells[oldCellIndex];

    size_t newCellIndex = _getCellIndex(newPosition);

    if (newCellIndex == oldCellIndex) {
        oldCell.renderXYBoxes[oldDataIndex].translate(newPosition);
        return;
    }

    BoundingBox renderXYBox = oldCell.renderXYBoxes[oldDataIndex];
    Vec4f renderColor = oldCell.renderColors[oldDataIndex];
    removeEntity(id);
    insertEntity(id, newPosition, renderXYBox, renderColor);
}

void SpatialGrid::updateRenderData(QuadRenderer& qr) {
    size_t offset = qr.size();
    size_t count = 0;

    // update cell boundary data
    count = GRID_CELL_COUNT;
    qr.update(offset, count, cellBoundBoxes.data(), cellBoundColors.data());
    offset += count;

    // update cell entity data
    for (const auto& cell : cells) {
        count = cell.ids.size();
        if (count > 0)
            qr.update(offset, count, cell.renderXYBoxes.data(), cell.renderColors.data());
        offset += count;
    }
}

// inline size_t _wrapCellCoord(int coord, int cellCount) {
//     return (coord % cellCount + cellCount) % cellCount;
// }

GridIndex SpatialGrid::_getCellIndex(const Vec2f& position) {
    int cellX = static_cast<int>(position.x) / CELL_PIXELS_X;
    int cellY = static_cast<int>(position.y) / CELL_PIXELS_Y;

    // int cellX = _wrapCellCoord(int(position.x) / CELL_PIXELS_X, GRID_CELL_COUNT_X);
    // int cellY = _wrapCellCoord(int(position.y) / CELL_PIXELS_Y, GRID_CELL_COUNT_Y);

    ASSERT(
        cellX >= 0 && cellX < GRID_CELL_COUNT_X &&
        cellY >= 0 && cellY < GRID_CELL_COUNT_Y,
        "Position out of bounds."
    );

    return static_cast<GridIndex>(cellY * GRID_CELL_COUNT_X + cellX);
}