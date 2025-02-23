#pragma once

#include "core/types.hpp"

#include <cstdint>
#include <vector>
#include <unordered_map>

namespace Core {

using ObjectID = uint32_t;

inline int64_t hashGridPos(int gx, int gy) {
    return (static_cast<int64_t>(gx) << 32) | (gy & 0xFFFFFFFF);
}

inline Location worldToGrid(const Position& pos, float cellSize) {
    int gridX = (int)(std::floor(pos.x / cellSize));
    int gridY = (int)(std::floor(pos.y / cellSize));
    return Location{gridX, gridY};
}

struct GridCell {
    std::vector<ObjectID> objects;
};

class UniformGrid {
public:
    UniformGrid(float cellSize) : cellSize(cellSize) {};
    void insert(ObjectID id, const Position& position);
    void remove(ObjectID id, const Position& position);
    std::vector<ObjectID> query(const Position& position);

private:
    float cellSize;

    // hash map for sparse storage
    std::unordered_map<int64_t, GridCell> grid;
};

void UniformGrid::insert(ObjectID id, const Position& position) {
    Location gridIdx = worldToGrid(position, cellSize);
    int64_t key = hashGridPos(gridIdx.x, gridIdx.y);
    grid[key].objects.push_back(id);
}

void UniformGrid::remove(ObjectID id, const Position& position) {
    Location gridIdx = worldToGrid(position, cellSize);
    int64_t key = hashGridPos(gridIdx.x, gridIdx.y);

    auto it = grid.find(key);
    if (it != grid.end()) {
        auto& objects = it->second.objects;
        objects.erase(std::remove(objects.begin(), objects.end(), id), objects.end());
        
        // clean up empty cells
        if (objects.empty()) {
            grid.erase(it);
        }
    }
}

std::vector<ObjectID> UniformGrid::query(const Position& position) {
    std::vector<ObjectID> result;
    Position minPos = {position.x - 1, position.y - 1};
    Position maxPos = {position.x + 1, position.y + 1};
    Location minIdx = worldToGrid(minPos, cellSize);
    Location maxIdx = worldToGrid(maxPos, cellSize);

    for (int gx = minIdx.x; gx <= maxIdx.x; ++gx) {
        for (int gy = minIdx.y; gy <= maxIdx.y; ++gy) {
            int64_t key = hashGridPos(gx, gy);
            if (grid.count(key)) {
                result.insert(result.end(), grid[key].objects.begin(), grid[key].objects.end());
            }
        }
    }
    return result;
}

} // namespace Core